#pragma once
#include <GLFW/glfw3.h>

#include "Base/Logging/DebugConsoleLogger.h"
#include "Base/Logging/FileLogger.h"

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

		std::atomic_bool stop_signal;

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
					rlms::GraphicsManager::Initialize (app_alloc.get (), stgs.memory.mesh_size, window, logger);
					rlms::GraphicsManager::Load ();

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
					rlms::InputManager::Initialize ();
					rlms::InputManager::SetCallbacks (window);

					//rlms::InputManager::BindEvent ("close", sf::Event::Closed);
					//rlms::InputManager::BindEvent ("resized", sf::Event::Resized);

					rlms::InputManager::BindKey ("game::quit", glfwGetKeyScancode (GLFW_KEY_ESCAPE));
					rlms::InputManager::EnableInputMap ("game");

					//rlms::InputManager::BindKey ("cam::reset", glfwGetKeyScancode (GLFW_KEY_SPACE));
					//rlms::InputManager::BindKey ("cam::test", glfwGetKeyScancode (GLFW_KEY_E));
					//rlms::InputManager::BindKey ("cam::reload", glfwGetKeyScancode (GLFW_KEY_R));
					//rlms::InputManager::BindKey ("cam::zoom", glfwGetKeyScancode (GLFW_KEY_PAGE_UP));
					//
					//rlms::InputManager::BindSlide ("cam::look", sf::Mouse::Right);
					//
					//rlms::InputManager::BindKey ("cam::forward", glfwGetKeyScancode (GLFW_KEY_Z));
					//rlms::InputManager::BindKey ("cam::left", glfwGetKeyScancode (GLFW_KEY_Q));
					//rlms::InputManager::BindKey ("cam::backward", glfwGetKeyScancode (GLFW_KEY_S));
					//rlms::InputManager::BindKey ("cam::right", glfwGetKeyScancode (GLFW_KEY_D));
					//rlms::InputManager::EnableInputMap ("cam");
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
					//Graphical Loop
					JobSystem::Register (PeriodicJob<std::ratio<1, 60>> (
						[this]() {
							if(statusGraphics != Idle) {
								rlms::GraphicsManager::Draw ();
							}
						}
					, stop_signal));

					//Input Loop
					JobSystem::Register (PeriodicJob<std::ratio<1, 500>> (
						[this]() {
							//logger->tag (LogTags::Dev) << "(Input)\n";
							rlms::InputManager::Poll (window);
							if (glfwWindowShouldClose (window)) {
								logger->tag (LogTags::Info) << "stop_signal has been received !\n";
								stop_signal.store (true);
							}
						}
					, stop_signal));

					//Update Loop
					JobSystem::Register (PeriodicJob<std::ratio<1, 30>> (
						[this]() {
							if(statusUpdate != Idle) {
								if (rlms::InputManager::IsPressed ("game::quit") || rlms::InputManager::IsPressed ("close")) {
									running = false;
								}
					
								//if (rlms::InputManager::IsPressed ("cam::set")) {
								//	logger->tag (LogTags::Debug) << "Enter\n";
								//}
								//if (rlms::InputManager::IsPressed ("cam::reset")) {
								//	Camera::MainCamera->reset ();
								//}
								//if (rlms::InputManager::IsDown ("cam::look")) {
								//	glm::ivec2 delta;
								//	auto vec = InputManager::GetDeltaPos ("cam::look");
								//	delta.x = vec[0];
								//	delta.y = vec[1];
								//	Camera::MainCamera->look (delta);
								//}
								//
								//if (rlms::InputManager::IsDown ("cam::forward")) {
								//	Camera::MainCamera->move (Camera::MainCamera->forward);
								//}
								//if (rlms::InputManager::IsDown ("cam::backward")) {
								//	Camera::MainCamera->move (-Camera::MainCamera->forward);
								//}
								//if (rlms::InputManager::IsDown ("cam::left")) {
								//	Camera::MainCamera->move (-Camera::MainCamera->right);
								//}
								//if (rlms::InputManager::IsDown ("cam::right")) {
								//	Camera::MainCamera->move (Camera::MainCamera->right);
								//}
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
					rlms::GraphicsManager::Unload ();
					rlms::GraphicsManager::Terminate ();
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