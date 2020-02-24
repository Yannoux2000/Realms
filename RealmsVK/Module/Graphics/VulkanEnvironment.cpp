#include "VulkanEnvironment.h"

void rlms::VulkanEnvironment::createInstance () {
	if (enableValidationLayers && !checkValidationLayerSupport ()) {
		throw std::runtime_error ("validation layers requested, but not available!");
	}

	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Realms";
	appInfo.applicationVersion = VK_MAKE_VERSION (1, 0, 0);
	appInfo.pEngineName = "Realms Engine";
	appInfo.engineVersion = VK_MAKE_VERSION (1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;

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

std::vector<const char*> rlms::VulkanEnvironment::getRequiredExtensions () {
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions (&glfwExtensionCount);

	std::vector<const char*> extensions (glfwExtensions, glfwExtensions + glfwExtensionCount);

	if (enableValidationLayers) {
		extensions.push_back (VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	return extensions;
}

bool rlms::VulkanEnvironment::checkValidationLayerSupport () {
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
		//VK_LAYER_KHRONOS_validation

		if (!layerFound) {
			return false;
		}
	}

	return true;
}

void rlms::VulkanEnvironment::createSurface (GLFWwindow* window) {
	VulkanResultCheck (glfwCreateWindowSurface (instance, window, nullptr, &surface));
}

void rlms::VulkanEnvironment::setupDebugMessenger () {
	if (!enableValidationLayers) return;

	VkDebugUtilsMessengerCreateInfoEXT createInfo;
	populateDebugMessengerCreateInfo (createInfo);

	if (CreateDebugUtilsMessengerEXT (instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
		throw std::runtime_error ("failed to set up debug messenger!");
	}
}

VkResult rlms::CreateDebugUtilsMessengerEXT (VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr (instance, "vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr) {
		return func (instance, pCreateInfo, pAllocator, pDebugMessenger);
	} else {
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

void rlms::DestroyDebugUtilsMessengerEXT (VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr (instance, "vkDestroyDebugUtilsMessengerEXT");
	if (func != nullptr) {
		func (instance, debugMessenger, pAllocator);
	}
}
