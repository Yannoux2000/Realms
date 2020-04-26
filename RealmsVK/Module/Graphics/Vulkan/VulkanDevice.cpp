#include "VulkanDevice.h"
#include "Vks.h"

using namespace rlms;

VulkanDevice::VulkanDevice (VkPhysicalDevice physicalDevice, VulkanQueues*& queues) : logicalDevice(VK_NULL_HANDLE) {
	assert (physicalDevice);
	this->physicalDevice = physicalDevice;
	this->queues = queues;

	// Store Properties features, limits and properties of the physical device for later use
	// Device properties also contain limits and sparse properties
	vkGetPhysicalDeviceProperties (physicalDevice, &properties);

	// Features should be checked by the examples before using them
	vkGetPhysicalDeviceFeatures (physicalDevice, &features);

	// Memory properties are used regularly for creating all kinds of buffers
	vkGetPhysicalDeviceMemoryProperties (physicalDevice, &memoryProperties);

	// Queue family properties, used for setting up requested queues upon device creation
	uint32_t queueFamilyCount;
	vkGetPhysicalDeviceQueueFamilyProperties (physicalDevice, &queueFamilyCount, nullptr);
	queueFamilyProperties.resize (queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties (physicalDevice, &queueFamilyCount, queueFamilyProperties.data ());

	// Get list of supported extensions
	uint32_t extCount = 0;
	vkEnumerateDeviceExtensionProperties (physicalDevice, nullptr, &extCount, nullptr);
	if (extCount > 0) {
		std::vector<VkExtensionProperties> extensions (extCount);
		if (vkEnumerateDeviceExtensionProperties (physicalDevice, nullptr, &extCount, &extensions.front ()) == VK_SUCCESS) {
			for (auto ext : extensions) {
				supportedExtensions.push_back (ext.extensionName);
			}
		}
	}
}
VulkanDevice::~VulkanDevice () {
	delete queues;
	if (logicalDevice) {
		vkDestroyDevice (logicalDevice, nullptr);
	}
}

void VulkanDevice::queryQueueFamilyIndex (VkSurfaceKHR surface) {
	assert(queues);
	for (uint32_t i = 0; i < static_cast<uint32_t>(queueFamilyProperties.size ()); i++) {
		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR (physicalDevice, i, surface, &presentSupport);
		VkQueueFlags qflags = queueFamilyProperties[i].queueFlags;

		if ((qflags & VK_QUEUE_GRAPHICS_BIT) && ((qflags & VK_QUEUE_COMPUTE_BIT) == 0) && ((qflags & VK_QUEUE_TRANSFER_BIT) == 0)) {
			queues->setGraphics (i);
		}

		if ((qflags & VK_QUEUE_COMPUTE_BIT) && ((qflags & VK_QUEUE_COMPUTE_BIT) == 0) && ((qflags & VK_QUEUE_TRANSFER_BIT) == 0)) {
			queues->setCompute (i);
		}

		if ((qflags & VK_QUEUE_TRANSFER_BIT) && ((qflags & VK_QUEUE_COMPUTE_BIT) == 0) && ((qflags & VK_QUEUE_TRANSFER_BIT) == 0)) {
			queues->setTransfer (i);
		}

		if (presentSupport) {
			queues->setPresent (i);
		}

		if (queues->completed ()) {
			return;
		}
	}

	// For other queue types or if no separate compute queue is present, return the first one to support the requested flags
	for (uint32_t i = 0; i < static_cast<uint32_t>(queueFamilyProperties.size ()); i++) {
		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR (physicalDevice, i, surface, &presentSupport);
		VkQueueFlags qflags = queueFamilyProperties[i].queueFlags;

		if (presentSupport) {
			queues->setPresent (i);
		}

		if (qflags & VK_QUEUE_GRAPHICS_BIT) {
			queues->setGraphics (i);
			continue;
		}
		if (qflags & VK_QUEUE_COMPUTE_BIT) {
			queues->setCompute (i);
			continue;
		}
		if (qflags & VK_QUEUE_TRANSFER_BIT) {
			queues->setTransfer (i);
			continue;
		}
	}

	if (queues->completed ()) {
		return;
	}
	throw std::runtime_error ("Could not find a matching queue family index");
}

VkResult VulkanDevice::createLogicalDevice (VkPhysicalDeviceFeatures enabledFeatures, std::vector<const char*> enabledExtensions, void* pNextChain) {
	// Desired queues need to be requested upon logical device creation
	// Due to differing queue family configurations of Vulkan implementations this can be a bit tricky, especially if the application
	// requests different queue types

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos{};

	// Get queue family indices for the requested queue family types
	// Note that the indices may overlap depending on the implementation

	const float defaultQueuePriority (0.0f);
	std::vector<uint32_t> qfIdx = queues->getUnique ();

	for (auto qfi : qfIdx) {
		VkDeviceQueueCreateInfo queueInfo{};
		queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueInfo.queueFamilyIndex = qfi;
		queueInfo.queueCount = 1;
		queueInfo.pQueuePriorities = &defaultQueuePriority;
		queueCreateInfos.push_back (queueInfo);
	}

	// Create the logical device representation
	std::vector<const char*> deviceExtensions (enabledExtensions);

	// If the device will be used for presenting to a display via a swapchain we need to request the swapchain extension
	deviceExtensions.push_back (VK_KHR_SWAPCHAIN_EXTENSION_NAME);

	VkDeviceCreateInfo deviceCreateInfo = {};
	deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size ());;
	deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data ();
	deviceCreateInfo.pEnabledFeatures = &enabledFeatures;

	// If a pNext(Chain) has been passed, we need to add it to the device creation info
	VkPhysicalDeviceFeatures2 physicalDeviceFeatures2{};
	if (pNextChain) {
		physicalDeviceFeatures2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
		physicalDeviceFeatures2.features = enabledFeatures;
		physicalDeviceFeatures2.pNext = pNextChain;
		deviceCreateInfo.pEnabledFeatures = nullptr;
		deviceCreateInfo.pNext = &physicalDeviceFeatures2;
	}

	// Enable the debug marker extension if it is present (likely meaning a debugging tool is present)
	if (extensionSupported (VK_EXT_DEBUG_MARKER_EXTENSION_NAME)) {
		deviceExtensions.push_back (VK_EXT_DEBUG_MARKER_EXTENSION_NAME);
		enableDebugMarkers = true;
	}

	if (deviceExtensions.size () > 0) {
		deviceCreateInfo.enabledExtensionCount = (uint32_t)deviceExtensions.size ();
		deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data ();
	}

	VkResult result = vkCreateDevice (physicalDevice, &deviceCreateInfo, nullptr, &logicalDevice);

	if (result == VK_SUCCESS) {
		//// Create a default command pool for graphics command buffers
		// commandPool = createCommandPool (queueFamilyIndices.graphics);
		queues->generateQueues (logicalDevice);
	}

	this->enabledFeatures = enabledFeatures;
	return result;
}

VkResult VulkanDevice::createBuffer (VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags memoryPropertyFlags, VkDeviceSize size, VkBuffer* buffer, VkDeviceMemory* memory, void* data) {
	// Create the buffer handle
	VkBufferCreateInfo bufferCreateInfo = vks::BUFFER_CREATE_INFO (usageFlags, size);
	bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	VulkanResultCheck (vkCreateBuffer (logicalDevice, &bufferCreateInfo, nullptr, buffer));

	// Create the memory backing up the buffer handle
	VkMemoryRequirements memReqs;
	VkMemoryAllocateInfo memAlloc = vks::MEMORY_ALLOCATE_INFO ();

	vkGetBufferMemoryRequirements (logicalDevice, *buffer, &memReqs);
	memAlloc.allocationSize = memReqs.size;
	// Find a memory type index that fits the properties of the buffer
	memAlloc.memoryTypeIndex = getMemoryType (memReqs.memoryTypeBits, memoryPropertyFlags);
	VulkanResultCheck (vkAllocateMemory (logicalDevice, &memAlloc, nullptr, memory));

	// If a pointer to the buffer data has been passed, map the buffer and copy over the data
	if (data != nullptr) {
		void* mapped;
		VulkanResultCheck (vkMapMemory (logicalDevice, *memory, 0, size, 0, &mapped));
		memcpy (mapped, data, size);
		// If host coherency hasn't been requested, do a manual flush to make writes visible
		if ((memoryPropertyFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) == 0) {

			VkMappedMemoryRange mappedRange = vks::MAPPED_MEMORY_RANGE (size, memory);
			vkFlushMappedMemoryRanges (logicalDevice, 1, &mappedRange);
		}
		vkUnmapMemory (logicalDevice, *memory);
	}

	// Attach the memory to the buffer object
	VulkanResultCheck (vkBindBufferMemory (logicalDevice, *buffer, *memory, 0));

	return VK_SUCCESS;
}
VkResult VulkanDevice::createBuffer (VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags memoryPropertyFlags, VulkanBuffer* buffer, VkDeviceSize size, void* data) {
	buffer->setDevice (logicalDevice);

	// Create the buffer handle
	VkBufferCreateInfo bufferCreateInfo = vks::BUFFER_CREATE_INFO (usageFlags, size);
	VulkanResultCheck (vkCreateBuffer (logicalDevice, &bufferCreateInfo, nullptr, *buffer));

	// Create the memory backing up the buffer handle
	VkMemoryRequirements memReqs;
	VkMemoryAllocateInfo memAlloc = vks::MEMORY_ALLOCATE_INFO ();

	vkGetBufferMemoryRequirements (logicalDevice, *buffer, &memReqs);
	memAlloc.allocationSize = memReqs.size;
	// Find a memory type index that fits the properties of the buffer
	memAlloc.memoryTypeIndex = getMemoryType (memReqs.memoryTypeBits, memoryPropertyFlags);
	VulkanResultCheck (vkAllocateMemory (logicalDevice, &memAlloc, nullptr, *buffer));

	buffer->alignment = memReqs.alignment;
	buffer->size = size;
	buffer->usageFlags = usageFlags;
	buffer->memoryPropertyFlags = memoryPropertyFlags;

	// If a pointer to the buffer data has been passed, map the buffer and copy over the data
	if (data != nullptr) {
		VulkanResultCheck (buffer->map ());
		memcpy (buffer->mapped, data, size);
		if ((memoryPropertyFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) == 0)
			buffer->flush ();

		buffer->unmap ();
	}

	// Initialize a default descriptor that covers the whole buffer size
	buffer->setupDescriptor ();

	// Attach the memory to the buffer object
	return buffer->bind ();
}

void VulkanDevice::copyBuffer (VulkanBuffer* src, VulkanBuffer* dst, VkCommandPool pool, VkQueue queue, VkBufferCopy* copyRegion) {
	assert (dst->size <= src->size);
	VkCommandBuffer copyCmd = createCommandBuffer (VK_COMMAND_BUFFER_LEVEL_PRIMARY, pool, true);
	VkBufferCopy bufferCopy{};
	if (copyRegion == nullptr) {
		bufferCopy.size = src->size;
	} else {
		bufferCopy = *copyRegion;
	}

	vkCmdCopyBuffer (copyCmd, *src, *dst, 1, &bufferCopy);

	flushCommandBuffer (copyCmd, pool, queue, true);
}

VkCommandPool VulkanDevice::createCommandPool (uint32_t queueFamilyIndex, VkCommandPoolCreateFlags createFlags) {
	VkCommandPoolCreateInfo cmdPoolInfo = vks::COMMAND_POOL_CREATE_INFO (queueFamilyIndex, createFlags);
	VkCommandPool cmdPool;
	VulkanResultCheck (vkCreateCommandPool (logicalDevice, &cmdPoolInfo, nullptr, &cmdPool));
	return cmdPool;
}

VkCommandBuffer VulkanDevice::createCommandBuffer (VkCommandBufferLevel level, VkCommandPool pool, bool begin) {

	VkCommandBufferAllocateInfo cmdBufAllocateInfo = vks::COMMAND_BUFFER_ALLOCATE_INFO (level, pool);
	
	VkCommandBuffer cmdBuffer;
	VulkanResultCheck (vkAllocateCommandBuffers (logicalDevice, &cmdBufAllocateInfo, &cmdBuffer));
	// If requested, also start recording for the new command buffer
	if (begin) {
		VkCommandBufferBeginInfo cmdBufInfo{};
		cmdBufInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		VulkanResultCheck (vkBeginCommandBuffer (cmdBuffer, &cmdBufInfo));
	}
	return cmdBuffer;
}
void VulkanDevice::flushCommandBuffer (VkCommandBuffer commandBuffer, VkCommandPool pool, VkQueue queue, bool free) {
	if (commandBuffer == VK_NULL_HANDLE) {
		return;
	}

	VulkanResultCheck (vkEndCommandBuffer (commandBuffer));

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	// Create fence to ensure that the command buffer has finished executing
	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = 0;

	VkFence fence;
	VulkanResultCheck (vkCreateFence (logicalDevice, &fenceInfo, nullptr, &fence));
	// Submit to the queue
	VulkanResultCheck (vkQueueSubmit (queue, 1, &submitInfo, fence));
	// Wait for the fence to signal that command buffer has finished executing
	VulkanResultCheck (vkWaitForFences (logicalDevice, 1, &fence, VK_TRUE, 100000000000));
	vkDestroyFence (logicalDevice, fence, nullptr);
	if (free) {
		vkFreeCommandBuffers (logicalDevice, pool, 1, &commandBuffer);
	}
}

bool VulkanDevice::extensionSupported (std::string extension) {
	return (std::find (supportedExtensions.begin (), supportedExtensions.end (), extension) != supportedExtensions.end ());
}

VkFormat VulkanDevice::getSupportedDepthFormat () {
	// All depth formats may be optional, so we need to find a suitable depth format to use
	std::vector<VkFormat> depthFormats = { 
		VK_FORMAT_D32_SFLOAT_S8_UINT,
		VK_FORMAT_D32_SFLOAT,
		VK_FORMAT_D24_UNORM_S8_UINT,
		VK_FORMAT_D16_UNORM_S8_UINT,
		VK_FORMAT_D16_UNORM 
	};
	for (auto& format : depthFormats) {
		VkFormatProperties formatProperties;
		vkGetPhysicalDeviceFormatProperties (physicalDevice, format, &formatProperties);
		// Format must support depth stencil attachment for optimal tiling
		if (formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) {
			return format;
		}
	}
	throw std::runtime_error ("Could not find a matching depth format");
}
uint32_t VulkanDevice::getMemoryType (uint32_t typeBits, VkMemoryPropertyFlags properties, VkBool32* memTypeFound) {
	for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++) {
		if ((typeBits & 1) == 1) {
			if ((memoryProperties.memoryTypes[i].propertyFlags & properties) == properties) {
				if (memTypeFound) {
					*memTypeFound = true;
				}
				return i;
			}
		}
		typeBits >>= 1;
	}

	if (memTypeFound) {
		*memTypeFound = false;
		return 0;
	} else {
		throw std::runtime_error ("Could not find a matching memory type");
	}
}
