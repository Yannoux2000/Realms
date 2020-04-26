#pragma once
#include "../IRenderer.h"
#include "../AspectRatio.h"

#include "Vulkan.h"

#include "VulkanDevice.h"
#include "VulkanSwapChain.h"

#include "Vks.h"
#include <array>

namespace rlms {
	class IVulkanRenderer : public IRenderer {
	protected:
		VulkanDevice* device;
		VulkanSwapChain* swapChain;
		VulkanQueues* queues;

		// List of shader modules created (stored for cleanup)
		std::vector<VkShaderModule> shaderModules;

		// Global render pass for frame buffer writes
		VkRenderPass renderPass;

		// List of available frame buffers (same as number of swap chain images)
		std::vector<VkFramebuffer> frameBuffers;
		// Active frame buffer index
		uint32_t currentBuffer = 0;
		// Descriptor set pool
		VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
		// Pipeline cache object
		VkPipelineCache pipelineCache;

		struct {
			// Depth buffer format
			VkFormat format;

			VkImage image;
			VkDeviceMemory mem;
			VkImageView view;
		} depthStencil;

		VkClearColorValue defaultClearColor = { { 0.025f, 0.025f, 0.025f, 1.0f } };

		virtual void DeviceSetup (VkPhysicalDeviceFeatures& enabledFeatures, std::vector<const char*>& enabledDeviceExtensions, void*& deviceCreatepNextChain) {

		}

	public:
		IVulkanRenderer() {}
		virtual ~IVulkanRenderer () {};

		//TODO : pas besoin de VulkanContext au final ? mais besoin de VulkanSwapChain
		void connect (VulkanSwapChain* r_swapchain, VulkanDevice* r_device, VulkanQueues* r_queues) {
			swapChain = r_swapchain;
			device = r_device;
			queues = r_queues;
		}

		virtual VkQueueFlags requestedQueueTypes () {
			return VK_QUEUE_GRAPHICS_BIT; //by default request for graphics queue only
		}

		void initDevice () {
			VkPhysicalDeviceFeatures enFeatures{};
			std::vector<const char*> enDeviceExt;
			void* pNext = nullptr;

			DeviceSetup (enFeatures, enDeviceExt, pNext); //if i use any of thiese in the future

			queues->requested = requestedQueueTypes ();
			device->queryQueueFamilyIndex (*swapChain);
			device->createLogicalDevice (enFeatures, enDeviceExt, pNext);
		}

		VkShaderModule loadShader (std::vector<char> const& code) {
			VkShaderModuleCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
			createInfo.codeSize = code.size ();
			createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data ());

			VkShaderModule shaderModule;
			if (vkCreateShaderModule (*device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
				throw std::runtime_error ("failed to load shader module!");
			}

			shaderModules.push_back (shaderModule);
			return shaderModule;
		}

		VkCommandBuffer createCommandBuffer (VkCommandBufferLevel level, VkCommandPool commandPool, bool begin) {
			VkCommandBuffer cmdBuffer;

			VkCommandBufferAllocateInfo commandBufferAllocateInfo{};
				commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
				commandBufferAllocateInfo.commandPool = commandPool;
				commandBufferAllocateInfo.level = level;
				commandBufferAllocateInfo.commandBufferCount = 1;

			VulkanResultCheck (vkAllocateCommandBuffers (*device, &commandBufferAllocateInfo, &cmdBuffer));

			// If requested, also start the new command buffer
			if (begin) {
				VkCommandBufferBeginInfo cmdBufInfo{};
				cmdBufInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
				VulkanResultCheck (vkBeginCommandBuffer (cmdBuffer, &cmdBufInfo));
			}

			return cmdBuffer;
		}
		bool checkCommandBuffer (VkCommandBuffer commandBuffer) {
			if (commandBuffer == VK_NULL_HANDLE) {
				return false;
			}
			return true;
		}
		bool checkCommandBuffers (std::vector<VkCommandBuffer> commandBuffers) {
			for (auto& cmdBuffer : commandBuffers) {
				if (cmdBuffer == VK_NULL_HANDLE) {
					return false;
				}
			}
			return true;
		}
		void flushCommandBuffer (VkCommandBuffer commandBuffer, VkQueue queue, VkCommandPool commandPool, bool free) {
			if (commandBuffer == VK_NULL_HANDLE) {
				return;
			}

			VulkanResultCheck (vkEndCommandBuffer (commandBuffer));

			VkSubmitInfo submitInfo = {};
			submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			submitInfo.commandBufferCount = 1;
			submitInfo.pCommandBuffers = &commandBuffer;

			VulkanResultCheck (vkQueueSubmit (queue, 1, &submitInfo, VK_NULL_HANDLE));
			VulkanResultCheck (vkQueueWaitIdle (queue));

			if (free) {
				vkFreeCommandBuffers (*device, commandPool, 1, &commandBuffer);
			}
		}
	};
}
