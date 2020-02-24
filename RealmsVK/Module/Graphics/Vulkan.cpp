#include "Vulkan.h"

void VulkanResultCheck (VkResult res) {
	switch (res) {
		case VkResult::VK_SUCCESS:
			return;

		case VkResult::VK_NOT_READY:
			throw std::runtime_error ("VK_NOT_READY");

		case VkResult::VK_TIMEOUT:
			throw std::runtime_error ("VK_TIMEOUT");

		case VkResult::VK_EVENT_SET:
			throw std::runtime_error ("VK_EVENT_SET");

		case VkResult::VK_EVENT_RESET:
			throw std::runtime_error ("VK_EVENT_RESET");

		case VkResult::VK_INCOMPLETE:
			throw std::runtime_error ("VK_INCOMPLETE");

		case VkResult::VK_ERROR_OUT_OF_HOST_MEMORY:
			throw std::runtime_error ("VK_ERROR_OUT_OF_HOST_MEMORY");

		case VkResult::VK_ERROR_OUT_OF_DEVICE_MEMORY:
			throw std::runtime_error ("VK_ERROR_OUT_OF_DEVICE_MEMORY");

		case VkResult::VK_ERROR_INITIALIZATION_FAILED:
			throw std::runtime_error ("VK_ERROR_INITIALIZATION_FAILED");

		case VkResult::VK_ERROR_DEVICE_LOST:
			throw std::runtime_error ("VK_ERROR_DEVICE_LOST");

		case VkResult::VK_ERROR_MEMORY_MAP_FAILED:
			throw std::runtime_error ("VK_ERROR_MEMORY_MAP_FAILED");

		case VkResult::VK_ERROR_LAYER_NOT_PRESENT:
			throw std::runtime_error ("VK_ERROR_LAYER_NOT_PRESENT");

		case VkResult::VK_ERROR_EXTENSION_NOT_PRESENT:
			throw std::runtime_error ("VK_ERROR_EXTENSION_NOT_PRESENT");

		case VkResult::VK_ERROR_FEATURE_NOT_PRESENT:
			throw std::runtime_error ("VK_ERROR_FEATURE_NOT_PRESENT");

		case VkResult::VK_ERROR_INCOMPATIBLE_DRIVER:
			throw std::runtime_error ("VK_ERROR_INCOMPATIBLE_DRIVER");

		case VkResult::VK_ERROR_TOO_MANY_OBJECTS:
			throw std::runtime_error ("VK_ERROR_TOO_MANY_OBJECTS");

		case VkResult::VK_ERROR_FORMAT_NOT_SUPPORTED:
			throw std::runtime_error ("VK_ERROR_FORMAT_NOT_SUPPORTED");

		case VkResult::VK_ERROR_FRAGMENTED_POOL:
			throw std::runtime_error ("VK_ERROR_FRAGMENTED_POOL");

		case VkResult::VK_ERROR_OUT_OF_POOL_MEMORY:
			throw std::runtime_error ("VK_ERROR_OUT_OF_POOL_MEMORY");

		case VkResult::VK_ERROR_INVALID_EXTERNAL_HANDLE:
			throw std::runtime_error ("VK_ERROR_INVALID_EXTERNAL_HANDLE");

		case VkResult::VK_ERROR_SURFACE_LOST_KHR:
			throw std::runtime_error ("VK_ERROR_SURFACE_LOST_KHR");

		case VkResult::VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
			throw std::runtime_error ("VK_ERROR_NATIVE_WINDOW_IN_USE_KHR");

		case VkResult::VK_SUBOPTIMAL_KHR:
			throw std::runtime_error ("VK_SUBOPTIMAL_KHR");

		case VkResult::VK_ERROR_OUT_OF_DATE_KHR:
			throw std::runtime_error ("VK_ERROR_OUT_OF_DATE_KHR");

		case VkResult::VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:
			throw std::runtime_error ("VK_ERROR_INCOMPATIBLE_DISPLAY_KHR");

		case VkResult::VK_ERROR_VALIDATION_FAILED_EXT:
			throw std::runtime_error ("VK_ERROR_VALIDATION_FAILED_EXT");

		case VkResult::VK_ERROR_INVALID_SHADER_NV:
			throw std::runtime_error ("VK_ERROR_INVALID_SHADER_NV");

		case VkResult::VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT:
			throw std::runtime_error ("VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT");

		case VkResult::VK_ERROR_FRAGMENTATION_EXT:
			throw std::runtime_error ("VK_ERROR_FRAGMENTATION_EXT");

		case VkResult::VK_ERROR_NOT_PERMITTED_EXT:
			throw std::runtime_error ("VK_ERROR_NOT_PERMITTED_EXT");

		case VkResult::VK_ERROR_INVALID_DEVICE_ADDRESS_EXT:
			throw std::runtime_error ("VK_ERROR_INVALID_DEVICE_ADDRESS_EXT");

		case VkResult::VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT:
			throw std::runtime_error ("VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT");
	}
}
