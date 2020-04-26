//#pragma once
//#include "Base/Logging/DebugConsoleLogger.h"
//#include "Base/Logging/FileLogger.h"
//#include "Base/Math/VoxelMath.h"
//
//#include "Base/Allocators/ProxyAllocator.h"
//
//#include "Utility/FileIO/VoxFileParser.h"
//
//#include "Module/Input/InputManager.h"
//
//#include "Module/Graphics/AspectRatio.h"
//
//#include "Module/World/BlockRegister.h"
//#include "Module/World/Chunk.h"
//
//#include <glm/glm.hpp>
//#include <glm/gtc/matrix_transform.hpp>
//#include <glm/gtc/type_ptr.hpp>
//
//#include <chrono>
//#include <array>
//#include <sstream>
//
//namespace rlms {
//	class RealmApplication : public ILogged {
//	public:
//		struct MemorySettings {
//			size_t total_size = 4096;
//			size_t mesh_size = 4096;
//
//		};
//
//		struct ApplicationSettings {
//			MemorySettings memory;
//			const unsigned int aspect_width = 800;
//			const unsigned int aspect_height = 600;
//		};
//
//		void start (ApplicationSettings& stgs) {
//			startLogger (nullptr, true);
//			logger->tag (LogTags::None) << "Starting Application.\n";
//
//			//read all graphics and controls and mods options
//
//			void* mem = malloc (stgs.memory.total_size);
//			app_alloc = std::make_unique<ProxyAllocator> (mem, stgs.memory.total_size);
//
//			running = true;
//			initWindow (stgs);
//			initInputs (stgs);
//			initGraphics (stgs);
//		}
//
//		void run () {
//			logger->tag (LogTags::None) << "Running Application.\n";
//			while (running) {
//				auto start = std::chrono::high_resolution_clock::now ();
//
//				//inputManager
//				InputLoop ();
//
//				auto input_end = std::chrono::high_resolution_clock::now ();
//				std::chrono::duration<double, std::milli> fp_in_ms = input_end - start;
//
//				//updateManager
//				UpdateLoop ();
//
//				auto update_end = std::chrono::high_resolution_clock::now ();
//				std::chrono::duration<double, std::milli> fp_up_ms = update_end - start;
//
//				//modelManager
//				ModelLoop ();
//
//				auto model_end = std::chrono::high_resolution_clock::now ();
//				std::chrono::duration<double, std::milli> fp_mdl_ms = model_end - start;
//
//				//graphicsManager
//				DisplayLoop ();
//
//				auto graphics_end = std::chrono::high_resolution_clock::now ();
//				std::chrono::duration<double, std::milli> fp_gfx_ms = graphics_end - start;
//			}
//		}
//
//		void stop () {
//			GraphicsManagerOld::Unload ();
//			GraphicsManagerOld::Terminate ();
//			InputManager::Terminate ();
//			delete window;
//			void* mem = app_alloc->getStart ();
//			app_alloc.reset ();
//			free (mem);
//		}
//
//	private:
//		bool running = false;
//		sf::Window* window;
//		std::unique_ptr<ProxyAllocator> app_alloc;
//
//		std::string getLogName () override {
//			return "Realms";
//		};
//
//		void initWindow (ApplicationSettings& stgs) {
//			logger->tag (LogTags::Debug) << "Initializing Window.";
//			sf::ContextSettings settings;
//			settings.depthBits = 24;
//			settings.stencilBits = 8;
//			settings.antialiasingLevel = 8;
//			settings.majorVersion = 3;
//			settings.minorVersion = 3;
//			settings.attributeFlags = settings.Core;
//
//			AspectRatio::Set (stgs.aspect_width, stgs.aspect_height);
//
//			logger->tag (LogTags::Dev) << "( " << AspectRatio::Width () << ", " << AspectRatio::Height () << ")\n";
//
//			// crée la fenêtre
//			window = new sf::Window (sf::VideoMode (AspectRatio::Width (), AspectRatio::Height (), 32), "OmegaLul", sf::Style::Titlebar | sf::Style::Close, settings);
//
//			// Initialize GLEW
//			glewExperimental = GL_TRUE;
//			glewInit ();
//
//			//constant settings
//			// Enable depth test
//			// Accept fragment if it closer to the camera than the former one
//			glEnable (GL_DEPTH_TEST);
//			glDepthFunc (GL_LESS);
//
//			window->setVerticalSyncEnabled (true);
//			window->setMouseCursorGrabbed (false);
//			window->setFramerateLimit (60);
//		}
//
//		void initInputs (ApplicationSettings& stgs) {
//			InputManager::Initialize (logger);
//
//			InputManager::BindEvent ("close", sf::Event::Closed);
//			InputManager::BindEvent ("resized", sf::Event::Resized);
//
//			// chargement des ressources, initialisation des états OpenGL, ...
//			InputManager::BindInput ("game::quit", sf::Keyboard::Escape);
//			InputManager::EnableInputMap ("game");
//
//			InputManager::BindInput ("cam::reset", sf::Keyboard::Space);
//			InputManager::BindInput ("cam::test", sf::Keyboard::E);
//			InputManager::BindInput ("cam::reload", sf::Keyboard::R);
//			InputManager::BindInput ("cam::zoom", sf::Keyboard::Add);
//
//			InputManager::BindSlide ("cam::look", sf::Mouse::Right);
//
//			InputManager::BindInput ("cam::forward", sf::Keyboard::Z);
//			InputManager::BindInput ("cam::left", sf::Keyboard::Q);
//			InputManager::BindInput ("cam::backward", sf::Keyboard::S);
//			InputManager::BindInput ("cam::right", sf::Keyboard::D);
//			InputManager::EnableInputMap ("cam");
//		}
//
//		void initGraphics (ApplicationSettings& stgs) {
//			GraphicsManagerOld::Initialize (app_alloc.get (), stgs.memory.mesh_size, logger);
//
//			GraphicsManagerOld::Register (0, "Models/Default/Workbench.vox");
//			GraphicsManagerOld::Register (1, "Models/Default/Blocks/Dirt.vox");
//			GraphicsManagerOld::Register (2, "Models/Default/Blocks/Wood.vox");
//			GraphicsManagerOld::Register (3, "Models/Default/Perso_m1.vox");
//
//			try {
//				GraphicsManagerOld::LoadModels ();
//				openGL_Error_Poll ();
//				GraphicsManagerOld::LoadRenderer ();
//				openGL_Error_Poll ();
//				GraphicsManagerOld::Load ();
//				openGL_Error_Poll ();
//
//			} catch (RlmsException& e) {
//				logger->tag (LogTags::Error) << e.what () << "\n";
//				running = false;
//			}
//		}
//
//		void InputLoop () {
//			InputManager::Poll (window);
//			if (InputManager::IsPressed ("game::quit") || InputManager::IsPressed ("close")) {
//				running = false;
//			}
//
//			if (InputManager::IsPressed ("cam::set")) {
//				logger->tag (LogTags::Debug) << "Enter\n";
//			}
//
//			if (InputManager::IsDown ("cam::forward")) {
//				Camera::MainCamera->move (Camera::MainCamera->forward);
//			}
//
//			if (InputManager::IsDown ("cam::backward")) {
//				Camera::MainCamera->move (-Camera::MainCamera->forward);
//			}
//
//			if (InputManager::IsDown ("cam::left")) {
//				Camera::MainCamera->move (-Camera::MainCamera->right);
//			}
//
//			if (InputManager::IsDown ("cam::right")) {
//				Camera::MainCamera->move (Camera::MainCamera->right);
//			}
//
//			if (InputManager::IsPressed ("cam::reset")) {
//				Camera::MainCamera->reset ();
//			}
//
//			if (InputManager::IsDown ("cam::look")) {
//				glm::ivec2 delta;
//				auto vec = InputManager::GetDeltaPos ("cam::look");
//				delta.x = vec.x;
//				delta.y = vec.y;
//				Camera::MainCamera->look (delta);
//			}
//		}
//
//		void UpdateLoop () {
//
//		}
//
//		void ModelLoop () {
//
//		}
//
//		void DisplayLoop () {
//			GraphicsManagerOld::Draw ();
//			// Termine la frame courante (en interne, échange les deux tampons de rendu)
//			window->display ();
//		}
//	};
//}