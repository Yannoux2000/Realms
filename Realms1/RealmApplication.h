#pragma once
#include "Base/Logging/DebugConsoleLogger.h"
#include "Base/Logging/FileLogger.h"
#include "Base/Math/VoxelMath.h"

#include "Base/Allocators/ProxyAllocator.h"

#include "Utility/FileIO/VoxFileParser.h"

#include <SFML/Window.hpp>

#include "Module/Input/InputManager.h"

#include "Module/Graphics/GLError.h"
#include "Module/Graphics/AspectRatio.h"

#include "Module/Graphics/GraphicsManager.h"

#include "Module/World/BlockRegister.h"
#include "Module/World/Chunk.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <chrono>
#include <array>
#include <sstream>

namespace rlms {
	class RealmApplication : public ILogged {
	public:
		struct MemorySettings {
			size_t total_size = 4096;
			size_t mesh_size = 4096;

		};

		struct ApplicationSettings {
			MemorySettings memory;
			const unsigned int aspect_width = 800;
			const unsigned int aspect_height = 600;
		};

		void start (ApplicationSettings& stgs) {
			startLogger (nullptr, true);
			logger->tag (LogTags::None) << "Starting Application.\n";

			//read all graphics and controls and mods options

			void* mem = malloc (stgs.memory.total_size);
			app_alloc = std::make_unique<ProxyAllocator> (mem, stgs.memory.total_size);

			running = true;
			initWindow (stgs);
			initInputs (stgs);
			initGraphics (stgs);
		}

		void run () {
			logger->tag (LogTags::None) << "Running Application.\n";
			while (running) {
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
			}
		}

		void stop () {
			GraphicsManager::Unload ();
			GraphicsManager::Terminate ();
			InputManager::Terminate ();

			//delete windows

			void* mem = app_alloc->getStart ();
			app_alloc.reset ();
			free (mem);
		}

	private:
		sf::Window* window;
		bool running = false;

		std::unique_ptr<ProxyAllocator> app_alloc;

		std::string getLogName () override {
			return "Realms";
		};

		void initWindow (ApplicationSettings& stgs) {
			logger->tag (LogTags::Debug) << "Initializing Window.";

			sf::ContextSettings settings;
			settings.depthBits = 24;
			settings.stencilBits = 8;
			settings.antialiasingLevel = 8;
			settings.majorVersion = 3;
			settings.minorVersion = 3;
			settings.attributeFlags = settings.Core;

			AspectRatio::Set (stgs.aspect_width, stgs.aspect_height);
			logger->tag (LogTags::Dev) << "( " << AspectRatio::Width () << ", " << AspectRatio::Height () << ")\n";

			window = new sf::Window (sf::VideoMode (AspectRatio::Width (), AspectRatio::Height (), 32), "Realms", sf::Style::Titlebar | sf::Style::Close, settings);

		}

		void initInputs (ApplicationSettings& stgs) {
			rlms::InputManager::Initialize (logger);

			rlms::InputManager::BindEvent ("close", sf::Event::Closed);
			rlms::InputManager::BindEvent ("resized", sf::Event::Resized);

			// chargement des ressources, initialisation des états OpenGL, ...
			rlms::InputManager::BindInput ("game::quit", sf::Keyboard::Escape);
			rlms::InputManager::EnableInputMap ("game");

			rlms::InputManager::BindInput ("cam::reset", sf::Keyboard::Space);
			rlms::InputManager::BindInput ("cam::test", sf::Keyboard::E);
			rlms::InputManager::BindInput ("cam::reload", sf::Keyboard::R);
			rlms::InputManager::BindInput ("cam::zoom", sf::Keyboard::Add);

			rlms::InputManager::BindSlide ("cam::look", sf::Mouse::Right);

			rlms::InputManager::BindInput ("cam::forward", sf::Keyboard::Z);
			rlms::InputManager::BindInput ("cam::left", sf::Keyboard::Q);
			rlms::InputManager::BindInput ("cam::backward", sf::Keyboard::S);
			rlms::InputManager::BindInput ("cam::right", sf::Keyboard::D);
			rlms::InputManager::EnableInputMap ("cam");
		}

		void initGraphics (ApplicationSettings& stgs) {
			rlms::GraphicsManager::Initialize (app_alloc.get (), stgs.memory.mesh_size, logger);

			rlms::GraphicsManager::Register (0, "Models/Default/Workbench.vox");
			rlms::GraphicsManager::Register (1, "Models/Default/Blocks/Dirt.vox");
			rlms::GraphicsManager::Register (2, "Models/Default/Blocks/Wood.vox");
			rlms::GraphicsManager::Register (3, "Models/Default/Perso_m1.vox");

			try {
				rlms::GraphicsManager::LoadModels ();
				openGL_Error_Poll ();
				rlms::GraphicsManager::LoadRenderer ();
				openGL_Error_Poll ();
				rlms::GraphicsManager::Load ();
				openGL_Error_Poll ();

			} catch (RlmsException& e) {
				logger->tag (LogTags::Error) << e.what () << "\n";
				running = false;
			}
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
				delta.x = vec.x;
				delta.y = vec.y;
				Camera::MainCamera->look (delta);
			}
		}

		void UpdateLoop () {

		}

		void ModelLoop () {

		}

		void DisplayLoop () {
			rlms::GraphicsManager::Draw ();
			// Termine la frame courante (en interne, échange les deux tampons de rendu)
			window->display ();
		}
	};
}