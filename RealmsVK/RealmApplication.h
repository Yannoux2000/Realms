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

#include "Module/ECS/ECS_Core.h"
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

		AssignAddress game_quit;

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
			logger->tag (LogTags::None) << "Initializing Application.\n";

			//read all graphics and controls and mods options

			running = true;
			initMemory (stgs);
			initWindow (stgs);
			initGraphics (stgs);
			initInputs (stgs);
			initGameCore (stgs);
			logger->tag (LogTags::None) << "Application is Ready.\n";
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
			app_alloc = std::unique_ptr<_allocType> (new _allocType (stgs.memory.total_size, mem));
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
			InputManager::Initialize (logger);

			InputManager::SetCallbacks (window);

			//InputManager::BindEvent ("close", sf::Event::Closed);
			//InputManager::BindEvent ("resized", sf::Event::Resized);

			// chargement des ressources, initialisation des états OpenGL, ...
			InputManager::BindKey ("game::quit", glfwGetKeyScancode (GLFW_KEY_ESCAPE));
			game_quit = InputManager::GetInput ("game::quit");
			InputManager::EnableInputMap (game_quit.map);

			//InputManager::BindKey ("cam::reset", glfwGetKeyScancode (GLFW_KEY_SPACE));
			//InputManager::BindKey ("cam::test", glfwGetKeyScancode (GLFW_KEY_E));
			//InputManager::BindKey ("cam::reload", glfwGetKeyScancode (GLFW_KEY_R));
			//InputManager::BindKey ("cam::zoom", glfwGetKeyScancode (GLFW_KEY_PAGE_UP));

			//InputManager::BindSlide ("cam::look", sf::Mouse::Right);

			//InputManager::BindKey ("cam::forward", glfwGetKeyScancode (GLFW_KEY_Z));
			//InputManager::BindKey ("cam::left", glfwGetKeyScancode (GLFW_KEY_Q));
			//InputManager::BindKey ("cam::backward", glfwGetKeyScancode (GLFW_KEY_S));
			//InputManager::BindKey ("cam::right", glfwGetKeyScancode (GLFW_KEY_D));
			//InputManager::EnableInputMap ("cam");
		}
		void initGraphics (ApplicationSettings& stgs) {
			GraphicsManager::Initialize (app_alloc.get (), stgs.memory.mesh_size, window, logger);
			GraphicsManager::Load ();
		}
		void initGameCore (ApplicationSettings& stgs) {
			ECS_Core::Initialize (app_alloc.get (), stgs.memory.ecs_size, logger);

			logger->tag (LogTags::Dev) << "Testing ECS_Core's Entity system.\n";
/*
			ENTITY_ID e = Entity::NULL_ID;
			Entity* p_e;
			//e = EntityManager::Create ();
			//p_e = EntityManager::Get (e);
			//p_e->setType ("ghost");

			//auto p = p_e->get<IComponent> ();

			//auto c = ComponentManager::CreateComponent<TransformComponent> (p_e);
			//auto p_c = ComponentManager::GetComponent<TransformComponent> (e);
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
			p_e = EntityManager::Get (e);
			if (p_e->has<TransformComponent> ()) {
				logger->tag (LogTags::Info) << "Good the component is register\n";

				auto comp = p_e->get<TransformComponent> ();

				logger->tag (LogTags::Dev) << p_e->getType () << " says :" << comp->phrase << "\n";
			}
			logger->tag (LogTags::Dev) << "Tests done!\n";*/
		}

		void InputLoop () {
			InputManager::Poll (window);
			if (InputManager::IsPressed (game_quit)) {
				running = false;
			}
		}
		void UpdateLoop () {

		}
		void ModelLoop () {

		}
		void DisplayLoop () {
			GraphicsManager::Draw ();
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
			GraphicsManager::Unload ();
			GraphicsManager::Terminate ();
		}
		void termGameCore () {
			ECS_Core::Terminate ();
		}
	};
}