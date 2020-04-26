#pragma once
#include "../../../Base/Exception.h"

namespace rlms {
	class VulkanException : public Exception {
	public:
		VulkanException (std::string t_type) noexcept : Exception (t_type) {};
		VulkanException (std::string t_type, std::string t_description) noexcept : Exception (t_type, t_description) {};
	};

	struct MissingValidationLayer : public VulkanException {
		MissingValidationLayer () noexcept : VulkanException ("Missing Validation Layer", "An error occured with your installation of the Vulkan SDK, Verify your project setup.") {}
	};

	struct FailedDebugMessengerInit : public VulkanException {
		FailedDebugMessengerInit () noexcept : VulkanException ("Debug Messenger Init Failed", "Couldn't Create a Debug Messenger somehow... Sorry i have no clue why...") {}
	};

	struct NoGPUAvailable : public VulkanException {
		NoGPUAvailable () noexcept : VulkanException ("No GPU Available", "Couldn't find compatible GPU, are you sure you can run this game ?") {}
	};
	
	struct FailedSwapChain : public VulkanException {
		FailedSwapChain () noexcept : VulkanException ("Failed Creating SwapChain", "The game failed to connect a mendatory Vulkan functionnality. Verify your drivers are updated and try again.") {}
	};

	struct FailedImageViews : public VulkanException {
		FailedImageViews () noexcept : VulkanException ("Failed Creating ImageViews", "The game failed to connect a mendatory Vulkan functionnality. Verify your drivers are updated and try again.") {}
	};

	struct FailedFramebuffer : public VulkanException {
		FailedFramebuffer () noexcept : VulkanException ("Failed Creating Framebuffer", "The game failed to connect a mendatory Vulkan functionnality. Verify your drivers are updated and try again.") {}
	};
}