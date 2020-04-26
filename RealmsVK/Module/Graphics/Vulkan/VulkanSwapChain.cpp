#define GLFW_INCLUDE_VULKAN

#include "VulkanSwapChain.h"
#include "../AspectRatio.h"
#include "VulkanException.h"
#include <algorithm>
#include <vector>

using namespace rlms;

VkSurfaceFormatKHR VulkanSwapChain::chooseSwapSurfaceFormat (const std::vector<VkSurfaceFormatKHR>& availableFormats) {

	for (const auto& availableFormat : availableFormats) {
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
			return availableFormat;
		}
	}

	return availableFormats[0];
}
VkPresentModeKHR VulkanSwapChain::chooseSwapPresentMode (const std::vector<VkPresentModeKHR>& availablePresentModes) {
	for (const auto& availablePresentMode : availablePresentModes) {
		if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
			return availablePresentMode;
		}
	}

	return VK_PRESENT_MODE_FIFO_KHR;
}
VkExtent2D VulkanSwapChain::chooseSwapExtent (const VkSurfaceCapabilitiesKHR& capabilities) {
	if (capabilities.currentExtent.width != UINT32_MAX) {
		return capabilities.currentExtent;
	} else {
		VkExtent2D actualExtent = { AspectRatio::Width () , AspectRatio::Height () };

		//clamping to a capable size
		actualExtent.width = std::max (capabilities.minImageExtent.width, std::min (capabilities.maxImageExtent.width, actualExtent.width));
		actualExtent.height = std::max (capabilities.minImageExtent.height, std::min (capabilities.maxImageExtent.height, actualExtent.height));

		AspectRatio::Set (actualExtent.width, actualExtent.height);
		return actualExtent;
	}
}
VkCompositeAlphaFlagBitsKHR VulkanSwapChain::chooseCompositeAlpha (const VkSurfaceCapabilitiesKHR& capabilities) {
	// Simply select the first composite alpha format available
	std::vector<VkCompositeAlphaFlagBitsKHR> compositeAlphaFlags = {
		VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
		VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR,
		VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR,
		VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR,
	};

	for (auto& compositeAlphaFlag : compositeAlphaFlags) {
		if (capabilities.supportedCompositeAlpha & compositeAlphaFlag) {
			return compositeAlphaFlag;
			break;
		};
	}
}

VulkanSwapChain::SwapChainSupportDetails VulkanSwapChain::QuerySwapChainSupport (VkPhysicalDevice const& device, VkSurfaceKHR const& surface) {
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

/** @brief Creates the platform surface window used for presentation */

VulkanSwapChain::VulkanSwapChain (VkInstance& r_instance) : instance (r_instance), swapChain(VK_NULL_HANDLE) {}

void rlms::VulkanSwapChain::InitSurface (GLFWwindow*& window) {
	VulkanResultCheck (glfwCreateWindowSurface (instance, window, nullptr, &surface));
}

bool VulkanSwapChain::PhysicalDeviceCompatible (VkPhysicalDevice& device) {
	SwapChainSupportDetails details = QuerySwapChainSupport (device, surface);
	return !details.formats.empty () && !details.presentModes.empty ();
}

void VulkanSwapChain::initSwapChain (VulkanDevice*& device, VulkanQueues*& queues) {
	std::vector<uint32_t> qfIdx = queues->getUnique ();
	SwapChainSupportDetails support = QuerySwapChainSupport (*device, surface);

	VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat (support.formats);
	VkPresentModeKHR presentMode = chooseSwapPresentMode (support.presentModes);
	extent = chooseSwapExtent (support.capabilities);
	VkCompositeAlphaFlagBitsKHR compositeAlpha = chooseCompositeAlpha (support.capabilities);

	uint32_t imageCount = support.capabilities.minImageCount + 1;
	if (support.capabilities.maxImageCount > 0 && imageCount > support.capabilities.maxImageCount) {
		imageCount = support.capabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = surface;

	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;


	if (qfIdx.size ()>1) {
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = qfIdx.size();
		createInfo.pQueueFamilyIndices = qfIdx.data ();
	} else {
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 1;
		createInfo.pQueueFamilyIndices = qfIdx.data ();
	}

	createInfo.preTransform = support.capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;

	createInfo.oldSwapchain = VK_NULL_HANDLE;

	if (vkCreateSwapchainKHR (*device, &createInfo, nullptr, &swapChain) != VK_SUCCESS) { // TODO: add Vk_Result states to Exception details
		throw FailedSwapChain ();
	}

	vkGetSwapchainImagesKHR (*device, swapChain, &imageCount, nullptr);
	images.resize (imageCount);
	vkGetSwapchainImagesKHR (*device, swapChain, &imageCount, images.data ());

	// Get the swap chain buffers containing the image and imageview
	buffers.resize (imageCount);
	for (uint32_t i = 0; i < imageCount; i++) {
		VkImageViewCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.pNext = NULL;
		createInfo.format = surfaceFormat.format;
		createInfo.components = {
			VK_COMPONENT_SWIZZLE_R,
			VK_COMPONENT_SWIZZLE_G,
			VK_COMPONENT_SWIZZLE_B,
			VK_COMPONENT_SWIZZLE_A
		};
		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.flags = 0;

		createInfo.image = images[i];
		buffers[i].image = images[i];

		if (vkCreateImageView (*device, &createInfo, nullptr, &buffers[i].view) != VK_SUCCESS) {
			throw FailedImageViews ();
		}
	}
	colorFormat = surfaceFormat.format;
	extent = extent;
}

void VulkanSwapChain::createFramebuffers (VkDevice& device, VkRenderPass& renderPass, std::vector<VkFramebuffer> framebuffers) {
	for (size_t i = 0; i < buffers.size (); i++) {
		VkImageView attachments[] = {
			buffers[i].view
		};

		VkFramebufferCreateInfo framebufferInfo = {};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = renderPass;
		framebufferInfo.attachmentCount = 1;
		framebufferInfo.pAttachments = attachments;
		framebufferInfo.width = extent.width;
		framebufferInfo.height = extent.height;
		framebufferInfo.layers = 1;

		if (vkCreateFramebuffer (device, &framebufferInfo, nullptr, &(framebuffers[i])) != VK_SUCCESS) {
			throw FailedFramebuffer ();
		}
	}
}
