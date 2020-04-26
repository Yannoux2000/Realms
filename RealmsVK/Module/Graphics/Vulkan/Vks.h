#pragma once
#include "Vulkan.h"
#include "../GraphicsManager.h"

#include <fstream>
#include <sstream>
#include <vector>
#include <string>

namespace rlms {
	namespace vks { //VulkanStructures
		std::vector<char> readFile (const std::string& filename);

		VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback (VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);

		inline VkApplicationInfo APPLICATION_INFO (char* AppName, uint32_t& vers) {
			VkApplicationInfo s{};
			s.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
			s.pApplicationName = AppName;
			s.applicationVersion = vers;
			s.pEngineName = AppName;
			s.engineVersion = vers;
			s.apiVersion = VK_API_VERSION_1_0;
			return s;
		}
		inline VkInstanceCreateInfo INSTANCE_CREATE_INFO (VkApplicationInfo* appS, std::vector<char*> ext) {
			VkInstanceCreateInfo s = {};
			s.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
			s.pApplicationInfo = appS;

			s.enabledExtensionCount = static_cast<uint32_t>(ext.size ());
			s.ppEnabledExtensionNames = ext.data ();

			return s;
		}

		inline VkDebugUtilsMessengerCreateInfoEXT DEBUG_UTILS_MESSENGER_CREATE_INFO () {
			VkDebugUtilsMessengerCreateInfoEXT s {};
			s.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;

			s.messageSeverity = 
				VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
				VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
				VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;

			s.messageType = 
				VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
				VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
				VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;

			s.pfnUserCallback = debugCallback;
			return s;
		}

		inline VkDeviceQueueCreateInfo DEVICE_QUEUE_CREATE_INFO (uint32_t qfi, float QPriority) {
			VkDeviceQueueCreateInfo s{};
			s.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			s.queueFamilyIndex = qfi;
			s.queueCount = 1;
			s.pQueuePriorities = &QPriority;
			return s;
		}
		inline VkBufferCreateInfo BUFFER_CREATE_INFO (VkBufferUsageFlags const& usageFlags, VkDeviceSize const& size) {
			VkBufferCreateInfo s{};
			s.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			s.usage = usageFlags;
			s.size = size;
			return s;
		}
		inline VkMemoryAllocateInfo MEMORY_ALLOCATE_INFO () {
			VkMemoryAllocateInfo s{};
			s.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			return s;
		}
		inline VkMappedMemoryRange MAPPED_MEMORY_RANGE (VkDeviceSize const& size, VkDeviceMemory* const& memory) {
			VkMappedMemoryRange s{};
			s.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
			s.memory = *memory;
			s.offset = 0;
			s.size = size;
			return s;
		}

		inline VkCommandBufferAllocateInfo COMMAND_BUFFER_ALLOCATE_INFO (
			VkCommandPool commandPool,
			VkCommandBufferLevel level,
			uint32_t bufferCount) {
			VkCommandBufferAllocateInfo s{};
			s.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			s.commandPool = commandPool;
			s.level = level;
			s.commandBufferCount = bufferCount;
			return s;
		}

		inline VkFenceCreateInfo FENCE_CREATE_INFO (VkFenceCreateFlags flags = 0) {
			VkFenceCreateInfo s{};
			s.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
			s.flags = flags;
			return s;
		}
		inline VkSemaphoreCreateInfo SEMAPHORE_CREATE_INFO () {
			VkSemaphoreCreateInfo semaphoreCreateInfo{};
			semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
			return semaphoreCreateInfo;
		}

		inline VkDescriptorSetLayoutBinding DESCRIPTOR_SET_LAYOUT_CREATE_INFO (
			VkDescriptorType type,
			VkShaderStageFlags stageFlags,
			uint32_t binding,
			uint32_t descriptorCount = 1) {
			VkDescriptorSetLayoutBinding setLayoutBinding{};
			setLayoutBinding.descriptorType = type;
			setLayoutBinding.stageFlags = stageFlags;
			setLayoutBinding.binding = binding;
			setLayoutBinding.descriptorCount = descriptorCount;
			return setLayoutBinding;
		}
		inline VkDescriptorSetLayoutCreateInfo DESCRIPTOR_SET_LAYOUT_CREATE_INFO (
			const VkDescriptorSetLayoutBinding* pBindings,
			uint32_t bindingCount) {
			VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo{};
			descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
			descriptorSetLayoutCreateInfo.pBindings = pBindings;
			descriptorSetLayoutCreateInfo.bindingCount = bindingCount;
			return descriptorSetLayoutCreateInfo;
		}

		inline VkPipelineLayoutCreateInfo PIPELINE_LAYOUT_CREATE_INFO (
			const VkDescriptorSetLayout* pSetLayouts,
			uint32_t setLayoutCount = 1) {
			VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
			pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
			pipelineLayoutCreateInfo.setLayoutCount = setLayoutCount;
			pipelineLayoutCreateInfo.pSetLayouts = pSetLayouts;
			return pipelineLayoutCreateInfo;
		}


		inline VkPipelineVertexInputStateCreateInfo pipelineVertexInputStateCreateInfo () {
			VkPipelineVertexInputStateCreateInfo pipelineVertexInputStateCreateInfo{};
			pipelineVertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
			return pipelineVertexInputStateCreateInfo;
		}

		inline VkPipelineInputAssemblyStateCreateInfo PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO (
			VkPrimitiveTopology topology,
			VkPipelineInputAssemblyStateCreateFlags flags,
			VkBool32 primitiveRestartEnable) {
			VkPipelineInputAssemblyStateCreateInfo pipelineInputAssemblyStateCreateInfo{};
			pipelineInputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
			pipelineInputAssemblyStateCreateInfo.topology = topology;
			pipelineInputAssemblyStateCreateInfo.flags = flags;
			pipelineInputAssemblyStateCreateInfo.primitiveRestartEnable = primitiveRestartEnable;
			return pipelineInputAssemblyStateCreateInfo;
		}

		inline VkPipelineRasterizationStateCreateInfo PIPELINE_RASTERIZATION_STATE_CREATE_INFO (
			VkPolygonMode polygonMode,
			VkCullModeFlags cullMode,
			VkFrontFace frontFace,
			VkPipelineRasterizationStateCreateFlags flags = 0) {
			VkPipelineRasterizationStateCreateInfo pipelineRasterizationStateCreateInfo{};
			pipelineRasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
			pipelineRasterizationStateCreateInfo.polygonMode = polygonMode;
			pipelineRasterizationStateCreateInfo.cullMode = cullMode;
			pipelineRasterizationStateCreateInfo.frontFace = frontFace;
			pipelineRasterizationStateCreateInfo.flags = flags;
			pipelineRasterizationStateCreateInfo.depthClampEnable = VK_FALSE;
			pipelineRasterizationStateCreateInfo.lineWidth = 1.0f;
			return pipelineRasterizationStateCreateInfo;
		}

		inline VkPipelineColorBlendAttachmentState pipelineColorBlendAttachmentState (
			VkColorComponentFlags colorWriteMask,
			VkBool32 blendEnable) {
			VkPipelineColorBlendAttachmentState pipelineColorBlendAttachmentState{};
			pipelineColorBlendAttachmentState.colorWriteMask = colorWriteMask;
			pipelineColorBlendAttachmentState.blendEnable = blendEnable;
			return pipelineColorBlendAttachmentState;
		}

		inline VkPipelineColorBlendStateCreateInfo pipelineColorBlendStateCreateInfo (
			uint32_t attachmentCount,
			const VkPipelineColorBlendAttachmentState* pAttachments) {
			VkPipelineColorBlendStateCreateInfo pipelineColorBlendStateCreateInfo{};
			pipelineColorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
			pipelineColorBlendStateCreateInfo.attachmentCount = attachmentCount;
			pipelineColorBlendStateCreateInfo.pAttachments = pAttachments;
			return pipelineColorBlendStateCreateInfo;
		}

		inline VkPipelineDepthStencilStateCreateInfo pipelineDepthStencilStateCreateInfo (
			VkBool32 depthTestEnable,
			VkBool32 depthWriteEnable,
			VkCompareOp depthCompareOp) {
			VkPipelineDepthStencilStateCreateInfo pipelineDepthStencilStateCreateInfo{};
			pipelineDepthStencilStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
			pipelineDepthStencilStateCreateInfo.depthTestEnable = depthTestEnable;
			pipelineDepthStencilStateCreateInfo.depthWriteEnable = depthWriteEnable;
			pipelineDepthStencilStateCreateInfo.depthCompareOp = depthCompareOp;
			pipelineDepthStencilStateCreateInfo.back.compareOp = VK_COMPARE_OP_ALWAYS;
			return pipelineDepthStencilStateCreateInfo;
		}

		inline VkPipelineViewportStateCreateInfo pipelineViewportStateCreateInfo (
			uint32_t viewportCount,
			uint32_t scissorCount,
			VkPipelineViewportStateCreateFlags flags = 0) {
			VkPipelineViewportStateCreateInfo pipelineViewportStateCreateInfo{};
			pipelineViewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
			pipelineViewportStateCreateInfo.viewportCount = viewportCount;
			pipelineViewportStateCreateInfo.scissorCount = scissorCount;
			pipelineViewportStateCreateInfo.flags = flags;
			return pipelineViewportStateCreateInfo;
		}

		inline VkPipelineMultisampleStateCreateInfo pipelineMultisampleStateCreateInfo (
			VkSampleCountFlagBits rasterizationSamples,
			VkPipelineMultisampleStateCreateFlags flags = 0) {
			VkPipelineMultisampleStateCreateInfo pipelineMultisampleStateCreateInfo{};
			pipelineMultisampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
			pipelineMultisampleStateCreateInfo.rasterizationSamples = rasterizationSamples;
			pipelineMultisampleStateCreateInfo.flags = flags;
			return pipelineMultisampleStateCreateInfo;
		}

		inline VkPipelineDynamicStateCreateInfo pipelineDynamicStateCreateInfo (
			const VkDynamicState* pDynamicStates,
			uint32_t dynamicStateCount,
			VkPipelineDynamicStateCreateFlags flags = 0) {
			VkPipelineDynamicStateCreateInfo pipelineDynamicStateCreateInfo{};
			pipelineDynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
			pipelineDynamicStateCreateInfo.pDynamicStates = pDynamicStates;
			pipelineDynamicStateCreateInfo.dynamicStateCount = dynamicStateCount;
			pipelineDynamicStateCreateInfo.flags = flags;
			return pipelineDynamicStateCreateInfo;
		}

		inline VkPipelineDynamicStateCreateInfo pipelineDynamicStateCreateInfo (
			const std::vector<VkDynamicState>& pDynamicStates,
			VkPipelineDynamicStateCreateFlags flags = 0) {
			VkPipelineDynamicStateCreateInfo pipelineDynamicStateCreateInfo{};
			pipelineDynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
			pipelineDynamicStateCreateInfo.pDynamicStates = pDynamicStates.data ();
			pipelineDynamicStateCreateInfo.dynamicStateCount = static_cast<uint32_t>(pDynamicStates.size ());
			pipelineDynamicStateCreateInfo.flags = flags;
			return pipelineDynamicStateCreateInfo;
		}

		inline VkPipelineTessellationStateCreateInfo pipelineTessellationStateCreateInfo (uint32_t patchControlPoints) {
			VkPipelineTessellationStateCreateInfo pipelineTessellationStateCreateInfo{};
			pipelineTessellationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
			pipelineTessellationStateCreateInfo.patchControlPoints = patchControlPoints;
			return pipelineTessellationStateCreateInfo;
		}

		inline VkGraphicsPipelineCreateInfo pipelineCreateInfo (
			VkPipelineLayout layout,
			VkRenderPass renderPass,
			VkPipelineCreateFlags flags = 0) {
			VkGraphicsPipelineCreateInfo pipelineCreateInfo{};
			pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
			pipelineCreateInfo.layout = layout;
			pipelineCreateInfo.renderPass = renderPass;
			pipelineCreateInfo.flags = flags;
			pipelineCreateInfo.basePipelineIndex = -1;
			pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
			return pipelineCreateInfo;
		}

		inline VkGraphicsPipelineCreateInfo pipelineCreateInfo () {
			VkGraphicsPipelineCreateInfo pipelineCreateInfo{};
			pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
			pipelineCreateInfo.basePipelineIndex = -1;
			pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
			return pipelineCreateInfo;
		}

		inline VkComputePipelineCreateInfo computePipelineCreateInfo (
			VkPipelineLayout layout,
			VkPipelineCreateFlags flags = 0) {
			VkComputePipelineCreateInfo computePipelineCreateInfo{};
			computePipelineCreateInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
			computePipelineCreateInfo.layout = layout;
			computePipelineCreateInfo.flags = flags;
			return computePipelineCreateInfo;
		}


		inline VkCommandPoolCreateInfo COMMAND_POOL_CREATE_INFO (uint32_t const& queueFamilyIndex, VkCommandPoolCreateFlags const& flags) {
			VkCommandPoolCreateInfo s = {};
			s.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			s.queueFamilyIndex = queueFamilyIndex;
			s.flags = flags;
			return s;
		}
		inline VkCommandBufferAllocateInfo COMMAND_BUFFER_ALLOCATE_INFO (VkCommandBufferLevel level, VkCommandPool pool) {
			VkCommandBufferAllocateInfo s{};
			s.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			s.commandPool = pool;
			s.level = level;
			s.commandBufferCount = 1;
			return s;
		}
		inline VkCommandBufferBeginInfo COMMAND_BUFFER_BEGIN_INFO () {
			VkCommandBufferBeginInfo cmdBufferBeginInfo{};
			cmdBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			return cmdBufferBeginInfo;
		}

		inline VkShaderModuleCreateInfo SHADER_MODULE_CREATE_INFO (std::vector<char> const& code) {
			VkShaderModuleCreateInfo s{};
			s.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
			s.codeSize = code.size ();
			s.pCode = reinterpret_cast<const uint32_t*>(code.data ());
			return s;
		}
		inline VkPipelineShaderStageCreateInfo PIPELINE_SHADER_STAGE_CREATE_INFO (VkShaderStageFlagBits const& stage, VkShaderModule const& shaderModule) {
			VkPipelineShaderStageCreateInfo s{};
			s.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			s.stage = stage;
			s.module = shaderModule;
			s.pName = "main";
			return s;
		}

		inline VkDescriptorPoolSize descriptorPoolSize (
			VkDescriptorType type,
			uint32_t descriptorCount) {
			VkDescriptorPoolSize descriptorPoolSize{};
			descriptorPoolSize.type = type;
			descriptorPoolSize.descriptorCount = descriptorCount;
			return descriptorPoolSize;
		}

		inline VkDescriptorPoolCreateInfo DESCRIPTOR_POOL_CREATE_INFO (
			uint32_t poolSizeCount,
			VkDescriptorPoolSize* pPoolSizes,
			uint32_t maxSets) {
			VkDescriptorPoolCreateInfo descriptorPoolInfo{};
			descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
			descriptorPoolInfo.poolSizeCount = poolSizeCount;
			descriptorPoolInfo.pPoolSizes = pPoolSizes;
			descriptorPoolInfo.maxSets = maxSets;
			return descriptorPoolInfo;
		}


		inline VkDescriptorSetAllocateInfo DESCRIPTOR_SET_ALLOCATE_INFO (
			VkDescriptorPool descriptorPool,
			const VkDescriptorSetLayout* pSetLayouts,
			uint32_t descriptorSetCount) {
			VkDescriptorSetAllocateInfo descriptorSetAllocateInfo{};
			descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			descriptorSetAllocateInfo.descriptorPool = descriptorPool;
			descriptorSetAllocateInfo.pSetLayouts = pSetLayouts;
			descriptorSetAllocateInfo.descriptorSetCount = descriptorSetCount;
			return descriptorSetAllocateInfo;
		}

		inline VkWriteDescriptorSet WRITE_DESCRIPTOR_SET (
			VkDescriptorSet dstSet,
			VkDescriptorType type,
			uint32_t binding,
			VkDescriptorBufferInfo* bufferInfo,
			uint32_t descriptorCount = 1) {
			VkWriteDescriptorSet writeDescriptorSet{};
			writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			writeDescriptorSet.dstSet = dstSet;
			writeDescriptorSet.descriptorType = type;
			writeDescriptorSet.dstBinding = binding;
			writeDescriptorSet.pBufferInfo = bufferInfo;
			writeDescriptorSet.descriptorCount = descriptorCount;
			return writeDescriptorSet;
		}

		inline VkWriteDescriptorSet WRITE_DESCRIPTOR_SET (
			VkDescriptorSet dstSet,
			VkDescriptorType type,
			uint32_t binding,
			VkDescriptorImageInfo* imageInfo,
			uint32_t descriptorCount = 1) {
			VkWriteDescriptorSet writeDescriptorSet{};
			writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			writeDescriptorSet.dstSet = dstSet;
			writeDescriptorSet.descriptorType = type;
			writeDescriptorSet.dstBinding = binding;
			writeDescriptorSet.pImageInfo = imageInfo;
			writeDescriptorSet.descriptorCount = descriptorCount;
			return writeDescriptorSet;
		}

		inline VkRenderPassBeginInfo RENDER_PASS_BEGIN_INFO () {
			VkRenderPassBeginInfo renderPassBeginInfo{};
			renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			return renderPassBeginInfo;
		}

		inline VkViewport viewport (
			float width,
			float height,
			float minDepth,
			float maxDepth) {
			VkViewport viewport{};
			viewport.width = width;
			viewport.height = height;
			viewport.minDepth = minDepth;
			viewport.maxDepth = maxDepth;
			return viewport;
		}

		inline VkRect2D rect2D (
			int32_t width,
			int32_t height,
			int32_t offsetX,
			int32_t offsetY) {
			VkRect2D rect2D{};
			rect2D.extent.width = width;
			rect2D.extent.height = height;
			rect2D.offset.x = offsetX;
			rect2D.offset.y = offsetY;
			return rect2D;
		}

		inline VkSubmitInfo SUBMIT_INFO () {
			VkSubmitInfo submitInfo{};
			submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			return submitInfo;
		}
	}
}