#pragma once
#include "../../Base/Exception.h"
namespace rlms {
	class VulkanException : public Exception {
	public:
		VulkanException (std::string t_type) noexcept : Exception (t_type) {};
		VulkanException (std::string t_type, std::string t_description) noexcept : Exception (t_type, t_description) {};
	};

	struct FailedSwapChain : public VulkanException {
		FailedSwapChain () noexcept : VulkanException ("Failed Creating SwapChain", "The game failed to start a mendatory Vulkan functionnality. Verify your drivers are updated and try again.") {}
	};

	struct FailedImageViews : public VulkanException {
		FailedImageViews () noexcept : VulkanException ("Failed Creating ImageViews", "The game failed to start a mendatory Vulkan functionnality. Verify your drivers are updated and try again.") {}
	};

	struct FailedFramebuffer : public VulkanException {
		FailedFramebuffer () noexcept : VulkanException ("Failed Creating Framebuffer", "The game failed to start a mendatory Vulkan functionnality. Verify your drivers are updated and try again.") {}
	};
}