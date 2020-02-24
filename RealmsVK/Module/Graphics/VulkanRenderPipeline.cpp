#include "VulkanRenderPipeline.h"

#include <fstream>

using namespace rlms;

std::vector<char> readFile (const std::string& filename) {
	std::ifstream file (filename, std::ios::ate | std::ios::binary);

	if (!file.is_open ()) {
		throw std::runtime_error ("failed to open file!");
	}

	size_t fileSize = (size_t)file.tellg ();
	std::vector<char> buffer (fileSize);

	file.seekg (0);
	file.read (buffer.data (), fileSize);

	file.close ();

	return buffer;
}

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
