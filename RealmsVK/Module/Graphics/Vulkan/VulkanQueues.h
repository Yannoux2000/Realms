#pragma once
#include "Vulkan.h"
#include <vector>

namespace rlms {
	class VulkanQueues {
	private:
		bool graphics_picked = false;
		bool compute_picked = false;
		bool transfer_picked = false;
		bool present_picked = false;

		uint32_t graphics_idx;
		uint32_t compute_idx;
		uint32_t transfer_idx;
		uint32_t present_idx;
	public:
		VkQueueFlags requested = VK_QUEUE_GRAPHICS_BIT;

		VkQueue graphics;
		VkQueue compute;
		VkQueue transfer;
		VkQueue present;

		void setGraphics (uint32_t q) {
			if (!graphics_picked && (requested & VK_QUEUE_GRAPHICS_BIT)) {
				graphics_idx = q;
			}
			graphics_picked = true;
		}
		uint32_t getGraphics () {
			if (graphics_picked) {
				return graphics_idx;
			}
			return UINT32_MAX;
		}
		void setCompute (uint32_t q) {
			if (!compute_picked && (requested & VK_QUEUE_COMPUTE_BIT)) {
				compute_idx = q;
			}
			compute_picked = true;
		}
		uint32_t getCompute () {
			if (compute_picked) {
				return compute_idx;
			}
			return getGraphics ();
		}
		void setTransfer (uint32_t q) {
			if (!transfer_picked && (requested & VK_QUEUE_TRANSFER_BIT)) {
				transfer_idx = q;
			}
			transfer_picked = true;
		}
		uint32_t getTransfer () {
			if (transfer_picked) {
				return transfer_idx;
			}
			return getGraphics ();
		}
		void setPresent (uint32_t q) {
			if (!present_picked) {
				present_idx = q;
			}
			present_picked = true;
		}
		uint32_t getPresent () {
			if (present_picked) {
				return present_idx;
			}
			return getGraphics ();
		}

		inline const bool completed () const {
			return graphics_picked &&
				compute_picked &&
				transfer_picked &&
				present_picked;
		}

		inline const std::vector<uint32_t> getUnique () {
			std::vector<uint32_t> ret ({ graphics_idx });

			if ((requested & VK_QUEUE_COMPUTE_BIT) && graphics_idx != compute_idx) {
				ret.push_back (compute_idx);
			}
			if ((requested & VK_QUEUE_TRANSFER_BIT) && graphics_idx != transfer_idx && compute_idx != transfer_idx) {
				ret.push_back (transfer_idx);
			}
			if (graphics_idx != present_idx && compute_idx != present_idx) {
				ret.push_back (present_idx);
			}

			return ret;
		}

		inline void generateQueues (VkDevice& device) {
			vkGetDeviceQueue (device, graphics_idx, 0, &graphics);

			if (requested & VK_QUEUE_COMPUTE_BIT) {
				vkGetDeviceQueue (device, compute_idx, 0, &compute);
			}
			if (requested & VK_QUEUE_TRANSFER_BIT) {
				vkGetDeviceQueue (device, transfer_idx, 0, &transfer);
			}

			vkGetDeviceQueue (device, present_idx, 0, &present);
		}

		inline void d() {

		}
	};
}
