#include "GraphicsManager.h"

#include "../../Base/Logging/ILogged.h"
#include "../../Base/Allocators/Allocator.h"

#include "../../Utility/FileIO/VoxFileParser.h"

#include "GameRenderer.h"
#include "MeshRegister.h"
#include "ShaderPrototypeLights.h"
#include "StaticMesh.h"
#include "AspectRatio.h"

#include "VulkanEnvironment.h"
#include "VulkanDevices.h"

using namespace rlms;

class RenderPipeline {
public:
	VkSwapchainKHR swapChain;
	std::vector<VkImage> swapChainImages;
	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;

	std::vector<VkImageView> swapChainImageViews;

	std::vector<VkFramebuffer> swapChainFramebuffers;

	VkRenderPass renderPass;
	VkPipelineLayout pipelineLayout;
	VkPipeline graphicsPipeline;

	void start ();
	void stop ();
};

class GraphicalDevice {
private:
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	VkDevice device;

	VkQueue graphicsQueue;
	VkQueue presentQueue;

	VkSurfaceKHR* r_surface;
	VkInstance* r_instance;

	void pickPhysicalDevice () {
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
			SwapChainSupportDetails swapChainSupport = VulkanRenderPipeline::QueryExtentionsSupport (device, *r_surface);
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

public:
	void start (VkSurfaceKHR* t_surface, VkInstance* t_instance) {
		r_instance = t_instance;
		r_surface = t_surface;

		pickPhysicalDevice ();
		createLogicalDevice ();
	}

	QueueFamilyIndices const& getQueueFamilies () {
		return findQueueFamilies (physicalDevice);
	}

	VkQueue const& getGfxQueue () {
		return graphicsQueue;
	}
	VkQueue const& getPrtQueue () {
		return presentQueue;
	}

	VkDevice const& getDevice () {
		return device;
	}
	VkPhysicalDevice const& getVkPhysicalDevice () {
		return physicalDevice;
	}

	void stop () {
		vkDestroyDevice (device, nullptr);
	}
};

class DrawingScheduler {
	std::vector<VkSemaphore> imageAvailableSemaphores;
	std::vector<VkSemaphore> renderFinishedSemaphores;
	std::vector<VkFence> inFlightFences;
	std::vector<VkFence> imagesInFlight;
	size_t currentFrame = 0;
};

class rlms::GraphicsManagerImpl : public ILogged {
private:
	friend class GraphicsManager;

	VulkanEnvironment _vulkanEnv;
	GraphicalDevice _GDevice;

	RenderPipeline _renderpipeline;

	VkCommandPool commandPool;
	std::vector<VkCommandBuffer> commandBuffers;

	std::vector<VkSemaphore> imageAvailableSemaphores;
	std::vector<VkSemaphore> renderFinishedSemaphores;
	std::vector<VkFence> inFlightFences;
	std::vector<VkFence> imagesInFlight;
	size_t currentFrame = 0;

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

	void createSwapChain () {
		SwapChainSupportDetails swapChainSupport = VulkanRenderPipeline::QueryExtentionsSupport (_GDevice.getVkPhysicalDevice(), _vulkanEnv.getVkSurfaceKHR());

		VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat (swapChainSupport.formats);
		VkPresentModeKHR presentMode = chooseSwapPresentMode (swapChainSupport.presentModes);
		VkExtent2D extent = chooseSwapExtent (swapChainSupport.capabilities);

		uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
		if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
			imageCount = swapChainSupport.capabilities.maxImageCount;
		}

		VkSwapchainCreateInfoKHR createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = _vulkanEnv.getVkSurfaceKHR ();

		createInfo.minImageCount = imageCount;
		createInfo.imageFormat = surfaceFormat.format;
		createInfo.imageColorSpace = surfaceFormat.colorSpace;
		createInfo.imageExtent = extent;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		QueueFamilyIndices indices = _GDevice.getQueueFamilies ();
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

		if (vkCreateSwapchainKHR (_GDevice.getDevice(), &createInfo, nullptr, &(_renderpipeline.swapChain)) != VK_SUCCESS) {
			throw std::runtime_error ("failed to create swap chain!");
		}

		vkGetSwapchainImagesKHR (_GDevice.getDevice (), _renderpipeline.swapChain, &imageCount, nullptr);
		_renderpipeline.swapChainImages.resize (imageCount);
		vkGetSwapchainImagesKHR (_GDevice.getDevice (), _renderpipeline.swapChain, &imageCount, _renderpipeline.swapChainImages.data ());

		_renderpipeline.swapChainImageFormat = surfaceFormat.format;
		_renderpipeline.swapChainExtent = extent;
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
			VkExtent2D actualExtent = { AspectRatio::Width () , AspectRatio::Height () };

			actualExtent.width = std::max (capabilities.minImageExtent.width, std::min (capabilities.maxImageExtent.width, actualExtent.width));
			actualExtent.height = std::max (capabilities.minImageExtent.height, std::min (capabilities.maxImageExtent.height, actualExtent.height));

			return actualExtent;
		}
	}

	void createImageViews () {
		_renderpipeline.swapChainImageViews.resize (_renderpipeline.swapChainImages.size ());

		for (size_t i = 0; i < _renderpipeline.swapChainImages.size (); i++) {
			VkImageViewCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			createInfo.image = _renderpipeline.swapChainImages[i];
			createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			createInfo.format = _renderpipeline.swapChainImageFormat;
			createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			createInfo.subresourceRange.baseMipLevel = 0;
			createInfo.subresourceRange.levelCount = 1;
			createInfo.subresourceRange.baseArrayLayer = 0;
			createInfo.subresourceRange.layerCount = 1;

			if (vkCreateImageView (_GDevice.getDevice (), &createInfo, nullptr, &(_renderpipeline.swapChainImageViews[i])) != VK_SUCCESS) {
				throw std::runtime_error ("failed to create image views!");
			}
		}
	}
	void createRenderPass () {
		VkAttachmentDescription colorAttachment = {};
		colorAttachment.format = _renderpipeline.swapChainImageFormat;
		colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentReference colorAttachmentRef = {};
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpass = {};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentRef;

		VkRenderPassCreateInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = 1;
		renderPassInfo.pAttachments = &colorAttachment;
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpass;

		if (vkCreateRenderPass (_GDevice.getDevice (), &renderPassInfo, nullptr, &(_renderpipeline.renderPass)) != VK_SUCCESS) {
			throw std::runtime_error ("failed to create render pass!");
		}
	}
	void createGraphicsPipeline () {
		auto vertShaderCode = readFile ("Ressources/Materials/vert.spv");
		auto fragShaderCode = readFile ("Ressources/Materials/frag.spv");

		VkShaderModule vertShaderModule = createShaderModule (vertShaderCode);
		VkShaderModule fragShaderModule = createShaderModule (fragShaderCode);

		VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
		vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
		vertShaderStageInfo.module = vertShaderModule;
		vertShaderStageInfo.pName = "main";

		VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
		fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		fragShaderStageInfo.module = fragShaderModule;
		fragShaderStageInfo.pName = "main";

		VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

		VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputInfo.vertexBindingDescriptionCount = 0;
		vertexInputInfo.vertexAttributeDescriptionCount = 0;

		VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
		inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		inputAssembly.primitiveRestartEnable = VK_FALSE;

		VkViewport viewport = {};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = (float)_renderpipeline.swapChainExtent.width;
		viewport.height = (float)_renderpipeline.swapChainExtent.height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		VkRect2D scissor = {};
		scissor.offset = { 0, 0 };
		scissor.extent = _renderpipeline.swapChainExtent;

		VkPipelineViewportStateCreateInfo viewportState = {};
		viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportState.viewportCount = 1;
		viewportState.pViewports = &viewport;
		viewportState.scissorCount = 1;
		viewportState.pScissors = &scissor;

		VkPipelineRasterizationStateCreateInfo rasterizer = {};
		rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizer.depthClampEnable = VK_FALSE;
		rasterizer.rasterizerDiscardEnable = VK_FALSE;
		rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
		rasterizer.lineWidth = 1.0f;
		rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
		rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
		rasterizer.depthBiasEnable = VK_FALSE;

		VkPipelineMultisampleStateCreateInfo multisampling = {};
		multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampling.sampleShadingEnable = VK_FALSE;
		multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

		VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
		colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		colorBlendAttachment.blendEnable = VK_FALSE;

		VkPipelineColorBlendStateCreateInfo colorBlending = {};
		colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlending.logicOpEnable = VK_FALSE;
		colorBlending.logicOp = VK_LOGIC_OP_COPY;
		colorBlending.attachmentCount = 1;
		colorBlending.pAttachments = &colorBlendAttachment;
		colorBlending.blendConstants[0] = 0.0f;
		colorBlending.blendConstants[1] = 0.0f;
		colorBlending.blendConstants[2] = 0.0f;
		colorBlending.blendConstants[3] = 0.0f;

		VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 0;
		pipelineLayoutInfo.pushConstantRangeCount = 0;

		if (vkCreatePipelineLayout (_GDevice.getDevice (), &pipelineLayoutInfo, nullptr, &(_renderpipeline.pipelineLayout)) != VK_SUCCESS) {
			throw std::runtime_error ("failed to create pipeline layout!");
		}

		VkGraphicsPipelineCreateInfo pipelineInfo = {};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.stageCount = 2;
		pipelineInfo.pStages = shaderStages;
		pipelineInfo.pVertexInputState = &vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &inputAssembly;
		pipelineInfo.pViewportState = &viewportState;
		pipelineInfo.pRasterizationState = &rasterizer;
		pipelineInfo.pMultisampleState = &multisampling;
		pipelineInfo.pColorBlendState = &colorBlending;
		pipelineInfo.layout = _renderpipeline.pipelineLayout;
		pipelineInfo.renderPass = _renderpipeline.renderPass;
		pipelineInfo.subpass = 0;
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

		if (vkCreateGraphicsPipelines (_GDevice.getDevice (), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &(_renderpipeline.graphicsPipeline)) != VK_SUCCESS) {
			throw std::runtime_error ("failed to create graphics pipeline!");
		}

		vkDestroyShaderModule (_GDevice.getDevice (), fragShaderModule, nullptr);
		vkDestroyShaderModule (_GDevice.getDevice (), vertShaderModule, nullptr);
	}
	VkShaderModule createShaderModule (const std::vector<char>& code) {
		VkShaderModuleCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = code.size ();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data ());

		VkShaderModule shaderModule;
		if (vkCreateShaderModule (_GDevice.getDevice (), &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
			throw std::runtime_error ("failed to create shader module!");
		}

		return shaderModule;
	}

	void createFramebuffers () {
		_renderpipeline.swapChainFramebuffers.resize (_renderpipeline.swapChainImageViews.size ());

		for (size_t i = 0; i < _renderpipeline.swapChainImageViews.size (); i++) {
			VkImageView attachments[] = {
				_renderpipeline.swapChainImageViews[i]
			};

			VkFramebufferCreateInfo framebufferInfo = {};
			framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferInfo.renderPass = _renderpipeline.renderPass;
			framebufferInfo.attachmentCount = 1;
			framebufferInfo.pAttachments = attachments;
			framebufferInfo.width = _renderpipeline.swapChainExtent.width;
			framebufferInfo.height = _renderpipeline.swapChainExtent.height;
			framebufferInfo.layers = 1;

			if (vkCreateFramebuffer (_GDevice.getDevice (), &framebufferInfo, nullptr, &(_renderpipeline.swapChainFramebuffers[i])) != VK_SUCCESS) {
				throw std::runtime_error ("failed to create framebuffer!");
			}
		}
	}
	void createCommandPool () {
		QueueFamilyIndices queueFamilyIndices = _GDevice.getQueueFamilies();

		VkCommandPoolCreateInfo poolInfo = {};
		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily;
		poolInfo.flags = 0; // Optionel

		if (vkCreateCommandPool (_GDevice.getDevice (), &poolInfo, nullptr, &commandPool) != VK_SUCCESS) {
			throw std::runtime_error ("échec de la création d'une command pool!");
		}
	}

	void createCommandBuffers () {
		commandBuffers.resize (_renderpipeline.swapChainFramebuffers.size ());

		VkCommandBufferAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = commandPool;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = (uint32_t)commandBuffers.size ();

		if (vkAllocateCommandBuffers (_GDevice.getDevice (), &allocInfo, commandBuffers.data ()) != VK_SUCCESS) {
			throw std::runtime_error ("failed to allocate command buffers!");
		}

		for (size_t i = 0; i < commandBuffers.size (); i++) {
			VkCommandBufferBeginInfo beginInfo = {};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

			if (vkBeginCommandBuffer (commandBuffers[i], &beginInfo) != VK_SUCCESS) {
				throw std::runtime_error ("failed to begin recording command buffer!");
			}

			VkRenderPassBeginInfo renderPassInfo = {};
			renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			renderPassInfo.renderPass = _renderpipeline.renderPass;
			renderPassInfo.framebuffer = _renderpipeline.swapChainFramebuffers[i];
			renderPassInfo.renderArea.offset = { 0, 0 };
			renderPassInfo.renderArea.extent = _renderpipeline.swapChainExtent;

			VkClearValue clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
			renderPassInfo.clearValueCount = 1;
			renderPassInfo.pClearValues = &clearColor;

			vkCmdBeginRenderPass (commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

			vkCmdBindPipeline (commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, _renderpipeline.graphicsPipeline);

			vkCmdDraw (commandBuffers[i], 3, 1, 0, 0);

			vkCmdEndRenderPass (commandBuffers[i]);

			if (vkEndCommandBuffer (commandBuffers[i]) != VK_SUCCESS) {
				throw std::runtime_error ("failed to record command buffer!");
			}
		}
	}

	void createSyncObjects () {
		imageAvailableSemaphores.resize (MAX_FRAMES_IN_FLIGHT);
		renderFinishedSemaphores.resize (MAX_FRAMES_IN_FLIGHT);
		inFlightFences.resize (MAX_FRAMES_IN_FLIGHT);
		imagesInFlight.resize (_renderpipeline.swapChainImages.size (), VK_NULL_HANDLE);

		VkSemaphoreCreateInfo semaphoreInfo = {};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkFenceCreateInfo fenceInfo = {};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
			if (vkCreateSemaphore (_GDevice.getDevice (), &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
				vkCreateSemaphore (_GDevice.getDevice (), &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
				vkCreateFence (_GDevice.getDevice (), &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS) {
				throw std::runtime_error ("failed to create synchronization objects for a frame!");
			}
		}
	}
	void drawFrame () {
		vkWaitForFences (_GDevice.getDevice (), 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

		uint32_t imageIndex;
		vkAcquireNextImageKHR (_GDevice.getDevice (), _renderpipeline.swapChain, UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

		if (imagesInFlight[imageIndex] != VK_NULL_HANDLE) {
			vkWaitForFences (_GDevice.getDevice (), 1, &imagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);
		}
		imagesInFlight[imageIndex] = inFlightFences[currentFrame];

		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		VkSemaphore waitSemaphores[] = { imageAvailableSemaphores[currentFrame] };
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;

		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffers[imageIndex];

		VkSemaphore signalSemaphores[] = { renderFinishedSemaphores[currentFrame] };
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		vkResetFences (_GDevice.getDevice (), 1, &inFlightFences[currentFrame]);

		if (vkQueueSubmit (_GDevice.getGfxQueue(), 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS) {
			throw std::runtime_error ("failed to submit draw command buffer!");
		}

		VkPresentInfoKHR presentInfo = {};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;

		VkSwapchainKHR swapChains[] = { _renderpipeline.swapChain };
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;

		presentInfo.pImageIndices = &imageIndex;

		vkQueuePresentKHR (_GDevice.getPrtQueue(), &presentInfo);

		currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
	}
};

void rlms::GraphicsManagerImpl::start (Allocator* const& alloc, size_t mesh_pool_size, GLFWwindow*& window, std::shared_ptr<Logger> funnel) {
	startLogger (funnel);
	logger->tag (LogTags::None) << "Initializing !" << '\n';

	_vulkanEnv.start (window);
	_GDevice.start (&_vulkanEnv.getVkSurfaceKHR (), &_vulkanEnv.getVKInstance ());

	createSwapChain ();
	createImageViews ();

	createRenderPass ();
	createGraphicsPipeline ();

	createFramebuffers ();

	createCommandPool ();
	createCommandBuffers ();

	createSyncObjects ();

	logger->tag (LogTags::None) << "Initialized correctly !" << '\n';
}

void rlms::GraphicsManagerImpl::stop () {
	logger->tag (LogTags::None) << "Terminating" << '\n';

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		vkDestroySemaphore (_GDevice.getDevice(), renderFinishedSemaphores[i], nullptr);
		vkDestroySemaphore (_GDevice.getDevice (), imageAvailableSemaphores[i], nullptr);
		vkDestroyFence (_GDevice.getDevice (), inFlightFences[i], nullptr);
	}

	vkDestroyCommandPool (_GDevice.getDevice (), commandPool, nullptr);

	for (auto framebuffer : _renderpipeline.swapChainFramebuffers) {
		vkDestroyFramebuffer (_GDevice.getDevice (), framebuffer, nullptr);
	}

	vkDestroyPipeline (_GDevice.getDevice (), _renderpipeline.graphicsPipeline, nullptr);
	vkDestroyPipelineLayout (_GDevice.getDevice (), _renderpipeline.pipelineLayout, nullptr);
	vkDestroyRenderPass (_GDevice.getDevice (), _renderpipeline.renderPass, nullptr);

	for (auto imageView : _renderpipeline.swapChainImageViews) {
		vkDestroyImageView (_GDevice.getDevice (), imageView, nullptr);
	}
	vkDestroySwapchainKHR (_GDevice.getDevice (), _renderpipeline.swapChain, nullptr);

	_GDevice.stop ();
	_vulkanEnv.stop ();

	logger->tag (LogTags::None) << "Terminated correctly !" << '\n';
}

void rlms::GraphicsManagerImpl::load () {

}

void rlms::GraphicsManagerImpl::draw () {
	drawFrame ();

	vkDeviceWaitIdle (_GDevice.getDevice());
}

void rlms::GraphicsManagerImpl::unload () {}

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

void RenderPipeline::start () {}

void RenderPipeline::stop () {}
