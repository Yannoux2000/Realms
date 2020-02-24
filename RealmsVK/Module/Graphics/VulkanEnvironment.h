#pragma once
#include "Vulkan.h"
#include "GraphicsManager.h"

namespace rlms {

	VkResult CreateDebugUtilsMessengerEXT (VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
	void DestroyDebugUtilsMessengerEXT (VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);

	class VulkanEnvironment {
	private:
		VkInstance instance;
		VkDebugUtilsMessengerEXT debugMessenger;
		VkSurfaceKHR surface;

		static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback (VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
			GraphicsManager::GetLogger ()->tag (LogTags::Error) << "validation layer: " << pCallbackData->pMessage << "\n";
			return VK_FALSE;
		}

		void populateDebugMessengerCreateInfo (VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
			createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
			createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
			createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
			createInfo.pfnUserCallback = debugCallback;
		}

		void createInstance ();

		std::vector<const char*> getRequiredExtensions ();
		bool checkValidationLayerSupport ();

		void createSurface (GLFWwindow* window);

		void setupDebugMessenger ();
	public:
		void start (GLFWwindow*& window) {
			createInstance ();
			setupDebugMessenger ();
			createSurface (window);
		}

		void stop () {
			if (enableValidationLayers) {
				DestroyDebugUtilsMessengerEXT (instance, debugMessenger, nullptr);
			}

			vkDestroySurfaceKHR (instance, surface, nullptr);
			vkDestroyInstance (instance, nullptr);
		}
		VkInstance& getVKInstance () {
			return instance;
		}
		VkSurfaceKHR& getVkSurfaceKHR () {
			return surface;
		}
	};
};
