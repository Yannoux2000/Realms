#pragma once
#include "Vulkan.h"
#include "VulkanDevice.h"
#include "VulkanSwapChain.h"

namespace rlms {

	class VulkanRenderPipeline {
	private:
		VulkanDevice* r_device = nullptr;
		VkSurfaceKHR* r_surface = nullptr;

		VulkanSwapChain _SwapChain;

		VkRenderPass renderPass;
		VkPipelineLayout pipelineLayout;
		VkPipeline graphicsPipeline;

		void createRenderPass ();
		void createGraphicsPipeline ();
		VkShaderModule createShaderModule (const std::vector<char>& code);

	public:
		static bool QueryExtentionsSupport (VkPhysicalDevice const& device, VkSurfaceKHR const& surface) {
			SwapChainSupportDetails swapChainSupport = VulkanSwapChain::QuerySwapChainSupport (device, surface);
			return swapChainSupport.formats.empty () || swapChainSupport.presentModes.empty ();
		}

		void start (VulkanDevice* t_device, VkSurfaceKHR* t_surface) {
			r_device = t_device;
			r_surface = t_surface;

			_SwapChain.createSwapChain (*r_device, *r_surface);
			_SwapChain.createImageViews (*r_device);

			createRenderPass ();
			createGraphicsPipeline ();

			_SwapChain.createFramebuffers (*r_device, renderPass);

		}
		void stop () {
			_SwapChain.destroyFramebuffers (r_device->getDevice ());

			vkDestroyPipeline (r_device->getDevice (), graphicsPipeline, nullptr);
			vkDestroyPipelineLayout (r_device->getDevice (), pipelineLayout, nullptr);
			vkDestroyRenderPass (r_device->getDevice (), renderPass, nullptr);

			_SwapChain.destroyImageViews (r_device->getDevice ());
			_SwapChain.destroySwapChain (r_device->getDevice ());
		}


		VkRenderPass const& getRenderPass () {
			return renderPass;
		}
		VkPipelineLayout const& getPipelineLayout () {
			return pipelineLayout;
		}
		VkPipeline const& getGraphicsPipeline () {
			return graphicsPipeline;
		}
		VulkanSwapChain& getSwapChain () {
			return _SwapChain;
		}
	};
};