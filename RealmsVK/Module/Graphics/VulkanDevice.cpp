#include <set>

#include "VulkanRenderPipeline.h"
#include "VulkanDevice.h"
#include "GraphicsManager.h"

using namespace rlms;

void VulkanDevice::pickPhysicalDevice () {
	GraphicsManager::GetLogger ()->tag (LogTags::Debug) << "Searching for a suitable GPU with Vulkan support...\n";

	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices (*r_instance, &deviceCount, nullptr);

	if (deviceCount == 0) {
		GraphicsManager::GetLogger ()->tag (LogTags::Error) << "Couldn't find any GPUs with Vulkan support!\n";
		throw std::runtime_error ("failed to find GPUs with Vulkan support!");
	}

	std::vector<VkPhysicalDevice> devices (deviceCount);
	vkEnumeratePhysicalDevices (*r_instance, &deviceCount, devices.data ());

	int current_score = 0;
	int new_score;

	for (const auto& device : devices) {
		new_score = ratePhysicalDevice (device);
		if (new_score > current_score) {
			physicalDevice = device;
			current_score = new_score;
			break;
		}
	}

	if (physicalDevice == VK_NULL_HANDLE) {
		GraphicsManager::GetLogger ()->tag (LogTags::Error) << "Failed to find a suitable GPU!\n";
		throw std::runtime_error ("failed to find a suitable GPU!");
	} else {
		VkPhysicalDeviceProperties deviceProperties;
		vkGetPhysicalDeviceProperties (physicalDevice, &deviceProperties);

		GraphicsManager::GetLogger ()->tag (LogTags::Info) << deviceProperties.deviceName << " Found.\n";

	}
}

int VulkanDevice::ratePhysicalDevice (VkPhysicalDevice device) {

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
	bool RenderPipelineAdequate = false;

	if (!extensionsSupported) {
		RenderPipelineAdequate = VulkanRenderPipeline::QueryExtentionsSupport (device, *r_surface);
	}

	if (!indices.isComplete () || !extensionsSupported || RenderPipelineAdequate) {
		return 0;
	}

	return score;
}

bool VulkanDevice::checkDeviceExtensionSupport (VkPhysicalDevice device) {
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

QueueFamilyIndices VulkanDevice::findQueueFamilies (VkPhysicalDevice device) {
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
		vkGetPhysicalDeviceSurfaceSupportKHR (device, i, *r_surface, &presentSupport);

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

void VulkanDevice::createLogicalDevice () {
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
