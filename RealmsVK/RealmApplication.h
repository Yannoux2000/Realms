#pragma once
#include <GLFW/glfw3.h>

#include "Base/Logging/DebugConsoleLogger.h"
#include "Base/Logging/FileLogger.h"

#include "Base/Allocators/ProxyAllocator.h"
//#include "Base/Allocators/StackAllocator.h"
#include "Base/Allocators/MasqueradeAllocator.h"

#include "Utility/FileIO/VoxFileParser.h"

#include "Module/Input/InputManager.h"

#include "Module/Graphics/AspectRatio.h"
#include "Module/Graphics/GraphicsManager.h"

#include "Module/ECS/GameCore.h"
#include "Module/ECS/TransformComponent.h"

#include "Module/World/BlockRegister.h"
#include "Module/World/Chunk.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <chrono>
#include <array>
#include <sstream>

#include "Module/LuaBindings/lua.h"

namespace rlms {
	class RealmApplication : public ILogged {
	private:
		using _allocType = MasqueradeAllocator;


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

			running = true;
			initMemory (stgs);
			initWindow (stgs);
			initGraphics (stgs);
			initInputs (stgs);
			initGameCore (stgs);
			logger->tag (LogTags::None) << "Application is Started.\n";
		}

		void run () {
			logger->tag (LogTags::None) << "Running Application.\n";
			while (running && !glfwWindowShouldClose(window)) {
				auto start = std::chrono::high_resolution_clock::now ();

				//inputManager
				InputLoop ();

				auto input_end = std::chrono::high_resolution_clock::now ();
				std::chrono::duration<double, std::milli> fp_in_ms = input_end - start;

				//updateManager
				UpdateLoop ();

				auto update_end = std::chrono::high_resolution_clock::now ();
				std::chrono::duration<double, std::milli> fp_up_ms = update_end - start;

				//modelManager
				ModelLoop ();

				auto model_end = std::chrono::high_resolution_clock::now ();
				std::chrono::duration<double, std::milli> fp_mdl_ms = model_end - start;

				//graphicsManager
				DisplayLoop ();

				auto graphics_end = std::chrono::high_resolution_clock::now ();
				std::chrono::duration<double, std::milli> fp_gfx_ms = graphics_end - start;

				//logger->tag (LogTags::Dev) 
				//	<< "inputs :" << fp_in_ms.count () << "ms;"
				//	<< " updates :" << fp_up_ms.count () << "ms;"
				//	<< "models :" << fp_mdl_ms.count () << "ms;"
				//	<< "display :" << fp_gfx_ms.count () << "ms\n";
			}
		}

		void stop () {
			logger->tag (LogTags::None) << "Terminating Application.\n";
			termInputs ();
			termGraphics ();
			termWindow ();
			termMemory ();
			termGameCore ();
			logger->tag (LogTags::None) << "Application has Terminated.\n";
		}

	private:
		GLFWwindow* window;

		bool running = false;
		std::unique_ptr<_allocType> app_alloc;

		std::string getLogName () override {
			return "Realms";
		};

		void initMemory (ApplicationSettings& stgs) {
			void* mem = malloc (stgs.memory.total_size);
			app_alloc = std::make_unique<_allocType> (mem, stgs.memory.total_size);
		}
		void initWindow (ApplicationSettings& stgs) {
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
		void initInputs (ApplicationSettings& stgs) {
			rlms::InputManager::Initialize (logger);

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
		void initGraphics (ApplicationSettings& stgs) {
			rlms::GraphicsManager::Initialize (app_alloc.get (), stgs.memory.mesh_size, window, logger);
			rlms::GraphicsManager::Load ();
		}
		void initGameCore (ApplicationSettings& stgs) {
			rlms::ECS_Core::Initialize (app_alloc.get (), stgs.memory.ecs_size, logger);

			logger->tag (LogTags::Dev) << "Testing ECS_Core's Entity system.\n";
/*
			ENTITY_ID e = Entity::NULL_ID;
			Entity* p_e;
			//e = rlms::EntityManager::Create ();
			//p_e = rlms::EntityManager::Get (e);
			//p_e->setType ("ghost");

			//auto p = p_e->get<IComponent> ();

			//auto c = rlms::ComponentManager::CreateComponent<TransformComponent> (p_e);
			//auto p_c = rlms::ComponentManager::GetComponent<TransformComponent> (e);
			//p_c->phrase = "Wow i'm so scary";

			lua_State* L = luaL_newstate (); 
			luaL_dostring (L,
				R"(ghost = {
					TransformComponent = {
						phrase = "I'M A SCARY GHOST!!!"
					}
				})");

			lua_getglobal (L, "ghost");
			assert (lua_istable (L, -1));
			lua_getfield (L, -1, "TransformComponent");

			lua_close (L);
			p_e = rlms::EntityManager::Get (e);
			if (p_e->has<TransformComponent> ()) {
				logger->tag (LogTags::Info) << "Good the component is register\n";

				auto comp = p_e->get<TransformComponent> ();

				logger->tag (LogTags::Dev) << p_e->getType () << " says :" << comp->phrase << "\n";
			}
			logger->tag (LogTags::Dev) << "Tests done!\n";*/
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