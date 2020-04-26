#pragma once
#include "Vulkan.h"
#include "VulkanDevice.h"

namespace rlms {

	struct SwapChainBuffer {
		VkImage image;
		VkImageView view;
	};

	class VulkanSwapChain {
	private:
		VkSwapchainKHR swapChain = VK_NULL_HANDLE;

		// References
		VkInstance instance;
		VkSurfaceKHR surface;

		/** @brief Handle to the current swap chain, required for recreation */
		std::vector<VkImage> images;

		///** @brief Queue family index of the detected graphics and presenting device queue */
		//uint32_t queueNodeIndex = UINT32_MAX;

		VkSurfaceFormatKHR chooseSwapSurfaceFormat (const std::vector<VkSurfaceFormatKHR>& availableFormats);
		VkPresentModeKHR chooseSwapPresentMode (const std::vector<VkPresentModeKHR>& availablePresentModes);
		VkExtent2D chooseSwapExtent (const VkSurfaceCapabilitiesKHR& capabilities);
		VkCompositeAlphaFlagBitsKHR chooseCompositeAlpha (const VkSurfaceCapabilitiesKHR& capabilities);
		
		struct SwapChainSupportDetails {
			VkSurfaceCapabilitiesKHR capabilities;
			std::vector<VkSurfaceFormatKHR> formats;
			std::vector<VkPresentModeKHR> presentModes;
		};
		SwapChainSupportDetails QuerySwapChainSupport (VkPhysicalDevice const& device, VkSurfaceKHR const& surface);
	public:
		std::vector<SwapChainBuffer> buffers;

		VkColorSpaceKHR colorSpace;
		VkFormat colorFormat;
		VkExtent2D extent;

		operator VkSwapchainKHR() { return swapChain; }
		operator VkSwapchainKHR*() { return &swapChain; }
		operator VkSurfaceKHR() { return surface; }

		/** @brief Creates the platform surface window used for presentation */
		VulkanSwapChain (VkInstance& r_instance);
		~VulkanSwapChain () { }

		void InitSurface (GLFWwindow*& window);

		bool PhysicalDeviceCompatible (VkPhysicalDevice& device);
		
		void initSwapChain (VulkanDevice*& device, VulkanQueues*& queues);

		void createFramebuffers (VkDevice& device, VkRenderPass& renderPass, std::vector<VkFramebuffer> framebuffers);
		
		void destroySwapChain (VkDevice device) {
			if (swapChain != VK_NULL_HANDLE) {
				for (uint32_t i = 0; i < buffers.size(); i++) {
					vkDestroyImageView (device, buffers[i].view, nullptr);
				}
			}
			if (surface != VK_NULL_HANDLE) {
				vkDestroySwapchainKHR (device, swapChain, nullptr);
				vkDestroySurfaceKHR (instance, surface, nullptr);
			}
			surface = VK_NULL_HANDLE;
			swapChain = VK_NULL_HANDLE;
		}
	};
};
