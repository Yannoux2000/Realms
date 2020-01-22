#pragma once

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

#include <iostream>
#include <stdexcept>
#include <vector>
#include <functional>
#include <cstdlib>

#include "_Preprocess.h"
#include "_MemLeakMonitor.h"

#include "Module/Graphics/VulkanErrors.h"

const int WIDTH = 800;
const int HEIGHT = 600;

const std::vector<const char*> validationLayers = {
	"VK_LAYER_KHRONOS_validation"
};

#ifdef RLMS_DEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

VkResult CreateDebugUtilsMessengerEXT (VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr (instance, "vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr) {
		return func (instance, pCreateInfo, pAllocator, pDebugMessenger);
	} else {
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

void DestroyDebugUtilsMessengerEXT (VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr (instance, "vkDestroyDebugUtilsMessengerEXT");
	if (func != nullptr) {
		func (instance, debugMessenger, pAllocator);
	}
}


struct QueueFamilyIndices {
	bool has_value = false;
	uint32_t graphicsFamily;

	void set (uint32_t const& t_graphicsFamily) {
		graphicsFamily = t_graphicsFamily;
		has_value = true;
	}

	bool isComplete () {
		return has_value;
	}
};

class HelloTriangleApplication {
public:
	void run () {
		initWindow ();
		initVulkan ();
		mainLoop ();
		cleanup ();
	}

private:
	GLFWwindow* window;

	VkInstance instance;
	VkDebugUtilsMessengerEXT debugMessenger;
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	VkDevice device;

	VkQueue graphicsQueue;

	void initWindow () {
		glfwInit ();

		glfwWindowHint (GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint (GLFW_RESIZABLE, GLFW_FALSE);

		window = glfwCreateWindow (WIDTH, HEIGHT, "Realms", nullptr, nullptr);
	}

	void initVulkan () {
		createInstance ();
		setupDebugMessenger ();
		pickPhysicalDevice ();
		createLogicalDevice ();
	}

	void mainLoop () {
		while (!glfwWindowShouldClose (window)) {
			glfwPollEvents ();
		}
	}

	void cleanup () {
		vkDestroyDevice (device, nullptr);

		if (enableValidationLayers) {
			DestroyDebugUtilsMessengerEXT (instance, debugMessenger, nullptr);
		}

		vkDestroyInstance (instance, nullptr);

		glfwDestroyWindow (window);

		glfwTerminate ();
	}

	void createInstance () {
		if (enableValidationLayers && !checkValidationLayerSupport ()) {
			throw std::runtime_error ("validation layers requested, but not available!");
		}

		VkApplicationInfo appInfo;
		populateAppInfo (appInfo);

		VkInstanceCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;

		auto extensions = getRequiredExtensions ();
		createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size ());
		createInfo.ppEnabledExtensionNames = extensions.data ();

		VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
		if (enableValidationLayers) {
			createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size ());
			createInfo.ppEnabledLayerNames = validationLayers.data ();

			populateDebugMessengerCreateInfo (debugCreateInfo);
			createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)& debugCreateInfo;
		} else {
			createInfo.enabledLayerCount = 0;

			createInfo.pNext = nullptr;
		}

		VulkanResultCheck (vkCreateInstance (&createInfo, nullptr, &instance));
	}

	void populateAppInfo (VkApplicationInfo& appInfo) {
		appInfo = {};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "Realms";
		appInfo.applicationVersion = VK_MAKE_VERSION (1, 0, 0);
		appInfo.pEngineName = "Realm_engine";
		appInfo.engineVersion = VK_MAKE_VERSION (1, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_0;
	}

	void populateDebugMessengerCreateInfo (VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
		createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		createInfo.pfnUserCallback = debugCallback;
	}

	void setupDebugMessenger () {
		if (!enableValidationLayers) return;

		VkDebugUtilsMessengerCreateInfoEXT createInfo;
		populateDebugMessengerCreateInfo (createInfo);

		VulkanResultCheck (CreateDebugUtilsMessengerEXT (instance, &createInfo, nullptr, &debugMessenger));
	}

	void pickPhysicalDevice () {
		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices (instance, &deviceCount, nullptr);

		if (deviceCount == 0) {
			throw std::runtime_error ("failed to find GPUs with Vulkan support!");
		}

		std::vector<VkPhysicalDevice> devices (deviceCount);
		vkEnumeratePhysicalDevices (instance, &deviceCount, devices.data ());

		for (const auto& device : devices) {
			if (isDeviceSuitable (device)) {
				physicalDevice = device;
				break;
			}
		}

		if (physicalDevice == VK_NULL_HANDLE) {
			throw std::runtime_error ("failed to find a suitable GPU!");
		}
	}

	void createLogicalDevice () {
		QueueFamilyIndices indices = findQueueFamilies (physicalDevice);

		VkDeviceQueueCreateInfo queueCreateInfo = {};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = indices.graphicsFamily;
		queueCreateInfo.queueCount = 1;

		float queuePriority = 1.0f;
		queueCreateInfo.pQueuePriorities = &queuePriority;

		VkPhysicalDeviceFeatures deviceFeatures = {};

		VkDeviceCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

		createInfo.pQueueCreateInfos = &queueCreateInfo;
		createInfo.queueCreateInfoCount = 1;

		createInfo.pEnabledFeatures = &deviceFeatures;

		createInfo.enabledExtensionCount = 0;

		if (enableValidationLayers) {
			createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size ());
			createInfo.ppEnabledLayerNames = validationLayers.data ();
		} else {
			createInfo.enabledLayerCount = 0;
		}

		if (vkCreateDevice (physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS) {
			throw std::runtime_error ("failed to create logical device!");
		}

		vkGetDeviceQueue (device, indices.graphicsFamily, 0, &graphicsQueue);
	}

	bool isDeviceSuitable (VkPhysicalDevice device) {
		QueueFamilyIndices indices = findQueueFamilies (device);

		return indices.isComplete ();
	}

	QueueFamilyIndices findQueueFamilies (VkPhysicalDevice device) {
		QueueFamilyIndices indices;

		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties (device, &queueFamilyCount, nullptr);

		std::vector<VkQueueFamilyProperties> queueFamilies (queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties (device, &queueFamilyCount, queueFamilies.data ());

		int i = 0;
		for (const auto& queueFamily : queueFamilies) {
			if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
				indices.set (i);
			}

			if (indices.isComplete ()) {
				break;
			}

			i++;
		}

		return indices;
	}

	std::vector<const char*> getRequiredExtensions () {
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions;
		glfwExtensions = glfwGetRequiredInstanceExtensions (&glfwExtensionCount);

		std::vector<const char*> extensions (glfwExtensions, glfwExtensions + glfwExtensionCount);

		if (enableValidationLayers) {
			extensions.push_back (VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		}

		return extensions;
	}

	bool checkValidationLayerSupport () {
		uint32_t layerCount;
		vkEnumerateInstanceLayerProperties (&layerCount, nullptr);

		std::vector<VkLayerProperties> availableLayers (layerCount);
		vkEnumerateInstanceLayerProperties (&layerCount, availableLayers.data ());

		for (const char* layerName : validationLayers) {
			bool layerFound = false;

			for (const auto& layerProperties : availableLayers) {
				if (strcmp (layerName, layerProperties.layerName) == 0) {
					layerFound = true;
					break;
				}
			}

			if (!layerFound) {
				return false;
			}
		}

		return true;
	}

	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback (VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
		std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
		return VK_FALSE;
	}
};

int vlk_main () {
	MemCheck ();

	HelloTriangleApplication app;

	try {
		app.run ();
	} catch (const std::exception& e) {
		std::cerr << e.what () << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}