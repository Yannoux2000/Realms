#pragma once
#include "Vulkan.h"


namespace rlms {
	struct SwapChainSupportDetails {
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};

	class VulkanRenderPipeline {
	private:

		VkRenderPass renderPass;
		VkPipelineLayout pipelineLayout;
		VkPipeline graphicsPipeline;

		std::vector<VkFramebuffer> swapChainFramebuffers;

	public:
		static SwapChainSupportDetails QueryExtentionsSupport (VkPhysicalDevice const& device, VkSurfaceKHR const& surface);

	};
};