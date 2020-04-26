#pragma once
#include "IVulkanRenderer.h"
#include "VulkanTexture.h"
#include "VulkanBuffer.h"

namespace rlms {
	class VulkanComputeRenderer : public IVulkanRenderer {
	private:

	// Resources for the graphics part of the example
		struct {
			VkCommandPool commandPool;					// Use a separate command pool (queue family may differ from the one used for graphics)
			std::vector<VkCommandBuffer> commandBuffers;// Command buffer storing the dispatch commands and barriers

			VkDescriptorSetLayout descriptorSetLayout;	// Raytraced image display shader binding layout
			VkDescriptorSet descriptorSetPreCompute;	// Raytraced image display shader bindings before compute shader image manipulation
			VkDescriptorSet descriptorSet;				// Raytraced image display shader bindings after compute shader image manipulation
			VkPipeline pipeline;						// Raytraced image display pipeline
			VkPipelineLayout pipelineLayout;			// Layout of the graphics pipeline
		} graphics;

		// Resources for the compute part of the example
		struct {
			struct {
				VulkanBuffer spheres;						// (Shader) storage buffer object with scene spheres
				VulkanBuffer planes;						// (Shader) storage buffer object with scene planes
			} storageBuffers;
			VulkanBuffer uniformBuffer;					// Uniform buffer object containing scene data
			VkCommandPool commandPool;					// Use a separate command pool (queue family may differ from the one used for graphics)
			VkCommandBuffer commandBuffer;				// Command buffer storing the dispatch commands and barriers

			VkFence fence;								// Synchronization fence to avoid rewriting compute CB if still in use

			VkDescriptorSetLayout descriptorSetLayout;	// Compute shader binding layout
			VkDescriptorSet descriptorSet;				// Compute shader bindings
			VkPipelineLayout pipelineLayout;			// Layout of the compute pipeline
			VkPipeline pipeline;						// Compute raytracing pipeline

			struct UBOCompute {							// Compute shader uniform block object
				glm::vec3 lightPos;
				float aspectRatio;						// Aspect ratio of the viewport
				glm::vec4 fogColor = glm::vec4 (0.0f);
				struct {
					glm::vec3 pos = glm::vec3 (0.0f, 0.0f, 4.0f);
					glm::vec3 lookat = glm::vec3 (0.0f, 0.5f, 0.0f);
					float fov = 10.0f;
				} camera;
			} ubo;
		} compute;

		VkPipelineStageFlags submitPipelineStages = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		VkFormat depthFormat;
	// SSBO sphere declaration 
		struct Sphere {									// Shader uses std140 layout (so we only use vec4 instead of vec3)
			glm::vec3 pos;
			float radius;
			glm::vec3 diffuse;
			float specular;
			uint32_t id;								// Id used to identify sphere for raytracing
			glm::ivec3 _pad;
		};

		// SSBO plane declaration
		struct Plane {
			glm::vec3 normal;
			float distance;
			glm::vec3 diffuse;
			float specular;
			uint32_t id;
			glm::ivec3 _pad;
		};

		VulkanTexture textureComputeTarget;
		std::vector<VkFence> waitFences;

		struct {
			// Swap chain image presentation
			VkSemaphore presentComplete;
			// Command buffer submission and execution
			VkSemaphore renderComplete;
		} semaphores;
	public:
		~VulkanComputeRenderer () {
		}

		VkQueueFlags requestedQueueTypes () override {
			return VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT; //by default request for graphics queue only
		}

		void start () override {
			createCommandPool ();
			createCommandBuffers ();
			createSynchronizationPrimitives ();
			setupDepthStencil ();
			setupRenderPass ();
			createPipelineCache ();
			setupFrameBuffer ();

			prepareStorageBuffers ();
			prepareUniformBuffers ();
			prepareTextureTarget (&textureComputeTarget, 1024, 1024, VK_FORMAT_R8G8B8A8_UNORM);

			setupDescriptorSetLayout ();
			preparePipelines ();
			setupDescriptorPool ();
			setupDescriptorSet ();
			prepareCompute ();
			buildCommandBuffers ();
		}

		void stop () override {
		// Graphics
			vkDestroyPipeline (*device, graphics.pipeline, nullptr);
			vkDestroyPipelineLayout (*device, graphics.pipelineLayout, nullptr);
			vkDestroyDescriptorSetLayout (*device, graphics.descriptorSetLayout, nullptr);
			destroyCommandBuffers ();
			vkDestroyCommandPool (*device, graphics.commandPool, nullptr);

			// Compute
			vkDestroyPipeline (*device, compute.pipeline, nullptr);
			vkDestroyPipelineLayout (*device, compute.pipelineLayout, nullptr);
			vkDestroyDescriptorSetLayout (*device, compute.descriptorSetLayout, nullptr);
			vkDestroyCommandPool (*device, compute.commandPool, nullptr);

			compute.uniformBuffer.destroy ();
			compute.storageBuffers.spheres.destroy ();
			compute.storageBuffers.planes.destroy ();

			textureComputeTarget.destroy ();

			if (descriptorPool != VK_NULL_HANDLE) {
				vkDestroyDescriptorPool (*device, descriptorPool, nullptr);
			}
			vkDestroyRenderPass (*device, renderPass, nullptr);
			for (uint32_t i = 0; i < frameBuffers.size (); i++) {
				vkDestroyFramebuffer (*device, frameBuffers[i], nullptr);
			}

			for (auto& shaderModule : shaderModules) {
				vkDestroyShaderModule (*device, shaderModule, nullptr);
			}
			vkDestroyImageView (*device, depthStencil.view, nullptr);
			vkDestroyImage (*device, depthStencil.image, nullptr);
			vkFreeMemory (*device, depthStencil.mem, nullptr);

			vkDestroyPipelineCache (*device, pipelineCache, nullptr);

			vkDestroyFence (*device, compute.fence, nullptr);
			vkDestroySemaphore (*device, semaphores.presentComplete, nullptr);
			vkDestroySemaphore (*device, semaphores.renderComplete, nullptr);
			for (auto& fence : waitFences) {
				vkDestroyFence (*device, fence, nullptr);
			}
		}

		void createCommandPool () {
			VkCommandPoolCreateInfo cmdPoolInfo = {};
			cmdPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			cmdPoolInfo.queueFamilyIndex = queues->getGraphics ();
			cmdPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
			VulkanResultCheck (vkCreateCommandPool (*device, &cmdPoolInfo, nullptr, &graphics.commandPool));

			cmdPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			cmdPoolInfo.queueFamilyIndex = queues->getCompute ();
			cmdPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
			VulkanResultCheck (vkCreateCommandPool (*device, &cmdPoolInfo, nullptr, &compute.commandPool));
		}

		void createCommandBuffers () {
			// Create one command buffer for each swap chain image and reuse for rendering
			graphics.commandBuffers.resize (swapChain->buffers.size());

			VkCommandBufferAllocateInfo cmdBufAllocateInfo =
				vks::COMMAND_BUFFER_ALLOCATE_INFO (
				graphics.commandPool,
				VK_COMMAND_BUFFER_LEVEL_PRIMARY,
				static_cast<uint32_t>(graphics.commandBuffers.size ()));

			VulkanResultCheck (vkAllocateCommandBuffers (*device, &cmdBufAllocateInfo, graphics.commandBuffers.data ()));
		}

		bool checkCommandBuffers () {
			for (auto& cmdBuffer : graphics.commandBuffers) {
				if (cmdBuffer == VK_NULL_HANDLE) {
					return false;
				}
			}
			return true;
		}

		void destroyCommandBuffers () {
			vkFreeCommandBuffers (*device, graphics.commandPool, static_cast<uint32_t>(graphics.commandBuffers.size ()), graphics.commandBuffers.data ());
		}

		void createSynchronizationPrimitives () {
			// Wait fences to sync command buffer access
			VkFenceCreateInfo fenceCreateInfo = vks::FENCE_CREATE_INFO ((VkFenceCreateFlags)VK_FENCE_CREATE_SIGNALED_BIT);
			waitFences.resize (graphics.commandBuffers.size ());
			for (auto& fence : waitFences) {
				VulkanResultCheck (vkCreateFence (*device, &fenceCreateInfo, nullptr, &fence));
			}

			// Create synchronization objects
			VkSemaphoreCreateInfo semaphoreCreateInfo = vks::SEMAPHORE_CREATE_INFO ();
			// Create a semaphore used to synchronize image presentation
			// Ensures that the image is displayed before we start submitting new commands to the queue
			VulkanResultCheck (vkCreateSemaphore (*device, &semaphoreCreateInfo, nullptr, &semaphores.presentComplete));
			// Create a semaphore used to synchronize command submission
			// Ensures that the image is not presented until all commands have been sumbitted and executed
			VulkanResultCheck (vkCreateSemaphore (*device, &semaphoreCreateInfo, nullptr, &semaphores.renderComplete));
		}

		void setupDepthStencil () {
			depthFormat = device->getSupportedDepthFormat ();

			VkImageCreateInfo imageCI{};
			imageCI.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			imageCI.imageType = VK_IMAGE_TYPE_2D;
			imageCI.format = depthFormat;
			imageCI.extent = { swapChain->extent.width, swapChain->extent.height, 1 };
			imageCI.mipLevels = 1;
			imageCI.arrayLayers = 1;
			imageCI.samples = VK_SAMPLE_COUNT_1_BIT;
			imageCI.tiling = VK_IMAGE_TILING_OPTIMAL;
			imageCI.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;

			VulkanResultCheck(vkCreateImage (*device, &imageCI, nullptr, &depthStencil.image));
			VkMemoryRequirements memReqs{};
			vkGetImageMemoryRequirements (*device, depthStencil.image, &memReqs);

			VkMemoryAllocateInfo memAllloc{};
			memAllloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			memAllloc.allocationSize = memReqs.size;
			memAllloc.memoryTypeIndex = device->getMemoryType (memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
			VulkanResultCheck (vkAllocateMemory (*device, &memAllloc, nullptr, &depthStencil.mem));
			VulkanResultCheck (vkBindImageMemory (*device, depthStencil.image, depthStencil.mem, 0));

			VkImageViewCreateInfo imageViewCI{};
			imageViewCI.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			imageViewCI.viewType = VK_IMAGE_VIEW_TYPE_2D;
			imageViewCI.image = depthStencil.image;
			imageViewCI.format = depthFormat;
			imageViewCI.subresourceRange.baseMipLevel = 0;
			imageViewCI.subresourceRange.levelCount = 1;
			imageViewCI.subresourceRange.baseArrayLayer = 0;
			imageViewCI.subresourceRange.layerCount = 1;
			imageViewCI.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
			// Stencil aspect should only be set on depth + stencil formats (VK_FORMAT_D16_UNORM_S8_UINT..VK_FORMAT_D32_SFLOAT_S8_UINT
			if (depthFormat >= VK_FORMAT_D16_UNORM_S8_UINT) {
				imageViewCI.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
			}
			VulkanResultCheck (vkCreateImageView (*device, &imageViewCI, nullptr, &depthStencil.view));
		}


		void setupRenderPass () {
			std::array<VkAttachmentDescription, 2> attachments = {};
			// Color attachment
			attachments[0].format = swapChain->colorFormat;
			attachments[0].samples = VK_SAMPLE_COUNT_1_BIT;
			attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
			attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			attachments[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			attachments[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
			// Depth attachment
			attachments[1].format = depthFormat;
			attachments[1].samples = VK_SAMPLE_COUNT_1_BIT;
			attachments[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			attachments[1].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
			attachments[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			attachments[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			attachments[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			attachments[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

			VkAttachmentReference colorReference = {};
			colorReference.attachment = 0;
			colorReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

			VkAttachmentReference depthReference = {};
			depthReference.attachment = 1;
			depthReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

			VkSubpassDescription subpassDescription = {};
			subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
			subpassDescription.colorAttachmentCount = 1;
			subpassDescription.pColorAttachments = &colorReference;
			subpassDescription.pDepthStencilAttachment = &depthReference;
			subpassDescription.inputAttachmentCount = 0;
			subpassDescription.pInputAttachments = nullptr;
			subpassDescription.preserveAttachmentCount = 0;
			subpassDescription.pPreserveAttachments = nullptr;
			subpassDescription.pResolveAttachments = nullptr;

			// Subpass dependencies for layout transitions
			std::array<VkSubpassDependency, 2> dependencies;

			dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
			dependencies[0].dstSubpass = 0;
			dependencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
			dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			dependencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
			dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

			dependencies[1].srcSubpass = 0;
			dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
			dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			dependencies[1].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
			dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			dependencies[1].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
			dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

			VkRenderPassCreateInfo renderPassInfo = {};
			renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
			renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size ());
			renderPassInfo.pAttachments = attachments.data ();
			renderPassInfo.subpassCount = 1;
			renderPassInfo.pSubpasses = &subpassDescription;
			renderPassInfo.dependencyCount = static_cast<uint32_t>(dependencies.size ());
			renderPassInfo.pDependencies = dependencies.data ();

			VulkanResultCheck(vkCreateRenderPass (*device, &renderPassInfo, nullptr, &renderPass));
		}

		void createPipelineCache () {
			VkPipelineCacheCreateInfo pipelineCacheCreateInfo = {};
			pipelineCacheCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
			VulkanResultCheck(vkCreatePipelineCache (*device, &pipelineCacheCreateInfo, nullptr, &pipelineCache));
		}

		void setupFrameBuffer () {

			VkImageView attachments[2];

			// Depth/Stencil attachment is the same for all frame buffers
			attachments[1] = depthStencil.view;

			VkFramebufferCreateInfo frameBufferCreateInfo = {};
			frameBufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			frameBufferCreateInfo.pNext = NULL;
			frameBufferCreateInfo.renderPass = renderPass;
			frameBufferCreateInfo.attachmentCount = 2;
			frameBufferCreateInfo.pAttachments = attachments;
			frameBufferCreateInfo.width = swapChain->extent.width;
			frameBufferCreateInfo.height = swapChain->extent.height;
			frameBufferCreateInfo.layers = 1;

			// Create frame buffers for every swap chain image
			frameBuffers.resize (swapChain->buffers.size());
			for (uint32_t i = 0; i < frameBuffers.size (); i++) {
				attachments[0] = swapChain->buffers[i].view;
				VulkanResultCheck (vkCreateFramebuffer (*device, &frameBufferCreateInfo, nullptr, &frameBuffers[i]));
			}
		}


		uint32_t currentId = 0;	// Id used to identify objects by the ray tracing shader

		Sphere newSphere (glm::vec3 pos, float radius, glm::vec3 diffuse, float specular) {
			Sphere sphere;
			sphere.id = currentId++;
			sphere.pos = pos;
			sphere.radius = radius;
			sphere.diffuse = diffuse;
			sphere.specular = specular;
			return sphere;
		}

		Plane newPlane (glm::vec3 normal, float distance, glm::vec3 diffuse, float specular) {
			Plane plane;
			plane.id = currentId++;
			plane.normal = normal;
			plane.distance = distance;
			plane.diffuse = diffuse;
			plane.specular = specular;
			return plane;
		}

		// Setup and fill the compute shader storage buffers containing primitives for the raytraced scene
		void prepareStorageBuffers () {

			// Spheres
			std::vector<Sphere> spheres;
			spheres.push_back (newSphere (glm::vec3 (1.75f, -0.5f, 0.0f), 1.0f, glm::vec3 (0.0f, 1.0f, 0.0f), 32.0f));
			spheres.push_back (newSphere (glm::vec3 (0.0f, 1.0f, -0.5f), 1.0f, glm::vec3 (0.65f, 0.77f, 0.97f), 32.0f));
			spheres.push_back (newSphere (glm::vec3 (-1.75f, -0.75f, -0.5f), 1.25f, glm::vec3 (0.9f, 0.76f, 0.46f), 32.0f));
			VkDeviceSize storageBufferSize = spheres.size () * sizeof (Sphere);

			// Stage
			VulkanBuffer stagingBuffer;

			device->createBuffer (
				VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
				&stagingBuffer,
				storageBufferSize,
				spheres.data ());

			device->createBuffer (
				// The SSBO will be used as a storage buffer for the compute pipeline and as a vertex buffer in the graphics pipeline
				VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
				&compute.storageBuffers.spheres,
				storageBufferSize);

			// Copy to staging buffer
			VkCommandBuffer copyCmd = IVulkanRenderer::createCommandBuffer (VK_COMMAND_BUFFER_LEVEL_PRIMARY, compute.commandPool, true);
			VkBufferCopy copyRegion = {};
			copyRegion.size = storageBufferSize;
			vkCmdCopyBuffer (copyCmd, stagingBuffer, compute.storageBuffers.spheres, 1, &copyRegion);
			IVulkanRenderer::flushCommandBuffer (copyCmd, queues->compute, compute.commandPool, true);

			stagingBuffer.destroy ();

			// Planes
			std::vector<Plane> planes;
			const float roomDim = 4.0f;
			planes.push_back (newPlane (glm::vec3 (0.0f, 1.0f, 0.0f), roomDim, glm::vec3 (1.0f), 32.0f));
			planes.push_back (newPlane (glm::vec3 (0.0f, -1.0f, 0.0f), roomDim, glm::vec3 (1.0f), 32.0f));
			planes.push_back (newPlane (glm::vec3 (0.0f, 0.0f, 1.0f), roomDim, glm::vec3 (1.0f), 32.0f));
			planes.push_back (newPlane (glm::vec3 (0.0f, 0.0f, -1.0f), roomDim, glm::vec3 (0.0f), 32.0f));
			planes.push_back (newPlane (glm::vec3 (-1.0f, 0.0f, 0.0f), roomDim, glm::vec3 (1.0f, 0.0f, 0.0f), 32.0f));
			planes.push_back (newPlane (glm::vec3 (1.0f, 0.0f, 0.0f), roomDim, glm::vec3 (0.0f, 1.0f, 0.0f), 32.0f));
			storageBufferSize = planes.size () * sizeof (Plane);

			// Stage
			device->createBuffer (
				VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
				&stagingBuffer,
				storageBufferSize,
				planes.data ());

			device->createBuffer (
				// The SSBO will be used as a storage buffer for the compute pipeline and as a vertex buffer in the graphics pipeline
				VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
				&compute.storageBuffers.planes,
				storageBufferSize);

			// Copy to staging buffer
			copyCmd = IVulkanRenderer::createCommandBuffer (VK_COMMAND_BUFFER_LEVEL_PRIMARY, compute.commandPool, true);
			copyRegion.size = storageBufferSize;
			vkCmdCopyBuffer (copyCmd, stagingBuffer, compute.storageBuffers.planes, 1, &copyRegion);
			IVulkanRenderer::flushCommandBuffer (copyCmd, queues->compute, compute.commandPool, true);

			stagingBuffer.destroy ();
		}

		// Prepare and initialize uniform buffer containing shader uniforms
		void prepareUniformBuffers () {
			// Compute shader parameter uniform buffer block
			device->createBuffer (
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
				&compute.uniformBuffer,
				sizeof (compute.ubo));

			updateUniformBuffers ();
		}

		void updateUniformBuffers () {
			compute.ubo.lightPos.x = 0.0f + sin (glm::radians (10.0f)) * cos (glm::radians (10.0f)) * 2.0f;
			compute.ubo.lightPos.y = 0.0f + sin (glm::radians (10.0f)) * 2.0f;
			compute.ubo.lightPos.z = 0.0f + cos (glm::radians (10.0f)) * 2.0f;
			VulkanResultCheck (compute.uniformBuffer.map ());
			memcpy (compute.uniformBuffer.mapped, &compute.ubo, sizeof (compute.ubo));
			compute.uniformBuffer.unmap ();
		}

		// Prepare a texture target that is used to store compute shader calculations
		void prepareTextureTarget (VulkanTexture* tex, uint32_t width, uint32_t height, VkFormat format) {
			// Get device properties for the requested texture format
			VkFormatProperties formatProperties;
			vkGetPhysicalDeviceFormatProperties (*device, format, &formatProperties);
			// Check if requested image format supports image storage operations
			assert (formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT);

			// Prepare blit target texture
			tex->width = width;
			tex->height = height;

			VkImageCreateInfo imageCreateInfo{};
			imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
			imageCreateInfo.format = format;
			imageCreateInfo.extent = { width, height, 1 };
			imageCreateInfo.mipLevels = 1;
			imageCreateInfo.arrayLayers = 1;
			imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
			imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
			imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			// Image will be sampled in the fragment shader and used as storage target in the compute shader
			imageCreateInfo.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_STORAGE_BIT;
			imageCreateInfo.flags = 0;

			VkMemoryAllocateInfo memAllocInfo{};
			memAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			VkMemoryRequirements memReqs;

			VulkanResultCheck (vkCreateImage (*device, &imageCreateInfo, nullptr, &tex->image));
			vkGetImageMemoryRequirements (*device, tex->image, &memReqs);
			memAllocInfo.allocationSize = memReqs.size;
			memAllocInfo.memoryTypeIndex = device->getMemoryType (memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
			VulkanResultCheck (vkAllocateMemory (*device, &memAllocInfo, nullptr, &tex->deviceMemory));
			VulkanResultCheck (vkBindImageMemory (*device, tex->image, tex->deviceMemory, 0));

			VkCommandBuffer layoutCmd = IVulkanRenderer::createCommandBuffer (VK_COMMAND_BUFFER_LEVEL_PRIMARY, graphics.commandPool, true);

			tex->imageLayout = VK_IMAGE_LAYOUT_GENERAL;

			VkImageSubresourceRange subresourceRange = {};
			subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			subresourceRange.baseMipLevel = 0;
			subresourceRange.levelCount = 1;
			subresourceRange.layerCount = 1;

			// Create an image barrier object
			VkImageMemoryBarrier imageMemoryBarrier{};
			imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			imageMemoryBarrier.newLayout = tex->imageLayout;
			imageMemoryBarrier.image = tex->image;
			imageMemoryBarrier.subresourceRange = subresourceRange;
			imageMemoryBarrier.srcAccessMask = 0;

			// Put barrier inside setup command buffer
			vkCmdPipelineBarrier (
				layoutCmd,
				VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
				VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
				0,
				0, nullptr,
				0, nullptr,
				1, &imageMemoryBarrier);

			IVulkanRenderer::flushCommandBuffer (layoutCmd, queues->graphics, graphics.commandPool, true);

			// Create sampler
			VkSamplerCreateInfo sampler{};
			sampler.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
			sampler.magFilter = VK_FILTER_LINEAR;
			sampler.minFilter = VK_FILTER_LINEAR;
			sampler.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
			sampler.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
			sampler.addressModeV = sampler.addressModeU;
			sampler.addressModeW = sampler.addressModeU;
			sampler.mipLodBias = 0.0f;
			sampler.maxAnisotropy = 1.0f;
			sampler.compareOp = VK_COMPARE_OP_NEVER;
			sampler.minLod = 0.0f;
			sampler.maxLod = 0.0f;
			sampler.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
			VulkanResultCheck (vkCreateSampler (*device, &sampler, nullptr, &tex->sampler));

			// Create image view
			VkImageViewCreateInfo view{};
			view.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			view.viewType = VK_IMAGE_VIEW_TYPE_2D;
			view.format = format;
			view.components = { VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A };
			view.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
			view.image = tex->image;
			VulkanResultCheck (vkCreateImageView (*device, &view, nullptr, &tex->view));

			// Initialize a descriptor for later use
			tex->descriptor.imageLayout = tex->imageLayout;
			tex->descriptor.imageView = tex->view;
			tex->descriptor.sampler = tex->sampler;
			tex->device = *device;
		}

		void setupDescriptorSetLayout () {
			std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings =
			{
				// Binding 0 : Fragment shader image sampler
				vks::DESCRIPTOR_SET_LAYOUT_CREATE_INFO (
					VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
					VK_SHADER_STAGE_FRAGMENT_BIT,
					0)
			};

			VkDescriptorSetLayoutCreateInfo descriptorLayout =
				vks::DESCRIPTOR_SET_LAYOUT_CREATE_INFO (
				setLayoutBindings.data (),
				static_cast<uint32_t>(setLayoutBindings.size ()));

			VulkanResultCheck (vkCreateDescriptorSetLayout (*device, &descriptorLayout, nullptr, &graphics.descriptorSetLayout));

			VkPipelineLayoutCreateInfo pPipelineLayoutCreateInfo =
				vks::PIPELINE_LAYOUT_CREATE_INFO (
				&graphics.descriptorSetLayout,
				1);

			VulkanResultCheck (vkCreatePipelineLayout (*device, &pPipelineLayoutCreateInfo, nullptr, &graphics.pipelineLayout));
		}

		void preparePipelines () {
			VkPipelineInputAssemblyStateCreateInfo inputAssemblyState =
				vks::PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO (
				VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
				0,
				VK_FALSE);

			VkPipelineRasterizationStateCreateInfo rasterizationState =
				vks::PIPELINE_RASTERIZATION_STATE_CREATE_INFO (
				VK_POLYGON_MODE_FILL,
				VK_CULL_MODE_FRONT_BIT,
				VK_FRONT_FACE_COUNTER_CLOCKWISE,
				0);

			VkPipelineColorBlendAttachmentState blendAttachmentState =
				vks::pipelineColorBlendAttachmentState (
				0xf,
				VK_FALSE);

			VkPipelineColorBlendStateCreateInfo colorBlendState =
				vks::pipelineColorBlendStateCreateInfo (
				1,
				&blendAttachmentState);

			VkPipelineDepthStencilStateCreateInfo depthStencilState =
				vks::pipelineDepthStencilStateCreateInfo (
				VK_FALSE,
				VK_FALSE,
				VK_COMPARE_OP_LESS_OR_EQUAL);

			VkPipelineViewportStateCreateInfo viewportState =
				vks::pipelineViewportStateCreateInfo (1, 1, 0);

			VkPipelineMultisampleStateCreateInfo multisampleState =
				vks::pipelineMultisampleStateCreateInfo (
				VK_SAMPLE_COUNT_1_BIT,
				0);

			std::vector<VkDynamicState> dynamicStateEnables = {
				VK_DYNAMIC_STATE_VIEWPORT,
				VK_DYNAMIC_STATE_SCISSOR
			};
			VkPipelineDynamicStateCreateInfo dynamicState =
				vks::pipelineDynamicStateCreateInfo (
				dynamicStateEnables.data (),
				dynamicStateEnables.size (),
				static_cast<uint32_t>(0));

		// Display pipeline
			std::array<VkPipelineShaderStageCreateInfo, 2> shaderStages;

			VkShaderModule vertShaderModule = loadShader (vks::readFile ("Ressources/computeraytracing/texture.vert.spv"));
			VkShaderModule fragShaderModule = loadShader (vks::readFile ("Ressources/computeraytracing/texture.frag.spv"));

			VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
			vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
			vertShaderStageInfo.module = vertShaderModule;
			vertShaderStageInfo.pName = "main";
			shaderStages[0] = vertShaderStageInfo;

			VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
			fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
			fragShaderStageInfo.module = fragShaderModule;
			fragShaderStageInfo.pName = "main";
			shaderStages[1] = fragShaderStageInfo;

			VkGraphicsPipelineCreateInfo pipelineCreateInfo =
				vks::pipelineCreateInfo (
				graphics.pipelineLayout,
				renderPass,
				0);

			VkPipelineVertexInputStateCreateInfo emptyInputState{};
			emptyInputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
			emptyInputState.vertexAttributeDescriptionCount = 0;
			emptyInputState.pVertexAttributeDescriptions = nullptr;
			emptyInputState.vertexBindingDescriptionCount = 0;
			emptyInputState.pVertexBindingDescriptions = nullptr;
			pipelineCreateInfo.pVertexInputState = &emptyInputState;

			pipelineCreateInfo.pInputAssemblyState = &inputAssemblyState;
			pipelineCreateInfo.pRasterizationState = &rasterizationState;
			pipelineCreateInfo.pColorBlendState = &colorBlendState;
			pipelineCreateInfo.pMultisampleState = &multisampleState;
			pipelineCreateInfo.pViewportState = &viewportState;
			pipelineCreateInfo.pDepthStencilState = &depthStencilState;
			pipelineCreateInfo.pDynamicState = &dynamicState;
			pipelineCreateInfo.stageCount = static_cast<uint32_t>(shaderStages.size ());
			pipelineCreateInfo.pStages = shaderStages.data ();
			pipelineCreateInfo.renderPass = renderPass;

			VulkanResultCheck (vkCreateGraphicsPipelines (*device, pipelineCache, 1, &pipelineCreateInfo, nullptr, &graphics.pipeline));
		}

		void setupDescriptorPool () {
			std::vector<VkDescriptorPoolSize> poolSizes =
			{
				vks::descriptorPoolSize (VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 2),			// Compute UBO
				vks::descriptorPoolSize (VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 4),	// Graphics image samplers
				vks::descriptorPoolSize (VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1),				// Storage image for ray traced image output
				vks::descriptorPoolSize (VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 2),			// Storage buffer for the scene primitives
			};

			VkDescriptorPoolCreateInfo descriptorPoolInfo =
				vks::DESCRIPTOR_POOL_CREATE_INFO (
				poolSizes.size (),
				poolSizes.data (),
				static_cast<uint32_t>(3));

			VulkanResultCheck (vkCreateDescriptorPool (*device, &descriptorPoolInfo, nullptr, &descriptorPool));
		}

		void setupDescriptorSet () {
			VkDescriptorSetAllocateInfo allocInfo =
				vks::DESCRIPTOR_SET_ALLOCATE_INFO (
				descriptorPool,
				&graphics.descriptorSetLayout,
				1);

			VulkanResultCheck(vkAllocateDescriptorSets (*device, &allocInfo, &graphics.descriptorSet));

			std::vector<VkWriteDescriptorSet> writeDescriptorSets =
			{
				// Binding 0 : Fragment shader texture sampler
				vks::WRITE_DESCRIPTOR_SET (
					graphics.descriptorSet,
					VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
					0,
					&textureComputeTarget.descriptor)
			};

			vkUpdateDescriptorSets (*device, static_cast<uint32_t>(writeDescriptorSets.size ()), writeDescriptorSets.data (), 0, NULL);
		}

		// Prepare the compute pipeline that generates the ray traced image
		void prepareCompute () {
			// Create a compute capable device queue
			// The VulkanDevice::createLogicalDevice functions finds a compute capable queue and prefers queue families that only support compute
			// Depending on the implementation this may result in different queue family indices for graphics and computes,
			// requiring proper synchronization (see the memory barriers in buildComputeCommandBuffer)

			std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings = {
				// Binding 0: Storage image (raytraced output)
				vks::DESCRIPTOR_SET_LAYOUT_CREATE_INFO (
					VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
					VK_SHADER_STAGE_COMPUTE_BIT,
					0),
				// Binding 1: Uniform buffer block
				vks::DESCRIPTOR_SET_LAYOUT_CREATE_INFO (
					VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
					VK_SHADER_STAGE_COMPUTE_BIT,
					1),
				// Binding 1: Shader storage buffer for the spheres
				vks::DESCRIPTOR_SET_LAYOUT_CREATE_INFO (
					VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
					VK_SHADER_STAGE_COMPUTE_BIT,
					2),
				// Binding 1: Shader storage buffer for the planes
				vks::DESCRIPTOR_SET_LAYOUT_CREATE_INFO (
					VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
					VK_SHADER_STAGE_COMPUTE_BIT,
					3)
			};

			VkDescriptorSetLayoutCreateInfo descriptorLayout =
				vks::DESCRIPTOR_SET_LAYOUT_CREATE_INFO (
				setLayoutBindings.data (),
				static_cast<uint32_t>(setLayoutBindings.size ()));

			VulkanResultCheck (vkCreateDescriptorSetLayout (*device, &descriptorLayout, nullptr, &compute.descriptorSetLayout));

			VkPipelineLayoutCreateInfo pPipelineLayoutCreateInfo =
				vks::PIPELINE_LAYOUT_CREATE_INFO (
				&compute.descriptorSetLayout,
				1);

			VulkanResultCheck (vkCreatePipelineLayout (*device, &pPipelineLayoutCreateInfo, nullptr, &compute.pipelineLayout));

			VkDescriptorSetAllocateInfo allocInfo =
				vks::DESCRIPTOR_SET_ALLOCATE_INFO (
				descriptorPool,
				&compute.descriptorSetLayout,
				1);

			VulkanResultCheck (vkAllocateDescriptorSets (*device, &allocInfo, &compute.descriptorSet));

			std::vector<VkWriteDescriptorSet> computeWriteDescriptorSets =
			{
				// Binding 0: Output storage image
				vks::WRITE_DESCRIPTOR_SET (
					compute.descriptorSet,
					VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
					0,
					&textureComputeTarget.descriptor),
				// Binding 1: Uniform buffer block
				vks::WRITE_DESCRIPTOR_SET (
					compute.descriptorSet,
					VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
					1,
					compute.uniformBuffer),
				// Binding 2: Shader storage buffer for the spheres
				vks::WRITE_DESCRIPTOR_SET (
					compute.descriptorSet,
					VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
					2,
					compute.storageBuffers.spheres),
				// Binding 2: Shader storage buffer for the planes
				vks::WRITE_DESCRIPTOR_SET (
					compute.descriptorSet,
					VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
					3,
					compute.storageBuffers.planes)
			};

			vkUpdateDescriptorSets (*device, computeWriteDescriptorSets.size (), computeWriteDescriptorSets.data (), 0, NULL);

			// Create compute shader pipelines
			VkComputePipelineCreateInfo computePipelineCreateInfo =
				vks::computePipelineCreateInfo (
				compute.pipelineLayout,
				0);

			VkShaderModule compShaderModule = loadShader (vks::readFile ("Ressources/computeraytracing/raytracing.comp.spv"));

			VkPipelineShaderStageCreateInfo compShaderStageInfo{};
			compShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			compShaderStageInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
			compShaderStageInfo.module = compShaderModule;
			compShaderStageInfo.pName = "main";


			computePipelineCreateInfo.stage = compShaderStageInfo;
			VulkanResultCheck (vkCreateComputePipelines (*device, pipelineCache, 1, &computePipelineCreateInfo, nullptr, &compute.pipeline));

			// Separate command pool as queue family for compute may be different than graphics
			VkCommandPoolCreateInfo cmdPoolInfo = {};
			cmdPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			cmdPoolInfo.queueFamilyIndex = queues->getCompute();
			cmdPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
			VulkanResultCheck (vkCreateCommandPool (*device, &cmdPoolInfo, nullptr, &compute.commandPool));

			// Create a command buffer for compute operations
			VkCommandBufferAllocateInfo cmdBufAllocateInfo =
				vks::COMMAND_BUFFER_ALLOCATE_INFO (
				compute.commandPool,
				VK_COMMAND_BUFFER_LEVEL_PRIMARY,
				1);

			VulkanResultCheck (vkAllocateCommandBuffers (*device, &cmdBufAllocateInfo, &compute.commandBuffer));

			// Fence for compute CB sync
			VkFenceCreateInfo fenceCreateInfo = vks::FENCE_CREATE_INFO (VK_FENCE_CREATE_SIGNALED_BIT);
			VulkanResultCheck (vkCreateFence (*device, &fenceCreateInfo, nullptr, &compute.fence));

			// Build a single command buffer containing the compute dispatch commands
			buildComputeCommandBuffer ();
		}

		void buildComputeCommandBuffer () {
			VkCommandBufferBeginInfo cmdBufInfo = vks::COMMAND_BUFFER_BEGIN_INFO ();

			VulkanResultCheck (vkBeginCommandBuffer (compute.commandBuffer, &cmdBufInfo));

			vkCmdBindPipeline (compute.commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, compute.pipeline);
			vkCmdBindDescriptorSets (compute.commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, compute.pipelineLayout, 0, 1, &compute.descriptorSet, 0, 0);

			vkCmdDispatch (compute.commandBuffer, textureComputeTarget.width / 16, textureComputeTarget.height / 16, 1);

			vkEndCommandBuffer (compute.commandBuffer);
		}

		void buildCommandBuffers () {
			// Destroy command buffers if already present
			if (!checkCommandBuffers ()) {
				destroyCommandBuffers ();
				createCommandBuffers ();
			}

			VkCommandBufferBeginInfo cmdBufInfo = vks::COMMAND_BUFFER_BEGIN_INFO ();

			VkClearValue clearValues[2];
			clearValues[0].color = defaultClearColor;
			clearValues[1].depthStencil = { 1.0f, 0 };

			VkRenderPassBeginInfo renderPassBeginInfo = vks::RENDER_PASS_BEGIN_INFO ();
			renderPassBeginInfo.renderPass = renderPass;
			renderPassBeginInfo.renderArea.offset.x = 0;
			renderPassBeginInfo.renderArea.offset.y = 0;
			renderPassBeginInfo.renderArea.extent.width = swapChain->extent.width;
			renderPassBeginInfo.renderArea.extent.height = swapChain->extent.height;
			renderPassBeginInfo.clearValueCount = 2;
			renderPassBeginInfo.pClearValues = clearValues;

			for (int32_t i = 0; i < graphics.commandBuffers.size (); ++i) {
				// Set target frame buffer
				renderPassBeginInfo.framebuffer = frameBuffers[i];

				VulkanResultCheck (vkBeginCommandBuffer (graphics.commandBuffers[i], &cmdBufInfo));

				// Image memory barrier to make sure that compute shader writes are finished before sampling from the texture
				VkImageMemoryBarrier imageMemoryBarrier = {};
				imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
				imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_GENERAL;
				imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_GENERAL;
				imageMemoryBarrier.image = textureComputeTarget.image;
				imageMemoryBarrier.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
				imageMemoryBarrier.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
				imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
				vkCmdPipelineBarrier (
					graphics.commandBuffers[i],
					VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
					VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
					0,
					0, nullptr,
					0, nullptr,
					1, &imageMemoryBarrier);

				vkCmdBeginRenderPass (graphics.commandBuffers[i], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

				VkViewport viewport = vks::viewport ((float)swapChain->extent.width, (float)swapChain->extent.height, 0.0f, 1.0f);
				vkCmdSetViewport (graphics.commandBuffers[i], 0, 1, &viewport);

				VkRect2D scissor = vks::rect2D (swapChain->extent.width, swapChain->extent.height, 0, 0);
				vkCmdSetScissor (graphics.commandBuffers[i], 0, 1, &scissor);

				// Display ray traced image generated by compute shader as a full screen quad
				// Quad vertices are generated in the vertex shader
				vkCmdBindDescriptorSets (graphics.commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphics.pipelineLayout, 0, 1, &graphics.descriptorSet, 0, NULL);
				vkCmdBindPipeline (graphics.commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphics.pipeline);
				vkCmdDraw (graphics.commandBuffers[i], 3, 1, 0, 0);

				vkCmdEndRenderPass (graphics.commandBuffers[i]);

				VulkanResultCheck (vkEndCommandBuffer (graphics.commandBuffers[i]));
			}
		}

		void draw () override {

			vkAcquireNextImageKHR (*device, *swapChain, UINT64_MAX, semaphores.presentComplete, (VkFence)nullptr, &currentBuffer);
			
			// Command buffer to be sumitted to the queue

			VkSubmitInfo submitInfo = vks::SUBMIT_INFO ();
			submitInfo.pWaitDstStageMask = &submitPipelineStages;
			submitInfo.waitSemaphoreCount = 1;
			submitInfo.pWaitSemaphores = &semaphores.presentComplete;
			submitInfo.signalSemaphoreCount = 1;
			submitInfo.pSignalSemaphores = &semaphores.renderComplete;
			submitInfo.commandBufferCount = 1;
			submitInfo.pCommandBuffers = &graphics.commandBuffers[currentBuffer];
			VulkanResultCheck (vkQueueSubmit (queues->graphics, 1, &submitInfo, VK_NULL_HANDLE));

			VkPresentInfoKHR presentInfo = {};
			presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
			presentInfo.pNext = NULL;
			presentInfo.swapchainCount = 1;
			presentInfo.pSwapchains = *swapChain;
			presentInfo.pImageIndices = &currentBuffer;
			// Check if a wait semaphore has been specified to wait for before presenting the image
			if (semaphores.renderComplete != VK_NULL_HANDLE) {
				presentInfo.pWaitSemaphores = &semaphores.renderComplete;
				presentInfo.waitSemaphoreCount = 1;
			}

			VulkanResultCheck(vkQueuePresentKHR (queues->present, &presentInfo));
			VulkanResultCheck (vkQueueWaitIdle (queues->present));

			// Submit compute commands
			// Use a fence to ensure that compute command buffer has finished executing before using it again
			vkWaitForFences (*device, 1, &compute.fence, VK_TRUE, UINT64_MAX);
			vkResetFences (*device, 1, &compute.fence);

			VkSubmitInfo computeSubmitInfo = vks::SUBMIT_INFO ();
			computeSubmitInfo.commandBufferCount = 1;
			computeSubmitInfo.pCommandBuffers = &compute.commandBuffer;

			VulkanResultCheck (vkQueueSubmit (queues->compute, 1, &computeSubmitInfo, compute.fence));

			updateUniformBuffers ();
		}
	};
}
