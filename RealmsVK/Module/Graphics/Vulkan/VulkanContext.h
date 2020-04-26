#pragma once
#include "../IContext.h"

#include "Vulkan.h"
#include "VulkanDevice.h"
#include "VulkanSwapChain.h"
#include "IVulkanRenderer.h"

namespace rlms {

	VkResult CreateDebugUtilsMessengerEXT (VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
	void DestroyDebugUtilsMessengerEXT (VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);

	class VulkanContext : public IContext {
	private:

		VkInstance instance;
		VkDebugUtilsMessengerEXT debugMessenger;
		// Physical device (GPU) that Vulkan will use
		VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

		/** @brief Encapsulated physical and logical vulkan device */
		VulkanDevice* vulkanDevice;
		/** @brief swapChains will be used for every renderer */
		VulkanSwapChain* swapChain;
		/** @brief Queues that are going to be used, this struct will hold all informations related to the queues from initialisation to runtime */
		VulkanQueues* queues;

		void createInstance ();
		bool checkValidationLayerSupport ();
		std::vector<const char*> getRequiredInstanceExtensions ();

		void PickPhysicalDevice ();
		int ratePhysicalDevice (VkPhysicalDevice device);
		bool checkDeviceExtensionSupport (VkPhysicalDevice device);

	public:
		VulkanContext () : IContext(), vulkanDevice(nullptr), swapChain(nullptr), instance (), debugMessenger (), physicalDevice () {}

		~VulkanContext () {}

		void start (GLFWwindow*& window) override {
			createInstance ();
			swapChain = new VulkanSwapChain (instance); //generates surface
			swapChain->InitSurface (window);

			PickPhysicalDevice ();
			queues = new VulkanQueues ();
			vulkanDevice = new VulkanDevice (physicalDevice, queues);

			static_cast<IVulkanRenderer*>(renderer)->connect (swapChain, vulkanDevice, queues);
			static_cast<IVulkanRenderer*>(renderer)->initDevice ();

			swapChain->initSwapChain (vulkanDevice, queues);

			renderer->start ();
		}

		void draw () override {
			renderer->draw ();
		}

		void stop () override {
			vkDeviceWaitIdle (*vulkanDevice);
			renderer->stop ();
			delete renderer;
			swapChain->destroySwapChain (*vulkanDevice);
			delete swapChain;
			delete vulkanDevice;

			if (enableValidationLayers) {
				DestroyDebugUtilsMessengerEXT (instance, debugMessenger, nullptr);
			}

			vkDestroyInstance (instance, nullptr);
		}

		VkInstance& getVKInstance () {
			return instance;
		}

		VkDevice& getVkDevice () {
			return *vulkanDevice;
		}

		VkPhysicalDevice const& getVkPhysicalDevice () {
			return physicalDevice;
		}

	};
}

