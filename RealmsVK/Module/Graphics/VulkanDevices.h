#pragma once
#include "VulkanEnvironment.h"
#include "VulkanRenderPipeline.h"
#include <set>

namespace rlms {
	struct QueueFamilyIndices {
		uint32_t graphicsFamily;
		uint32_t presentFamily;

		bool gfxF = false;
		bool pstF = false;

		bool isComplete () {
			return gfxF && pstF;
		}
	};

	class VulkanDevices {
	private:
		VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
		VkDevice device;

		VkQueue graphicsQueue;
		VkQueue presentQueue;

		VkSurfaceKHR* r_surface;
		VkInstance* r_instance;

		void pickPhysicalDevice ();
		int ratePhysicalDevice (VkPhysicalDevice device);
		bool checkDeviceExtensionSupport (VkPhysicalDevice device);
		QueueFamilyIndices findQueueFamilies (VkPhysicalDevice device);

		void createLogicalDevice ();

	public:
		void start (VkSurfaceKHR* t_surface, VkInstance* t_instance) {
			r_instance = t_instance;
			r_surface = t_surface;

			pickPhysicalDevice ();
			createLogicalDevice ();
		}

		void stop () {
			vkDestroyDevice (device, nullptr);
		}

		QueueFamilyIndices const& getQueueFamilies () {
			return findQueueFamilies (physicalDevice);
		}

		VkQueue const& getGfxQueue () {
			return graphicsQueue;
		}
		VkQueue const& getPrtQueue () {
			return presentQueue;
		}

		VkDevice const& getDevice () {
			return device;
		}
		VkPhysicalDevice const& getVkPhysicalDevice () {
			return physicalDevice;
		}

	};

};