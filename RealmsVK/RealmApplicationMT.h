#pragma once
#include <GLFW/glfw3.h>

#include "Base/Logging/DebugConsoleLogger.h"
#include "Base/Logging/FileLogger.h"

#include "Base/Allocators/ProxyAllocator.h"
//#include "Base/Allocators/StackAllocator.h"
#include "Base/Allocators/MasqueradeAllocator.h"

//#include "Utility/FileIO/VoxFileParser.h"
#include "Utility/MultiThreading/JobSystem.h"

#include "Module/Input/InputManager.h"

#include "Module/Graphics/AspectRatio.h"
#include "Module/Graphics/GraphicsManager.h"

#include "Module/ECS/ECS_Core.h"
#include "Module/ECS/TransformComponent.h"

#include "Module/World/BlockRegister.h"
#include "Module/World/Chunk.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <chrono>
#include <array>
#include <future>
#include <sstream>

#include "Module/LuaBindings/lua.h"


namespace rlms {
	class RealmApplication : public ILogged {
	private:
		using _allocType = MasqueradeAllocator;

		std::atomic_bool runInput;
		std::atomic_bool runGraphics;
		std::atomic_bool runUpdate;

	public:
		struct MemorySettings {
			size_t total_size = 16384;
			size_t mesh_size = 4096;
			size_t ecs_size = 8192;
		};

		struct ApplicationSettings {
			MemorySettings memory;
			unsigned int aspect_width = 800;
			unsigned int aspect_height = 600;
			bool fullscreen = false;
		};

		void start (ApplicationSettings& stgs) {
			startLogger (nullptr, true);
			logger->tag (LogTags::None) << "Starting Application.\n";

			//read all graphics and controls and mods options
			initMemory (stgs);
			JobSystem::Initialize (logger);

			JobSystem::Register (Job (
				[this, stgs]() {

					initWindow (stgs);
					initGraphics (stgs);

					while (runGraphics) {
						DisplayLoop ();
					}

					termGraphics ();
					termWindow ();
				}
			, 2000));


			JobSystem::Register (Job (
				[this, stgs]() {
					rlms::ECS_Core::Initialize (app_alloc.get (), stgs.memory.ecs_size, logger);
				}
			, 2000));


			JobSystem::Register (Job (
				[this, stgs]() {

					initInputs (stgs);

					while (runInput) {
						InputLoop ();
					}

					InputManager::Terminate ();
				}
			, 2000));

			logger->tag (LogTags::None) << "Application is Started.\n";
			logger->tag (LogTags::None) << "Running Application.\n";
			JobSystem::Pass ();
		}

		void run () {
		}

		void stop () {
			logger->tag (LogTags::None) << "Terminating Application.\n";

			termInputs ();

			termGameCore ();

			termGraphics ();
			termWindow ();

			JobSystem::Terminate ();
			termMemory ();
			logger->tag (LogTags::None) << "Application has Terminated.\n";
		}

	private:
		GLFWwindow* window;

		bool running = false;
		std::unique_ptr<_allocType> app_alloc;

		std::string getLogName () override {
			return "Realms";
		};

		void initMemory (ApplicationSettings const& stgs) {
			void* mem = malloc (stgs.memory.total_size);
			app_alloc = std::unique_ptr<_allocType> ( new _allocType (stgs.memory.total_size, mem));
		}
		void initWindow (ApplicationSettings const& stgs) {
			logger->tag (LogTags::Debug) << "Initializing Window.";

			AspectRatio::Set (stgs.aspect_width, stgs.aspect_height);
			logger->tag (LogTags::Dev) << "( " << AspectRatio::Width () << ", " << AspectRatio::Height () << ")\n";

			// crée la fenêtre
			glfwInit ();

			glfwWindowHint (GLFW_CLIENT_API, GLFW_NO_API);
			glfwWindowHint (GLFW_RESIZABLE, GLFW_FALSE);
			glfwWindowHint (GLFW_SCALE_TO_MONITOR, GLFW_TRUE);

			if (stgs.fullscreen) {
				GLFWmonitor* mon = glfwGetPrimaryMonitor ();
				AspectRatio::Set (1920, 1080);
				window = glfwCreateWindow (AspectRatio::Width (), AspectRatio::Height (), "Realms", mon, nullptr);
			} else {
				window = glfwCreateWindow (AspectRatio::Width (), AspectRatio::Height (), "Realms", nullptr, nullptr);
			}
		}
		void initInputs (ApplicationSettings const& stgs) {
			rlms::InputManager::Initialize ();

			rlms::InputManager::SetCallbacks (window);

			//rlms::InputManager::BindEvent ("close", sf::Event::Closed);
			//rlms::InputManager::BindEvent ("resized", sf::Event::Resized);

			// chargement des ressources, initialisation des états OpenGL, ...
			rlms::InputManager::BindKey ("game::quit", glfwGetKeyScancode (GLFW_KEY_ESCAPE));
			rlms::InputManager::EnableInputMap ("game");

			rlms::InputManager::BindKey ("cam::reset", glfwGetKeyScancode (GLFW_KEY_SPACE));
			rlms::InputManager::BindKey ("cam::test", glfwGetKeyScancode (GLFW_KEY_E));
			rlms::InputManager::BindKey ("cam::reload", glfwGetKeyScancode (GLFW_KEY_R));
			rlms::InputManager::BindKey ("cam::zoom", glfwGetKeyScancode (GLFW_KEY_PAGE_UP));

			//rlms::InputManager::BindSlide ("cam::look", sf::Mouse::Right);

			rlms::InputManager::BindKey ("cam::forward", glfwGetKeyScancode (GLFW_KEY_Z));
			rlms::InputManager::BindKey ("cam::left", glfwGetKeyScancode (GLFW_KEY_Q));
			rlms::InputManager::BindKey ("cam::backward", glfwGetKeyScancode (GLFW_KEY_S));
			rlms::InputManager::BindKey ("cam::right", glfwGetKeyScancode (GLFW_KEY_D));
			rlms::InputManager::EnableInputMap ("cam");
		}
		void initGraphics (ApplicationSettings const& stgs) {
			rlms::GraphicsManager::Initialize (app_alloc.get (), stgs.memory.mesh_size, window);
			rlms::GraphicsManager::Load ();
		}
		void initGameCore (ApplicationSettings const& stgs) {
			rlms::ECS_Core::Initialize (app_alloc.get (), stgs.memory.ecs_size);
		}

		void InputLoop () {
			rlms::InputManager::Poll (window);
			if (rlms::InputManager::IsPressed ("game::quit") || rlms::InputManager::IsPressed ("close")) {
				running = false;
			}

			if (rlms::InputManager::IsPressed ("cam::set")) {
				logger->tag (LogTags::Debug) << "Enter\n";
			}

			if (rlms::InputManager::IsDown ("cam::forward")) {
				Camera::MainCamera->move (Camera::MainCamera->forward);
			}

			if (rlms::InputManager::IsDown ("cam::backward")) {
				Camera::MainCamera->move (-Camera::MainCamera->forward);
			}

			if (rlms::InputManager::IsDown ("cam::left")) {
				Camera::MainCamera->move (-Camera::MainCamera->right);
			}

			if (rlms::InputManager::IsDown ("cam::right")) {
				Camera::MainCamera->move (Camera::MainCamera->right);
			}

			if (rlms::InputManager::IsPressed ("cam::reset")) {
				Camera::MainCamera->reset ();
			}

			if (rlms::InputManager::IsDown ("cam::look")) {
				glm::ivec2 delta;
				auto vec = InputManager::GetDeltaPos ("cam::look");
				delta.x = vec[0];
				delta.y = vec[1];
				Camera::MainCamera->look (delta);
			}
		}
		void UpdateLoop () {

		}
		void ModelLoop () {

		}
		void DisplayLoop () {
			rlms::GraphicsManager::Draw ();
		}
		void GameCoreLoop () {

		}

		void termMemory () {
			void* mem = app_alloc->getStart ();
			app_alloc.reset ();
			free (mem);
		}
		void termWindow () {
			glfwDestroyWindow (window);
			glfwTerminate ();
		}
		void termInputs () {
			InputManager::Terminate ();
		}
		void termGraphics () {
			rlms::GraphicsManager::Unload ();
			rlms::GraphicsManager::Terminate ();
		}
		void termGameCore () {
			rlms::ECS_Core::Terminate ();
		}
	};
}