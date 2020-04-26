#pragma once
#include <GLFW/glfw3.h>

#include "Base/Logging/DebugConsoleLogger.h"
#include "Base/Logging/FileLogger.h"

#include "Base/Statistics.h"

#include "Base/Allocators/ProxyAllocator.h"
//#include "Base/Allocators/StackAllocator.h"
#include "Base/Allocators/MasqueradeAllocator.h"

//#include "Utility/FileIO/VoxFileParser.h"
#include "Utility/MultiThreading/JobSystem.h"
#include "Utility/MultiThreading/PeriodicJob.h"

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

		enum SystemsStatus {
			Undefined = 0,
			Initializing,
			Ready,
			Running,
			Idle,
			Stopped,
			Terminated,
			Failed
		};

		std::atomic<SystemsStatus> statusInput;
		std::atomic<SystemsStatus> statusUpdate;
		std::atomic<SystemsStatus> statusGraphics;
		std::atomic<SystemsStatus> statusWindow;

		Statistics statsInput;
		Statistics statsUpdate;
		Statistics statsGraphics;

		std::atomic_bool stop_signal;

		AssignAddress game_quit;

	public:
		struct MemorySettings {
			size_t total_size = 16384;
			size_t mesh_size = 4096;
			size_t ecs_size = 8192;
			size_t entity_size = 4096;
			size_t component_size = 4096;
		};

		struct ApplicationSettings {
			MemorySettings memory;
			unsigned int aspect_width = 800;
			unsigned int aspect_height = 600;
			bool fullscreen = false;
		};

		void start (ApplicationSettings& stgs) {
			ApplicationTime::Initialize ();
			startLogger (nullptr, true);

			logger->tag (LogTags::None) << "Initializing Application.\n";
			stop_signal.store (false);

			//read all graphics and controls and mods options
			statsInput.reset ();
			statsUpdate.reset ();
			statsGraphics.reset ();

			//Memory initialisation
			void* mem = malloc (stgs.memory.total_size);
			app_alloc = std::unique_ptr<_allocType> (new _allocType (stgs.memory.total_size, mem));

			JobSystem::Initialize (logger);

			//Window initialisation
			JobSystem::Register (ConditionnalJob (
				[this]() -> bool {
					return statusWindow == Initializing;
				}
				, [this, stgs]() {
					logger->tag (LogTags::Debug) << "Initializing Window.\n";

					AspectRatio::Set (stgs.aspect_width, stgs.aspect_height);
					logger->tag (LogTags::Dev) << "( " << AspectRatio::Width () << ", " << AspectRatio::Height () << ")\n";

					// crée la fenêtre
					if (!glfwInit ()) {
						statusWindow.store (Failed);
					}

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

					statusWindow.store (Ready);
				}
			, 1));

			//Core initialisation
			JobSystem::Register (ConditionnalJob (
				[this]() -> bool {
					return statusWindow == Initializing;
				}
				, [this, stgs]() {
					EntityManager::Initialize (app_alloc.get (), stgs.memory.entity_size, logger);
					ComponentManager::Initialize (app_alloc.get (), stgs.memory.component_size, logger);
					statusUpdate.store (Ready);
				}
			, 1));

			//Graphics initialisation
			JobSystem::Register (ConditionnalJob (
				[this]() -> bool {
					return statusWindow == Ready
						&& statusGraphics == Initializing;
				}
				, [this, stgs]() {
					GraphicsManager::Initialize (app_alloc.get (), stgs.memory.mesh_size, window, logger);
					GraphicsManager::Load ();

					statusGraphics.store (Ready);
				}
			, 2));

			//Input initialisation
			JobSystem::Register (ConditionnalJob (
				[this]() -> bool {
					return statusWindow == Ready
						&& statusInput == Initializing;
				}
				, [this, stgs]() {
					InputManager::Initialize ();
					InputManager::SetCallbacks (window);

					//InputManager::BindEvent ("close", sf::Event::Closed);
					//InputManager::BindEvent ("resized", sf::Event::Resized);

					InputManager::BindKey ("game::quit", glfwGetKeyScancode (GLFW_KEY_ESCAPE));
					InputManager::EnableInputMap ("game");
					game_quit = InputManager::GetInput ("game::quit");

					//InputManager::BindKey ("cam::reset", glfwGetKeyScancode (GLFW_KEY_SPACE));
					//InputManager::BindKey ("cam::test", glfwGetKeyScancode (GLFW_KEY_E));
					//InputManager::BindKey ("cam::reload", glfwGetKeyScancode (GLFW_KEY_R));
					//InputManager::BindKey ("cam::zoom", glfwGetKeyScancode (GLFW_KEY_PAGE_UP));
					//
					//InputManager::BindSlide ("cam::look", sf::Mouse::Right);
					//
					//InputManager::BindKey ("cam::forward", glfwGetKeyScancode (GLFW_KEY_Z));
					//InputManager::BindKey ("cam::left", glfwGetKeyScancode (GLFW_KEY_Q));
					//InputManager::BindKey ("cam::backward", glfwGetKeyScancode (GLFW_KEY_S));
					//InputManager::BindKey ("cam::right", glfwGetKeyScancode (GLFW_KEY_D));
					//InputManager::EnableInputMap ("cam");
					statusInput.store (Ready);
				}
			, 2));

			//Periodic Jobs initialisation
			JobSystem::Register (ConditionnalJob (
				[this]() -> bool {
				return statusInput == SystemsStatus::Ready
					&& statusUpdate == SystemsStatus::Ready
					&& statusWindow == SystemsStatus::Ready
					&& statusGraphics == SystemsStatus::Ready;
				},
				[this]() {
					logger->tag (LogTags::Info) << "Application is Ready, Initializing.\n";
					//Graphics Loop
					JobSystem::Register (PeriodicJob<std::ratio<1, 60>> (
						[this]() {
							if(statusGraphics != Idle) {
								auto start = std::chrono::high_resolution_clock::now ();

								GraphicsManager::Draw ();

								auto end = std::chrono::high_resolution_clock::now ();
								std::chrono::duration<double, std::micro> ms = end - start;

								statsGraphics.add (ms.count ());

								std::ostringstream oss ("");
								oss << "Graphical:\t(" << statsGraphics.min () << " ms" <<
									",  " << statsGraphics.mean () << " ms" <<
									",  " << statsGraphics.max () << " ms" <<
									",  " << statsGraphics.sD () << " ms)";
								logger->log (oss.str (), LogTags::Dev);
							}
						}
					, stop_signal));

					//Input Loop
					JobSystem::Register (PeriodicJob<std::ratio<1, 120>> (
						[this]() {
							auto start = std::chrono::high_resolution_clock::now ();
							//logger->tag (LogTags::Dev) << "(Input)\n";
							InputManager::Poll (window);
							if (glfwWindowShouldClose (window)) {
								logger->tag (LogTags::Info) << "stop_signal has been received !\n";
								stop_signal.store (true);
							}
							auto end = std::chrono::high_resolution_clock::now ();
							std::chrono::duration<double, std::micro> ms = end - start;

							statsInput.add (ms.count ());

							std::ostringstream oss ("");
							oss << "Input:    \t(" << statsInput.min () << " ms" <<
								",  " << statsInput.mean () << " ms" <<
								",  " << statsInput.max () << " ms" <<
								",  " << statsInput.sD () << " ms)";
							logger->log (oss.str (), LogTags::Dev);
						}
					, stop_signal));

					//Update Loop
					JobSystem::Register (PeriodicJob<std::ratio<1, 120>> (
						[this]() {
							if(statusUpdate != Idle) {
								auto start = std::chrono::high_resolution_clock::now ();

								if (InputManager::IsPressed (game_quit) /*|| InputManager::IsPressed (InputManager::GetInput ("close"))*/) {
									running = false;
								}
					
								//if (InputManager::IsPressed ("cam::set")) {
								//	logger->tag (LogTags::Debug) << "Enter\n";
								//}
								//if (InputManager::IsPressed ("cam::reset")) {
								//	Camera::MainCamera->reset ();
								//}
								//if (InputManager::IsDown ("cam::look")) {
								//	glm::ivec2 delta;
								//	auto vec = InputManager::GetDeltaPos ("cam::look");
								//	delta.x = vec[0];
								//	delta.y = vec[1];
								//	Camera::MainCamera->look (delta);
								//}
								//
								//if (InputManager::IsDown ("cam::forward")) {
								//	Camera::MainCamera->move (Camera::MainCamera->forward);
								//}
								//if (InputManager::IsDown ("cam::backward")) {
								//	Camera::MainCamera->move (-Camera::MainCamera->forward);
								//}
								//if (InputManager::IsDown ("cam::left")) {
								//	Camera::MainCamera->move (-Camera::MainCamera->right);
								//}
								//if (InputManager::IsDown ("cam::right")) {
								//	Camera::MainCamera->move (Camera::MainCamera->right);
								//}
								
								auto end = std::chrono::high_resolution_clock::now ();
								std::chrono::duration<double, std::micro> ms = end - start;

								statsUpdate.add (ms.count ());

								std::ostringstream oss ("");
								oss << "Update:  \t(" << statsUpdate.min() << " ms" <<
									",  " << statsUpdate.mean () << " ms" <<
									",  " << statsUpdate.max () << " ms" <<
									",  " << statsUpdate.sD () << " ms)";
								logger->log (oss.str (), LogTags::Dev);
							}
						}
					, stop_signal));

				}
				, 3));

			statusInput.store (Initializing);
			statusUpdate.store (Initializing);
			statusWindow.store (Initializing);
			statusGraphics.store (Initializing);
			JobSystem::WakeUp ();
		}

		void run () {
			logger->tag (LogTags::Info) << "Application is Running.\n";
			while (!stop_signal) {
				JobSystem::MainWorker ();
			}
		}

		void stop () {
			logger->tag (LogTags::None) << "Terminating Application.\n";
			JobSystem::Register (Job (
				[this]() {
					statusInput.store (Stopped);
					statusGraphics.store (Stopped);
					statusUpdate.store (Stopped);
					statusWindow.store (Stopped);
				}
				, JOB_MIN_PRIORITY));

			while (JobSystem::IsBusy ());

			//Terminate Input
			JobSystem::Register (ConditionnalJob (
				[this]() -> bool {
					return stop_signal && statusInput == Stopped;
				}
				, [this]() {
					InputManager::Terminate ();
					statusInput.store (Terminated);
				}
				, JOB_MIN_PRIORITY - 1));

			//Terminate Graphics
			JobSystem::Register (ConditionnalJob (
				[this]() -> bool {
					return stop_signal && statusGraphics == Stopped;
				}
				, [this]() {
					GraphicsManager::Unload ();
					GraphicsManager::Terminate ();
					statusGraphics.store (Terminated);
				}
				, JOB_MIN_PRIORITY - 1));

			//Terminate Update
			JobSystem::Register (ConditionnalJob (
				[this]() -> bool {
					return stop_signal && statusUpdate == Stopped;
				}
				, [this]() {
					EntityManager::Terminate ();
					ComponentManager::Terminate ();
					statusUpdate.store (Terminated);
				}
				, JOB_MIN_PRIORITY - 0));

			//Terminate Window
			JobSystem::Register (ConditionnalJob (
				[this]() -> bool {
					return stop_signal && statusWindow == Stopped;
				}
				, [this]() {
					glfwDestroyWindow (window);
					glfwTerminate ();
					statusWindow.store (Terminated);
				}
				, JOB_MIN_PRIORITY - 0));

			JobSystem::WakeUp ();
			JobSystem::Terminate ();

			void* mem = app_alloc->getStart ();
			app_alloc.reset ();
			free (mem);

			logger->tag (LogTags::None) << "Application has Terminated.\n";
		}

	private:
		GLFWwindow* window;

		bool running = false;
		std::unique_ptr<_allocType> app_alloc;

		std::string getLogName () override {
			return "Realms";
		};
	};
}