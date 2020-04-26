#pragma once
#include "IRenderer.h"
#include <GLFW/glfw3.h>

#include <vector>

namespace rlms {
	class IContext {
	protected:
		// differente rendering pipelines are encoded inside a derivated class of IVulkanRenderPass
		IRenderer* renderer;

		// allows for custom shader importations
		//std::map<VkShaderStageFlagBits, std::string> shaderPaths;
		std::vector<std::string> shaderPaths;

	public:
		IContext () : renderer (nullptr), shaderPaths ({}) {}

		virtual ~IContext () {};

		void setShader (std::string const& path) {
			shaderPaths.push_back (path);
		}

		void setRenderer (IRenderer* r_renderer) {
			renderer = r_renderer;
		}

		virtual void start (GLFWwindow*& window) = 0;

		virtual void draw () {
			renderer->draw ();
		}

		virtual void stop () = 0;
	};
}
