#include "Vulkan.h"
#include "../../../Base/Exception.h"

using namespace rlms;

void VulkanResultCheck (VkResult res) {
	switch (res) {
		case VkResult::VK_SUCCESS:
			return;

		case VkResult::VK_NOT_READY:
			throw Exception ("VK_NOT_READY");

		case VkResult::VK_TIMEOUT:
			throw Exception ("VK_TIMEOUT");

		case VkResult::VK_EVENT_SET:
			throw Exception ("VK_EVENT_SET");

		case VkResult::VK_EVENT_RESET:
			throw Exception ("VK_EVENT_RESET");

		case VkResult::VK_INCOMPLETE:
			throw Exception ("VK_INCOMPLETE");

		case VkResult::VK_ERROR_OUT_OF_HOST_MEMORY:
			throw Exception ("VK_ERROR_OUT_OF_HOST_MEMORY");

		case VkResult::VK_ERROR_OUT_OF_DEVICE_MEMORY:
			throw Exception ("VK_ERROR_OUT_OF_DEVICE_MEMORY");

		case VkResult::VK_ERROR_INITIALIZATION_FAILED:
			throw Exception ("VK_ERROR_INITIALIZATION_FAILED");

		case VkResult::VK_ERROR_DEVICE_LOST:
			throw Exception ("VK_ERROR_DEVICE_LOST");

		case VkResult::VK_ERROR_MEMORY_MAP_FAILED:
			throw Exception ("VK_ERROR_MEMORY_MAP_FAILED");

		case VkResult::VK_ERROR_LAYER_NOT_PRESENT:
			throw Exception ("VK_ERROR_LAYER_NOT_PRESENT");

		case VkResult::VK_ERROR_EXTENSION_NOT_PRESENT:
			throw Exception ("VK_ERROR_EXTENSION_NOT_PRESENT");

		case VkResult::VK_ERROR_FEATURE_NOT_PRESENT:
			throw Exception ("VK_ERROR_FEATURE_NOT_PRESENT");

		case VkResult::VK_ERROR_INCOMPATIBLE_DRIVER:
			throw Exception ("VK_ERROR_INCOMPATIBLE_DRIVER");

		case VkResult::VK_ERROR_TOO_MANY_OBJECTS:
			throw Exception ("VK_ERROR_TOO_MANY_OBJECTS");

		case VkResult::VK_ERROR_FORMAT_NOT_SUPPORTED:
			throw Exception ("VK_ERROR_FORMAT_NOT_SUPPORTED");

		case VkResult::VK_ERROR_FRAGMENTED_POOL:
			throw Exception ("VK_ERROR_FRAGMENTED_POOL");

		case VkResult::VK_ERROR_OUT_OF_POOL_MEMORY:
			throw Exception ("VK_ERROR_OUT_OF_POOL_MEMORY");

		case VkResult::VK_ERROR_INVALID_EXTERNAL_HANDLE:
			throw Exception ("VK_ERROR_INVALID_EXTERNAL_HANDLE");

		case VkResult::VK_ERROR_SURFACE_LOST_KHR:
			throw Exception ("VK_ERROR_SURFACE_LOST_KHR");

		case VkResult::VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
			throw Exception ("VK_ERROR_NATIVE_WINDOW_IN_USE_KHR");

		case VkResult::VK_SUBOPTIMAL_KHR:
			throw Exception ("VK_SUBOPTIMAL_KHR");

		case VkResult::VK_ERROR_OUT_OF_DATE_KHR:
			throw Exception ("VK_ERROR_OUT_OF_DATE_KHR");

		case VkResult::VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:
			throw Exception ("VK_ERROR_INCOMPATIBLE_DISPLAY_KHR");

		case VkResult::VK_ERROR_VALIDATION_FAILED_EXT:
			throw Exception ("VK_ERROR_VALIDATION_FAILED_EXT");

		case VkResult::VK_ERROR_INVALID_SHADER_NV:
			throw Exception ("VK_ERROR_INVALID_SHADER_NV");

		case VkResult::VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT:
			throw Exception ("VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT");

		case VkResult::VK_ERROR_FRAGMENTATION_EXT:
			throw Exception ("VK_ERROR_FRAGMENTATION_EXT");

		case VkResult::VK_ERROR_NOT_PERMITTED_EXT:
			throw Exception ("VK_ERROR_NOT_PERMITTED_EXT");

		case VkResult::VK_ERROR_INVALID_DEVICE_ADDRESS_EXT:
			throw Exception ("VK_ERROR_INVALID_DEVICE_ADDRESS_EXT");

		case VkResult::VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT:
			throw Exception ("VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT");
	}
}
