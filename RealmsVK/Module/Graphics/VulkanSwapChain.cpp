#include "VulkanSwapChain.h"
#include "AspectRatio.h"
#include "VulkanException.h"
#include <algorithm>

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

		actualExtent.width = std::max (capabilities.minImageExtent.width, std::min (capabilities.maxImageExtent.width, actualExtent.width));
		actualExtent.height = std::max (capabilities.minImageExtent.height, std::min (capabilities.maxImageExtent.height, actualExtent.height));

		return actualExtent;
	}
}

SwapChainSupportDetails VulkanSwapChain::QuerySwapChainSupport (VkPhysicalDevice const& device, VkSurfaceKHR const& surface) {
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

void VulkanSwapChain::createSwapChain (VulkanDevice& device, VkSurfaceKHR& surface) {
	SwapChainSupportDetails support = QuerySwapChainSupport (device.getVkPhysicalDevice (), surface);

	VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat (support.formats);
	VkPresentModeKHR presentMode = chooseSwapPresentMode (support.presentModes);
	VkExtent2D extent = chooseSwapExtent (support.capabilities);

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

	QueueFamilyIndices indices = device.getQueueFamilies ();
	uint32_t queueFamilyIndices[] = { indices.graphicsFamily, indices.presentFamily };

	if (indices.graphicsFamily != indices.presentFamily) {
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	} else {
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	}

	createInfo.preTransform = support.capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;

	createInfo.oldSwapchain = VK_NULL_HANDLE;

	if (vkCreateSwapchainKHR (device.getDevice (), &createInfo, nullptr, &(_swapChain)) != VK_SUCCESS) {
		throw FailedSwapChain ();
	}

	vkGetSwapchainImagesKHR (device.getDevice (), _swapChain, &imageCount, nullptr);
	_images.resize (imageCount);
	vkGetSwapchainImagesKHR (device.getDevice (), _swapChain, &imageCount, _images.data ());

	_imageFormat = surfaceFormat.format;
	_extent = extent;
}

void VulkanSwapChain::createImageViews (VulkanDevice& device) {
	_imageViews.resize (_images.size ());

	for (size_t i = 0; i < _images.size (); i++) {
		VkImageViewCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = _images[i];
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = _imageFormat;
		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;

		if (vkCreateImageView (device.getDevice (), &createInfo, nullptr, &(_imageViews[i])) != VK_SUCCESS) {
			throw FailedImageViews ();
		}
	}
}

void VulkanSwapChain::createFramebuffers (VulkanDevice& device, VkRenderPass& renderPass) {
	_framebuffers.resize (_imageViews.size ());

	for (size_t i = 0; i < _imageViews.size (); i++) {
		VkImageView attachments[] = {
			_imageViews[i]
		};

		VkFramebufferCreateInfo framebufferInfo = {};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = renderPass;
		framebufferInfo.attachmentCount = 1;
		framebufferInfo.pAttachments = attachments;
		framebufferInfo.width = _extent.width;
		framebufferInfo.height = _extent.height;
		framebufferInfo.layers = 1;

		if (vkCreateFramebuffer (device.getDevice (), &framebufferInfo, nullptr, &(_framebuffers[i])) != VK_SUCCESS) {
			throw FailedFramebuffer ();
		}
	}
}
