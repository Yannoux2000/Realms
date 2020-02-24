#pragma once

#include "Vulkan.h"
#include "VulkanDevice.h"

namespace rlms {

	struct SwapChainSupportDetails {
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};

	class VulkanSwapChain {
	private:
		VkSwapchainKHR _swapChain;
		std::vector<VkImage> _images;
		VkFormat _imageFormat;
		VkExtent2D _extent;

		std::vector<VkImageView> _imageViews;

		std::vector<VkFramebuffer> _framebuffers;

		VkSurfaceFormatKHR chooseSwapSurfaceFormat (const std::vector<VkSurfaceFormatKHR>& availableFormats);
		VkPresentModeKHR chooseSwapPresentMode (const std::vector<VkPresentModeKHR>& availablePresentModes);
		VkExtent2D chooseSwapExtent (const VkSurfaceCapabilitiesKHR& capabilities);
	public:
		static SwapChainSupportDetails QuerySwapChainSupport (VkPhysicalDevice const& device, VkSurfaceKHR const& surface);

		void createSwapChain (VulkanDevice& device, VkSurfaceKHR& surface);
		void createImageViews (VulkanDevice& device);
		void createFramebuffers (VulkanDevice& device, VkRenderPass& renderPass);

		void destroySwapChain (VkDevice const& device) {
			vkDestroySwapchainKHR (device, _swapChain, nullptr);
		}
		void destroyImageViews (VkDevice const& device) {
			for (auto imageView : _imageViews) {
				vkDestroyImageView (device, imageView, nullptr);
			}
		}
		void destroyFramebuffers (VkDevice const& device) {
			for (auto framebuffer : _framebuffers) {
				vkDestroyFramebuffer (device, framebuffer, nullptr);
			}
		}

		VkSwapchainKHR const& getSwapChain () {
			return _swapChain;
		}
		std::vector<VkImage> const& getImages () {
			return _images;
		}
		VkFormat const& getImageFormat () {
			return _imageFormat;
		}
		VkExtent2D getExtent () {
			return _extent;
		}

		std::vector<VkImageView> const& getImageViews () {
			return _imageViews;
		}
		std::vector<VkFramebuffer> const& getFramebuffers () {
			return _framebuffers;
		}
	};
};
