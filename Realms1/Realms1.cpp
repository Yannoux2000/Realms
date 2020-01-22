#include "Base/Logging/DebugConsoleLogger.h"
#include "Base/Logging/FileLogger.h"
#include "Base/Math/VoxelMath.h"

#include "Utility/FileIO/VoxFileParser.h"

#include <SFML/Window.hpp>

#include "Module/Input/InputManager.h"

#include "Module/Graphics/OpenGL.h"
#include "Module/Graphics/GLError.h"
#include "Module/Graphics/ShaderPrototypeLights.h"
#include "Module/Graphics/ScreenShaderPrototype1.h"
#include "Module/Graphics/ScreenShaderProgram.h"
#include "Module/Graphics/AspectRatio.h"
#include "Module/Graphics/GameRenderer.h"
#include "Module/Graphics/StaticMesh.h"
#include "Module/Graphics/Camera.h"
#include "Module/Graphics/ViewportFrameBuffer.h"
#include "Module/Graphics/MeshRegister.h"

#include "Module/World/BlockRegister.h"
#include "Module/World/Chunk.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <chrono>
#include <array>
#include <sstream>

extern "C" {
	#include "lua.h"
	#include "lauxlib.h"
	#include "lualib.h"
}

#include "RealmApplication.h"

//For debugging.
#include "SFML/Graphics.hpp" // sf::rectangle

#include "_MemLeakMonitor.h"


void lua_tests() {
	lua_State* L = luaL_newstate ();
	luaL_openlibs (L);

	luaL_dofile (L, "info.lua");

	lua_getglobal (L, "mod");
	if (lua_istable (L, -1)) {

		lua_pushstring (L, "a");
		lua_gettable (L, -2);

		if (lua_isnumber (L, -1)) {
			double r = lua_tonumber (L, -1);
			std::cout << "number " << r << std::endl;
		} else {
			std::cout << "number not found ?" << std::endl;
		}

		lua_pop (L, -1);
	} else {
		std::cout << "error while reading the lua file..." << std::endl;
	}

	lua_close (L);
}

using namespace rlms;

int main () {
	MemCheck ();

	RealmApplication app;
	RealmApplication::MemorySettings memory;
	RealmApplication::ApplicationSettings settings;

	settings.memory = memory;
	app.start (settings);

	try {
		app.run ();
		openGL_Error_Poll ();
	} catch (const std::exception& e) {
		std::cout << e.what () << std::endl;
	}

	app.stop ();
	return 0;
}

int old_main () {
	MemCheck ();

	std::shared_ptr<LoggerHandler> main_logger = std::make_shared<LoggerHandler>();
	main_logger->channelIn<FileLogger> ("Logs/main");
	main_logger->channelIn<DebugConsoleLogger> ();

	sf::ContextSettings settings;
	settings.depthBits = 24;
	settings.stencilBits = 8;
	settings.antialiasingLevel = 8;
	settings.majorVersion = 3;
	settings.minorVersion = 3;
	settings.attributeFlags = settings.Core;

	AspectRatio::Set (800, 600);

	main_logger->tag (LogTags::Info) << "( " << AspectRatio::Width () << ", " << AspectRatio::Height () << ")\n";

	// crée la fenêtre
	sf::Window window (sf::VideoMode (AspectRatio::Width (), AspectRatio::Height (), 32), "OmegaLul", sf::Style::Titlebar | sf::Style::Close, settings);

	// Initialize GLEW
	glewExperimental = GL_TRUE;
	glewInit ();
	
	//constant settings
	// Enable depth test
	// Accept fragment if it closer to the camera than the former one
	glEnable (GL_DEPTH_TEST);
	glDepthFunc (GL_LESS);

	window.setVerticalSyncEnabled (true);
	window.setMouseCursorGrabbed (false);
	window.setFramerateLimit (60);
	bool running = true;

	rlms::InputManager::Initialize (main_logger);
	{
		// chargement des ressources, initialisation des états OpenGL, ...
		rlms::InputManager::BindInput ("game::quit", sf::Keyboard::Escape);
		rlms::InputManager::EnableInputMap ("game");

		rlms::InputManager::BindEvent ("quit", sf::Event::Closed);
		rlms::InputManager::BindEvent ("resized", sf::Event::Resized);
		
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

	void* mem = malloc (4096);
	ProxyAllocator* app_alloc = new ProxyAllocator (mem, 4096);
	rlms::GraphicsManagerOld::Initialize (app_alloc, 4096, main_logger);
	rlms::GraphicsManagerOld::Load ();

	main_logger->tag (LogTags::Info) << "loading Shaders...\n";
	Shader vexShad = ShaderPrototype1::vertexShader ();
	Shader geoShad = ShaderPrototype1::geometryShader ();
	Shader fragShad = ShaderPrototype1::fragmentShader ();
	GameRenderer renderer;

	renderer.setVexShader (std::move (vexShad));
	renderer.setGeoShader (std::move (geoShad));
	renderer.setFrgShader (std::move (fragShad));

	try {
		renderer.start (main_logger);
		openGL_Error_Poll ();
		main_logger->tag (LogTags::Info) << "Shaders load successful.\n";
	} catch (GLException glError) {
		main_logger->tag (LogTags::Error) << glError.what () << "\n";
		running = false;
	}

	//Loading Palette
	std::array<int, 256> palette;
	try {
		VoxFileParser::LoadPalette ("Models/Default/Air.vox", palette);
		main_logger->tag (LogTags::Info) << "Palette import successful.\n";
	} catch (InvalidVox error) {
		main_logger->tag (LogTags::Error) << error.what () << "\n";
		running = false;
	}

	renderer.use ();
	renderer.setPalette (palette);

	try {
		openGL_Error_Poll ();
		main_logger->tag (LogTags::Info) << "Palette set successful.\n";
	} catch (GLException glError) {
		main_logger->tag (LogTags::Error) << glError.what () << "\n";
		running = false;
	}

	{
		rlms::GraphicsManagerOld::Register (0, "Models/Default/Workbench.vox");
		rlms::GraphicsManagerOld::Register (1, "Models/Default/Blocks/Dirt.vox");
		rlms::GraphicsManagerOld::Register (2, "Models/Default/Blocks/Wood.vox");
		rlms::GraphicsManagerOld::Register (3, "Models/Default/Perso_m1.vox");

		try {
			rlms::GraphicsManagerOld::LoadModels ();
		} catch (std::exception error) {
			main_logger->tag (LogTags::Error) << error.what () << "\n";
			running = false;
		}
	}
	rlms::GraphicsManagerOld::LoadRenderer ();
	rlms::GraphicsManagerOld::Load ();

	BlockRegister::Register (3, rlms::GraphicsManagerOld::GetMesh (1));
	BlockRegister::Register (4, rlms::GraphicsManagerOld::GetMesh (2));

	struct dirLight {
		glm::vec3 direction;
		glm::vec3 ambient;
		glm::vec3 diffuse;
		glm::vec3 specular;
	} sun;

	sun = dirLight {
		glm::vec3 (1.0f, -1.0f, -1.0f),
		glm::vec3 (0.6f, 0.6f, 0.6f),
		glm::vec3 (0.8f, 0.8f, 0.8f),
		glm::vec3 (0.0f, 0.0f, 0.0f)
	};

	renderer.use ();
		// directional light
	renderer.getIShader ()->setVec3 ("dirLight.direction", sun.direction);
	renderer.getIShader ()->setVec3 ("dirLight.ambient", sun.ambient);
	renderer.getIShader ()->setVec3 ("dirLight.diffuse", sun.diffuse);
	renderer.getIShader ()->setVec3 ("dirLight.specular", sun.specular);

	/*
	ScreenShaderProgram screenShader;

	Shader vexSShader = ScreenShaderPrototype1::vertexShader ();
	Shader frgSShader = ScreenShaderPrototype1::fragmentShader ();

	screenShader.attach (vexSShader);
	screenShader.attach (frgSShader);
	screenShader.load ();

	struct UITexture {
		glm::vec2 posA;
		glm::vec2 posB;
		glm::vec2 UVA;
		glm::vec2 UVB;

		UITexture () : posA (glm::vec2 (-1.0f, -1.0f)), posB (glm::vec2 (1.0f, 1.0f)), UVA (glm::vec2 (0.0f, 0.0f)), UVB (glm::vec2 (1.0f, 1.0f)) {}
		UITexture (glm::vec2 t_posA, glm::vec2 t_posB) : posA (t_posA), posB (t_posB), UVA (glm::vec2 (0.0f, 0.0f)), UVB (glm::vec2 (1.0f, 1.0f)) {}
		UITexture (glm::vec2 t_posA, glm::vec2 t_posB, glm::vec2 t_UVA, glm::vec2 t_UVB) : posA (t_posA), posB (t_posB), UVA (t_UVA), UVB (t_UVB) {}
	};

	UITexture gfb();

	float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
		// positions   // texCoords
		-0.9f,  0.9f,  0.0f, 1.0f,
		-0.9f, -0.9f,  0.0f, 0.0f,
		 0.9f, -0.9f,  1.0f, 0.0f,

		-0.9f,  0.9f,  0.0f, 1.0f,
		 0.9f, -0.9f,  1.0f, 0.0f,
		 0.9f,  0.9f,  1.0f, 1.0f
	};
	//screenShader.bind ();
	unsigned int quadVAO, quadVBO;
	glGenVertexArrays (1, &quadVAO);
	glGenBuffers (1, &quadVBO);
	
	glBindVertexArray (quadVAO);
	glBindBuffer (GL_ARRAY_BUFFER, quadVBO);
	glBufferData (GL_ARRAY_BUFFER, sizeof (quadVertices), &quadVertices, GL_STATIC_DRAW);

	ViewportFrameBuffer vfb;
	vfb.load ();

	screenShader.use ();
	screenShader.setInt ("screenTexture", 0);
	*/

	try {
		openGL_Error_Poll ();
		main_logger->tag (LogTags::Info) << "Ready for Main Loop.\n";
	} catch (GLException glError) {
		main_logger->tag (LogTags::Error) << glError.what () << "\n";
		running = false;
	}

	GameRenderer* currentRenderer = &renderer;

	glm::mat4 mesh;
	bool typing = false;
	uint8_t culling_mask = 0xFF;
	// la boucle principale
	while (running) {
		auto start = std::chrono::high_resolution_clock::now ();

		//rlms::InputManager
		{
			rlms::InputManager::Poll (window);

			if (rlms::InputManager::IsPressed ("game::quit") || rlms::InputManager::IsPressed ("quit")) {
				running = false;
			}

			if (rlms::InputManager::IsPressed ("quit")) {
				auto event = rlms::InputManager::GetEvent ("quit");
				if(event != nullptr) {
					running = false;
					//glViewport (0, 0, event->size.width, event->size.height);
				}
			}

			if (rlms::InputManager::IsDown ("cam::test")) {
				currentRenderer = &renderer;
			}

			if (rlms::InputManager::IsPressed ("cam::set")) {
				main_logger->tag (LogTags::Debug) << "Enter\n";
			}

			if (rlms::InputManager::IsPressed ("cam::reload")) {
				culling_mask = Camera::MainCamera->getBFCullingflags ();
			}

			if (rlms::InputManager::IsDown ("cam::forward")) {
				Camera::MainCamera->move (Camera::MainCamera->forward);
			}

			if (rlms::InputManager::IsDown ("cam::backward")) {
				Camera::MainCamera->move (- Camera::MainCamera->forward);
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

		auto input_end = std::chrono::high_resolution_clock::now ();
		std::chrono::duration<double, std::milli> fp_in_ms = input_end - start;

		//rlms::Updates
		{

		}

		auto update_end = std::chrono::high_resolution_clock::now ();
		std::chrono::duration<double, std::milli> fp_updates_ms = update_end - start;

		//Display
		//{
		//	// bind to framebuffer and draw scene as we normally would to color texture 
		//	//vfb.enable ();

		//	// Clear the screen to black
		//	glClearColor (0.0f, 0.0f, 0.0f, 1.0f);
		//	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//	currentRenderer->use ();
		//	currentRenderer->setCamera (Camera::MainCamera.get());
		//	currentRenderer->bind (rlms::GraphicsManager::GetMesh (3));
		//	//currentRenderer->bind (meshs->get(3));

		//	auto trans = glm::mat4 (1.0f);
		//	trans = glm::translate (trans, glm::vec3 (0, 0, 8));
		//	currentRenderer->setModelTrans (trans);
		//	currentRenderer->draw ();

		//	currentRenderer->unbind ();

		//	// now bind back to default framebuffer and draw a quad plane with the attached framebuffer color texture
		//	//ViewportFrameBuffer::Disable ();

		//	//// Clear the screen to black
		//	//glClearColor (0.0f, 0.0f, 0.0f, 1.0f);
		//	//glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//	//screenShader.use ();
		//	//glBindVertexArray (quadVAO);
		//	//screenShader.bind ();
		//	//screenShader.draw (vfb.getTexture());
		//	////glDrawArrays (GL_TRIANGLES, 0, 6);

		//	try {
		//		openGL_Error_Poll ();
		//		main_logger->tag (LogTags::Info) << "Rendering ok.\n";
		//	} catch (GLException glError) {
		//		main_logger->tag (LogTags::Error) << glError.what () << "\n";
		//		running = false;
		//	}

		//	// termine la frame courante (en interne, échange les deux tampons de rendu)
		//	window.display ();
		//}

		{
			rlms::GraphicsManagerOld::Draw ();
			// Termine la frame courante (en interne, échange les deux tampons de rendu)
			window.display ();
		}

		auto end = std::chrono::high_resolution_clock::now ();
		std::chrono::duration<double, std::milli> fp_ms = end - start;
		//main_logger->tag (LogTags::Debug) << "inputs dt : " << fp_in_ms.count () << "\t\tframe dt :" << fp_ms.count () << "\n";
 	}

	// libération des ressources...
	//vfb.unload ();

	rlms::GraphicsManagerOld::Unload ();
	rlms::GraphicsManagerOld::Terminate ();

	delete app_alloc;
	free (mem);
	
	rlms::InputManager::Terminate();

	return 0;
}
