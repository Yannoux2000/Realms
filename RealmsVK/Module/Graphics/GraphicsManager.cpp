#include "GraphicsManager.h"

#include "VulkanErrors.h"

#include "../../Base/Logging/ILogged.h"
#include "../../Base/Allocators/Allocator.h"

#include "../../Utility/FileIO/VoxFileParser.h"

#include "GameRenderer.h"
#include "MeshRegister.h"
#include "ShaderPrototypeLights.h"
#include "StaticMesh.h"
#include "AspectRatio.h"

#include <set>

using namespace rlms;

const int MAX_FRAMES_IN_FLIGHT = 2;

const std::vector<const char*> validationLayers = {
	"VK_LAYER_KHRONOS_validation"
};

const std::vector<const char*> deviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
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
	uint32_t graphicsFamily;
	uint32_t presentFamily;

	bool gfxF = false;
	bool pstF = false;

	bool isComplete () {
		return gfxF && pstF;
	}
};

struct SwapChainSupportDetails {
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};

class rlms::GraphicsManagerImpl : public ILogged {
private:
	friend class GraphicsManager;

	VkInstance instance;
	VkDebugUtilsMessengerEXT debugMessenger;
	VkSurfaceKHR surface;

	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	VkDevice device;

	VkQueue graphicsQueue;
	VkQueue presentQueue;

	VkSwapchainKHR swapChain;
	std::vector<VkImage> swapChainImages;
	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;

	std::string getLogName () override {
		return "GraphicsManager";
	};

	void start (Allocator* const& alloc, size_t mesh_pool_size, GLFWwindow*& window, std::shared_ptr<Logger> funnel = nullptr);
	void stop ();

	MeshRegister* getRegister ();
	void Register (IMODEL_TYPE_ID const& type_id, std::string&& filename, std::string&& meshAlias);
	IMesh* getMesh (IMODEL_TYPE_ID const& type_id);

	void load ();
	void draw ();
	void unload ();

	void populateAppInfo (VkApplicationInfo& appInfo) {
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "Realms";
		appInfo.applicationVersion = VK_MAKE_VERSION (1, 0, 0);
		appInfo.pEngineName = "Realms Engine";
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

	void createInstance () {
		if (enableValidationLayers && !checkValidationLayerSupport ()) {
			throw std::runtime_error ("validation layers requested, but not available!");
		}

		VkApplicationInfo appInfo = {};
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

	void setupDebugMessenger () {
		if (!enableValidationLayers) return;

		VkDebugUtilsMessengerCreateInfoEXT createInfo;
		populateDebugMessengerCreateInfo (createInfo);

		if (CreateDebugUtilsMessengerEXT (instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
			throw std::runtime_error ("failed to set up debug messenger!");
		}
	}
	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback (VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
		GraphicsManager::GetLogger ()->tag (LogTags::Error) << "validation layer: " << pCallbackData->pMessage << "\n";
		return VK_FALSE;
	}

	void createSurface (GLFWwindow* window) {
		VulkanResultCheck (glfwCreateWindowSurface (instance, window, nullptr, &surface));
	}

	void pickPhysicalDevice () {
		logger->tag (LogTags::Debug) << "Searching for a suitable GPU with Vulkan support...\n";

		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices (instance, &deviceCount, nullptr);

		if (deviceCount == 0) {
			logger->tag (LogTags::Error) << "Couldn't find any GPUs with Vulkan support!\n";
			throw std::runtime_error ("failed to find GPUs with Vulkan support!");
		}

		std::vector<VkPhysicalDevice> devices (deviceCount);
		vkEnumeratePhysicalDevices (instance, &deviceCount, devices.data ());

		int current_score = 0;
		int new_score;

		for (const auto& device : devices) {
			new_score = ratePhysicalDevice (device);
			if ( new_score > current_score) {
				physicalDevice = device;
				current_score = new_score;
				break;
			}
		}

		if (physicalDevice == VK_NULL_HANDLE) {
			logger->tag (LogTags::Error) << "Failed to find a suitable GPU!\n";
			throw std::runtime_error ("failed to find a suitable GPU!");
		} else {
			VkPhysicalDeviceProperties deviceProperties;
			vkGetPhysicalDeviceProperties (physicalDevice, &deviceProperties);

			logger->tag (LogTags::Info) << deviceProperties.deviceName << " Found.\n";

		}
	}
	int ratePhysicalDevice (VkPhysicalDevice device) {

		QueueFamilyIndices indices = findQueueFamilies (device);

		VkPhysicalDeviceProperties deviceProperties;
		VkPhysicalDeviceFeatures deviceFeatures;
		vkGetPhysicalDeviceProperties (device, &deviceProperties);
		vkGetPhysicalDeviceFeatures (device, &deviceFeatures);

		int score = 0;

		// Les carte graphiques dédiées ont un énorme avantage en terme de performances
		if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
			score += 1000;
		}

		// La taille maximale des textures affecte leur qualité
		score += deviceProperties.limits.maxImageDimension2D;

		// L'application ne peut fonctionner sans les geometry shaders
		if (!deviceFeatures.geometryShader) {
			return 0;
		}

		// L'application ne peut fonctionner sans les extentions désignées
		bool extensionsSupported = checkDeviceExtensionSupport (device);
		bool swapChainAdequate = false;
		
		if (!extensionsSupported) {
			SwapChainSupportDetails swapChainSupport = querySwapChainSupport (device);
			swapChainAdequate = swapChainSupport.formats.empty () || swapChainSupport.presentModes.empty ();
		}

		if (!indices.isComplete () || !extensionsSupported || swapChainAdequate) {
			return 0;
		}

		return score;
	}
	bool checkDeviceExtensionSupport (VkPhysicalDevice device) {
		uint32_t extensionCount;
		vkEnumerateDeviceExtensionProperties (device, nullptr, &extensionCount, nullptr);

		std::vector<VkExtensionProperties> availableExtensions (extensionCount);
		vkEnumerateDeviceExtensionProperties (device, nullptr, &extensionCount, availableExtensions.data ());

		std::set<std::string> requiredExtensions (deviceExtensions.begin (), deviceExtensions.end ());

		for (const auto& extension : availableExtensions) {
			requiredExtensions.erase (extension.extensionName);
		}

		return requiredExtensions.empty ();
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
				indices.graphicsFamily = i;
				indices.gfxF = true;
			}

			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR (device, i, surface, &presentSupport);

			if (presentSupport) {
				indices.presentFamily = i;
				indices.pstF = true;
			}

			if (indices.isComplete ()) {
				break;
			}

			i++;
		}

		return indices;
	}

	void createLogicalDevice () {
		QueueFamilyIndices indices = findQueueFamilies (physicalDevice);

		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
		std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily, indices.presentFamily };

		float queuePriority = 1.0f;
		for (uint32_t queueFamily : uniqueQueueFamilies) {
			VkDeviceQueueCreateInfo queueCreateInfo = {};
			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.queueFamilyIndex = queueFamily;
			queueCreateInfo.queueCount = 1;
			queueCreateInfo.pQueuePriorities = &queuePriority;
			queueCreateInfos.push_back (queueCreateInfo);
		}

		VkPhysicalDeviceFeatures deviceFeatures = {};

		VkDeviceCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

		createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size ());
		createInfo.pQueueCreateInfos = queueCreateInfos.data ();

		createInfo.pEnabledFeatures = &deviceFeatures;

		createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size ());
		createInfo.ppEnabledExtensionNames = deviceExtensions.data ();

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
		vkGetDeviceQueue (device, indices.presentFamily, 0, &presentQueue);
	}

	void createSwapChain () {
		SwapChainSupportDetails swapChainSupport = querySwapChainSupport (physicalDevice);

		VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat (swapChainSupport.formats);
		VkPresentModeKHR presentMode = chooseSwapPresentMode (swapChainSupport.presentModes);
		VkExtent2D extent = chooseSwapExtent (swapChainSupport.capabilities);

		uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
		if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
			imageCount = swapChainSupport.capabilities.maxImageCount;
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

		QueueFamilyIndices indices = findQueueFamilies (physicalDevice);
		uint32_t queueFamilyIndices[] = { indices.graphicsFamily, indices.presentFamily };

		if (indices.graphicsFamily != indices.presentFamily) {
			createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			createInfo.queueFamilyIndexCount = 2;
			createInfo.pQueueFamilyIndices = queueFamilyIndices;
		} else {
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		}

		createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.presentMode = presentMode;
		createInfo.clipped = VK_TRUE;

		createInfo.oldSwapchain = VK_NULL_HANDLE;

		if (vkCreateSwapchainKHR (device, &createInfo, nullptr, &swapChain) != VK_SUCCESS) {
			throw std::runtime_error ("failed to create swap chain!");
		}

		vkGetSwapchainImagesKHR (device, swapChain, &imageCount, nullptr);
		swapChainImages.resize (imageCount);
		vkGetSwapchainImagesKHR (device, swapChain, &imageCount, swapChainImages.data ());

		swapChainImageFormat = surfaceFormat.format;
		swapChainExtent = extent;
	}
	SwapChainSupportDetails querySwapChainSupport (VkPhysicalDevice device) {
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
	VkSurfaceFormatKHR chooseSwapSurfaceFormat (const std::vector<VkSurfaceFormatKHR>& availableFormats) {

		for (const auto& availableFormat : availableFormats) {
			if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
				return availableFormat;
			}
		}

		return availableFormats[0];
	}
	VkPresentModeKHR chooseSwapPresentMode (const std::vector<VkPresentModeKHR>& availablePresentModes) {
		for (const auto& availablePresentMode : availablePresentModes) {
			if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
				return availablePresentMode;
			}
		}

		return VK_PRESENT_MODE_FIFO_KHR;
	}
	VkExtent2D chooseSwapExtent (const VkSurfaceCapabilitiesKHR& capabilities) {
		if (capabilities.currentExtent.width != UINT32_MAX) {
			return capabilities.currentExtent;
		} else {
			VkExtent2D actualExtent = { AspectRatio::Width() , AspectRatio::Height() };

			actualExtent.width = std::max (capabilities.minImageExtent.width, std::min (capabilities.maxImageExtent.width, actualExtent.width));
			actualExtent.height = std::max (capabilities.minImageExtent.height, std::min (capabilities.maxImageExtent.height, actualExtent.height));

			return actualExtent;
		}
	}

};

void rlms::GraphicsManagerImpl::start (Allocator* const& alloc, size_t mesh_pool_size, GLFWwindow*& window, std::shared_ptr<Logger> funnel) {
	startLogger (funnel);
	logger->tag (LogTags::None) << "Initializing !" << '\n';

	createInstance ();
	setupDebugMessenger ();
	createSurface (window);

	pickPhysicalDevice ();
	createLogicalDevice ();

	createSwapChain ();

	logger->tag (LogTags::None) << "Initialized correctly !" << '\n';
}

void rlms::GraphicsManagerImpl::stop () {
	logger->tag (LogTags::None) << "Terminating" << '\n';

	vkDestroySwapchainKHR (device, swapChain, nullptr);

	vkDestroyDevice (device, nullptr);

	if (enableValidationLayers) {
		DestroyDebugUtilsMessengerEXT (instance, debugMessenger, nullptr);
	}

	vkDestroySurfaceKHR (instance, surface, nullptr);
	vkDestroyInstance (instance, nullptr);

	logger->tag (LogTags::None) << "Terminated correctly !" << '\n';
}

void rlms::GraphicsManagerImpl::load () {

}

void rlms::GraphicsManagerImpl::draw () {

}

void rlms::GraphicsManagerImpl::unload () {
}

std::unique_ptr<GraphicsManagerImpl> rlms::GraphicsManager::instance;

std::shared_ptr<LoggerHandler> rlms::GraphicsManager::GetLogger () {
	return instance->getLogger ();
}

void rlms::GraphicsManager::Initialize (Allocator* const& alloc, size_t mesh_pool_size, GLFWwindow*& window, std::shared_ptr<Logger> funnel) {
	instance = std::make_unique<GraphicsManagerImpl> ();
	instance->start (alloc, mesh_pool_size, window, funnel);
}

void rlms::GraphicsManager::Terminate () {
	instance->stop ();
	instance.reset ();
}

void rlms::GraphicsManager::Load () {
	instance->load ();
}

void rlms::GraphicsManager::Draw () {
	instance->draw ();
}

void rlms::GraphicsManager::Unload () {
	instance->unload ();
}
