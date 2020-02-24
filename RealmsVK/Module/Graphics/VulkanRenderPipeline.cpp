#include "VulkanRenderPipeline.h"

using namespace rlms;

SwapChainSupportDetails VulkanRenderPipeline::QueryExtentionsSupport (VkPhysicalDevice const& device, VkSurfaceKHR const& surface) {
	SwapChainSupportDetails details;

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR (device, surface, &details.capabilities);

	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR (device, surface, &formatCount, nullptr);

	if (formatCount != 0) {
		details.formats.resize (formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR (device, surface, &formatCount, details.formats.data ());
	}

	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR (device, surface, &presentModeCount, nullptr);

	if (presentModeCount != 0) {
		details.presentModes.resize (presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR (device, surface, &presentModeCount, details.presentModes.data ());
	}

	return details;
}
