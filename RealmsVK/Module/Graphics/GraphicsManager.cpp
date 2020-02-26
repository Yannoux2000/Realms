#include "GraphicsManager.h"

#include "../../Base/Logging/ILogged.h"
#include "../../Base/Allocators/IAllocator.h"

#include "../../Utility/FileIO/VoxFileParser.h"

#include "GameRenderer.h"
#include "MeshRegister.h"
#include "ShaderPrototypeLights.h"
#include "StaticMesh.h"
#include "AspectRatio.h"

#include "VulkanEnvironment.h"
#include "VulkanDevice.h"
#include "VulkanRenderPipeline.h"

using namespace rlms;

class DrawingScheduler {
	std::vector<VkSemaphore> imageAvailableSemaphores;
	std::vector<VkSemaphore> renderFinishedSemaphores;
	std::vector<VkFence> inFlightFences;
	std::vector<VkFence> imagesInFlight;
	size_t currentFrame = 0;
};

class rlms::GraphicsManagerImpl : public ILogged {
private:
	friend class GraphicsManager;

	VulkanEnvironment _vulkanEnv;
	VulkanDevice _GDevice;
	VulkanRenderPipeline _renderpipeline;

	VkCommandPool commandPool;
	std::vector<VkCommandBuffer> commandBuffers;

	std::vector<VkSemaphore> imageAvailableSemaphores;
	std::vector<VkSemaphore> renderFinishedSemaphores;
	std::vector<VkFence> inFlightFences;
	std::vector<VkFence> imagesInFlight;
	size_t currentFrame = 0;

	std::string getLogName () override {
		return "GraphicsManager";
	};

	void start (IAllocator* const& alloc, size_t mesh_pool_size, GLFWwindow*& window, std::shared_ptr<Logger> funnel = nullptr);
	void stop ();

	MeshRegister* getRegister ();
	void Register (IMODEL_TYPE_ID const& type_id, std::string&& filename, std::string&& meshAlias);
	IMesh* getMesh (IMODEL_TYPE_ID const& type_id);

	void load ();
	void draw ();
	void unload ();

	void createCommandPool () {
		QueueFamilyIndices queueFamilyIndices = _GDevice.getQueueFamilies();

		VkCommandPoolCreateInfo poolInfo = {};
		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily;
		poolInfo.flags = 0; // Optionel

		if (vkCreateCommandPool (_GDevice.getDevice (), &poolInfo, nullptr, &commandPool) != VK_SUCCESS) {
			throw std::runtime_error ("échec de la création d'une command pool!");
		}
	}

	void createCommandBuffers () {
		commandBuffers.resize (_renderpipeline.getSwapChain().getFramebuffers().size ());

		VkCommandBufferAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = commandPool;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = (uint32_t)commandBuffers.size ();

		if (vkAllocateCommandBuffers (_GDevice.getDevice (), &allocInfo, commandBuffers.data ()) != VK_SUCCESS) {
			throw std::runtime_error ("failed to allocate command buffers!");
		}

		for (size_t i = 0; i < commandBuffers.size (); i++) {
			VkCommandBufferBeginInfo beginInfo = {};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

			if (vkBeginCommandBuffer (commandBuffers[i], &beginInfo) != VK_SUCCESS) {
				throw std::runtime_error ("failed to begin recording command buffer!");
			}

			VkRenderPassBeginInfo renderPassInfo = {};
			renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			renderPassInfo.renderPass = _renderpipeline.getRenderPass();
			renderPassInfo.framebuffer = _renderpipeline.getSwapChain ().getFramebuffers ()[i];
			renderPassInfo.renderArea.offset = { 0, 0 };
			renderPassInfo.renderArea.extent = _renderpipeline.getSwapChain ().getExtent();

			VkClearValue clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
			renderPassInfo.clearValueCount = 1;
			renderPassInfo.pClearValues = &clearColor;

			vkCmdBeginRenderPass (commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

			vkCmdBindPipeline (commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, _renderpipeline.getGraphicsPipeline());

			vkCmdDraw (commandBuffers[i], 3, 1, 0, 0);

			vkCmdEndRenderPass (commandBuffers[i]);

			if (vkEndCommandBuffer (commandBuffers[i]) != VK_SUCCESS) {
				throw std::runtime_error ("failed to record command buffer!");
			}
		}
	}

	void createSyncObjects () {
		imageAvailableSemaphores.resize (MAX_FRAMES_IN_FLIGHT);
		renderFinishedSemaphores.resize (MAX_FRAMES_IN_FLIGHT);
		inFlightFences.resize (MAX_FRAMES_IN_FLIGHT);
		imagesInFlight.resize (_renderpipeline.getSwapChain ().getImages().size (), VK_NULL_HANDLE);

		VkSemaphoreCreateInfo semaphoreInfo = {};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkFenceCreateInfo fenceInfo = {};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
			if (vkCreateSemaphore (_GDevice.getDevice (), &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
				vkCreateSemaphore (_GDevice.getDevice (), &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
				vkCreateFence (_GDevice.getDevice (), &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS) {
				throw std::runtime_error ("failed to create synchronization objects for a frame!");
			}
		}
	}

	void drawFrame () {
		vkWaitForFences (_GDevice.getDevice (), 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

		uint32_t imageIndex;
		vkAcquireNextImageKHR (_GDevice.getDevice (), _renderpipeline.getSwapChain ().getSwapChain(), UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

		if (imagesInFlight[imageIndex] != VK_NULL_HANDLE) {
			vkWaitForFences (_GDevice.getDevice (), 1, &imagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);
		}
		imagesInFlight[imageIndex] = inFlightFences[currentFrame];

		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		VkSemaphore waitSemaphores[] = { imageAvailableSemaphores[currentFrame] };
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;

		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffers[imageIndex];

		VkSemaphore signalSemaphores[] = { renderFinishedSemaphores[currentFrame] };
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		vkResetFences (_GDevice.getDevice (), 1, &inFlightFences[currentFrame]);

		if (vkQueueSubmit (_GDevice.getGfxQueue(), 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS) {
			throw std::runtime_error ("failed to submit draw command buffer!");
		}

		VkPresentInfoKHR presentInfo = {};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;

		VkSwapchainKHR swapChains[] = { _renderpipeline.getSwapChain ().getSwapChain() };
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;

		presentInfo.pImageIndices = &imageIndex;

		vkQueuePresentKHR (_GDevice.getPrtQueue(), &presentInfo);

		currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
	}
};

void rlms::GraphicsManagerImpl::start (IAllocator* const& alloc, size_t mesh_pool_size, GLFWwindow*& window, std::shared_ptr<Logger> funnel) {
	startLogger (funnel);
	logger->tag (LogTags::None) << "Initializing !" << '\n';

	_vulkanEnv.start (window);
	_GDevice.start (&_vulkanEnv.getVkSurfaceKHR (), &_vulkanEnv.getVKInstance ());
	_renderpipeline.start (&_GDevice, &_vulkanEnv.getVkSurfaceKHR());

	createCommandPool ();
	createCommandBuffers ();

	createSyncObjects ();

	logger->tag (LogTags::None) << "Initialized correctly !" << '\n';
}

void rlms::GraphicsManagerImpl::stop () {
	logger->tag (LogTags::None) << "Terminating" << '\n';

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		vkDestroySemaphore (_GDevice.getDevice(), renderFinishedSemaphores[i], nullptr);
		vkDestroySemaphore (_GDevice.getDevice (), imageAvailableSemaphores[i], nullptr);
		vkDestroyFence (_GDevice.getDevice (), inFlightFences[i], nullptr);
	}

	vkDestroyCommandPool (_GDevice.getDevice (), commandPool, nullptr);

	_renderpipeline.stop ();
	_GDevice.stop ();
	_vulkanEnv.stop ();

	logger->tag (LogTags::None) << "Terminated correctly !" << '\n';
}

void rlms::GraphicsManagerImpl::load () {

}

void rlms::GraphicsManagerImpl::draw () {
	drawFrame ();

	vkDeviceWaitIdle (_GDevice.getDevice());
}

void rlms::GraphicsManagerImpl::unload () {}

std::unique_ptr<GraphicsManagerImpl> rlms::GraphicsManager::instance;

std::shared_ptr<LoggerHandler> rlms::GraphicsManager::GetLogger () {
	return instance->getLogger ();
}

void rlms::GraphicsManager::Initialize (IAllocator* const& alloc, size_t mesh_pool_size, GLFWwindow*& window, std::shared_ptr<Logger> funnel) {
	instance = std::make_unique<GraphicsManagerImpl> ();
	instance->start (alloc, mesh_pool_size, window, funnel);
}

void rlms::GraphicsManager::Terminate () {
	instance->stop ();
	instance.reset ();
}

void rlms::GraphicsManager::Load () {
	instance->load ();
}

void rlms::GraphicsManager::Draw () {
	instance->draw ();
}

void rlms::GraphicsManager::Unload () {
	instance->unload ();
}
