#include "VulkanContext.h"
#include "VulkanException.h"
#include "VulkanDevice.h"
#include "Vks.h"

#include <set>
using namespace rlms;

void VulkanContext::createInstance () {
	//Validation Layers
	if (enableValidationLayers && !checkValidationLayerSupport()) {
		throw MissingValidationLayer ();
	}

	VkApplicationInfo appInfo{};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Realms";
	appInfo.applicationVersion = VK_MAKE_VERSION (1, 0, 0);
	appInfo.pEngineName = "Realms";
	appInfo.engineVersion = VK_MAKE_VERSION (1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	auto ext = getRequiredInstanceExtensions ();

	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;
	createInfo.enabledExtensionCount = static_cast<uint32_t>(ext.size ());
	createInfo.ppEnabledExtensionNames = ext.data ();

	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
	debugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;

	debugCreateInfo.messageSeverity =
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;

	debugCreateInfo.messageType =
		VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;

	debugCreateInfo.pfnUserCallback = vks::debugCallback;

	if (enableValidationLayers) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size ());
		createInfo.ppEnabledLayerNames = validationLayers.data ();

		VkValidationFeatureEnableEXT enables[] = { VK_VALIDATION_FEATURE_ENABLE_BEST_PRACTICES_EXT };
		VkValidationFeaturesEXT features = {};
		features.sType = VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT;
		features.enabledValidationFeatureCount = 1;
		features.pEnabledValidationFeatures = enables;
		debugCreateInfo.pNext = &features;

		createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)& debugCreateInfo;
	} else {
		createInfo.enabledLayerCount = 0;
		createInfo.pNext = nullptr;
	}

	vkCreateInstance (&createInfo, nullptr, &instance);

	if (enableValidationLayers){
		if (CreateDebugUtilsMessengerEXT (instance, &debugCreateInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
			throw FailedDebugMessengerInit ();
		}
	}
}
bool VulkanContext::checkValidationLayerSupport () {
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties (&layerCount, nullptr);

	std::vector<VkLayerProperties> availableLayers (layerCount);
	vkEnumerateInstanceLayerProperties (&layerCount, availableLayers.data ());

	GraphicsManager::GetLogger ()->print (LogTags::Debug, "Available Validation Layer : ");
	for (auto layer : availableLayers) {
		GraphicsManager::GetLogger ()->print (LogTags::Debug, std::string (layer.layerName));
	}

	std::ostringstream oss;
	GraphicsManager::GetLogger ()->print (LogTags::Debug, "Required Validation Layer : ");
	for (const char* layerName : validationLayers) {
		bool layerFound = false;
		oss.str ("");
		oss << layerName << " : ";

		for (const auto& layerProperties : availableLayers) {
			if (strcmp (layerName, layerProperties.layerName) == 0) {
				layerFound = true;
			}
		}

		//VK_LAYER_KHRONOS_validation
		if (!layerFound) {
			oss << " MISSING X";
			GraphicsManager::GetLogger ()->print (LogTags::Debug, oss.str ());
			return false;
		} else {
			oss << "  Found  !";
			GraphicsManager::GetLogger ()->print (LogTags::Debug, oss.str ());
		}
	}

	return true;
}
std::vector<const char*> VulkanContext::getRequiredInstanceExtensions () {
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions (&glfwExtensionCount);

	std::vector<const char*> extensions (glfwExtensions, glfwExtensions + glfwExtensionCount);

	if (enableValidationLayers) {
		extensions.push_back (VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	GraphicsManager::GetLogger ()->print (LogTags::Debug, "Required extensions : ");
	for (const char* ext_name : extensions) {
		GraphicsManager::GetLogger ()->print (LogTags::Debug, std::string (ext_name));
	}

	return extensions;
}

void VulkanContext::PickPhysicalDevice () {
	GraphicsManager::GetLogger ()->tag (LogTags::Debug) << "Searching for a suitable GPU with Vulkan support...\n";

	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices (instance, &deviceCount, nullptr);

	if (deviceCount == 0) {
		GraphicsManager::GetLogger ()->tag (LogTags::Error) << "Couldn't find any GPUs with Vulkan support!\n";
		throw NoGPUAvailable ();
	}

	std::vector<VkPhysicalDevice> devices (deviceCount);
	vkEnumeratePhysicalDevices (instance, &deviceCount, devices.data ());

	int current_score = 0;
	int new_score;

	std::ostringstream oss ("");
	oss << "Found " << deviceCount << " devices :";
	GraphicsManager::GetLogger ()->print (LogTags::Debug, oss.str());
	for (const auto& device : devices) {
		new_score = ratePhysicalDevice (device);

		if (new_score > current_score) {//pick best scoring gpu
			physicalDevice = device;
			current_score = new_score;
			break;
		}
	}

	if (physicalDevice == VK_NULL_HANDLE) {
		GraphicsManager::GetLogger ()->print (LogTags::Error, "Failed to find a suitable GPU!\n");
		throw NoGPUAvailable ();
	} else {
		VkPhysicalDeviceProperties deviceProperties;
		vkGetPhysicalDeviceProperties (physicalDevice, &deviceProperties);

		std::ostringstream oss("");
		oss << deviceProperties.deviceName << " Found.";
		GraphicsManager::GetLogger ()->print (LogTags::Info, oss.str());
	}
}
int VulkanContext::ratePhysicalDevice (VkPhysicalDevice device) {
	VkPhysicalDeviceProperties deviceProperties;
	VkPhysicalDeviceFeatures deviceFeatures;
	vkGetPhysicalDeviceProperties (device, &deviceProperties);
	vkGetPhysicalDeviceFeatures (device, &deviceFeatures);

	std::ostringstream oss ("");
	oss << "GPU : " << deviceProperties.deviceName << "(" << deviceProperties.vendorID << " : " << deviceProperties.deviceID << ")";
	GraphicsManager::GetLogger ()->print (LogTags::Debug, oss.str ());

	int score = 0;

	// Les carte graphiques dédiées ont un énorme avantage en terme de performances
	if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
		score += 1000;
	}

	// La taille maximale des textures affecte leur qualité
	score += deviceProperties.limits.maxImageDimension2D;

	//// L'application ne peut fonctionner sans les geometry shaders
	//if (!deviceFeatures.geometryShader) {
	//	return 0;
	//}

	// L'application ne peut fonctionner sans les extentions désignées
	bool extensionsSupported = checkDeviceExtensionSupport (device);
	bool RenderPipelineAdequate = false;

	if (extensionsSupported) {
		RenderPipelineAdequate = swapChain->PhysicalDeviceCompatible (device);
	}

	if (!extensionsSupported || !RenderPipelineAdequate) {
		return 0;
	}

	oss << "Device scored : " << score;
	GraphicsManager::GetLogger ()->print (LogTags::Debug, oss.str ());
	return score;
}

bool VulkanContext::checkDeviceExtensionSupport (VkPhysicalDevice device) {
	uint32_t extensionCount;
	vkEnumerateDeviceExtensionProperties (device, nullptr, &extensionCount, nullptr);

	std::vector<VkExtensionProperties> availableExtensions (extensionCount);
	vkEnumerateDeviceExtensionProperties (device, nullptr, &extensionCount, availableExtensions.data ());

	std::set<std::string> requiredExtensions (deviceExtensions.begin (), deviceExtensions.end ());

	std::ostringstream oss ("");
	oss << "GPU has Extensions : ";
	GraphicsManager::GetLogger ()->print (LogTags::Debug, oss.str ());

	for (const auto& extension : availableExtensions) {
		oss.str ("");
		oss << extension.extensionName;
		GraphicsManager::GetLogger ()->print (LogTags::Debug, oss.str ());

		requiredExtensions.erase (extension.extensionName);
	}

	return requiredExtensions.empty (); //if we removed all requiredExtensions, then we are good
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
