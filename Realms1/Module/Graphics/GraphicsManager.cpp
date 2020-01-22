#include "GraphicsManager.h"

#include "../../Base/Logging/ILogged.h"
#include "../../Base/Allocators/Allocator.h"

#include "../../Utility/FileIO/VoxFileParser.h"

#include "GameRenderer.h"
#include "MeshRegister.h"
#include "ShaderPrototypeLights.h"
#include "StaticMesh.h"

#include "../World/Chunk.h"

using namespace rlms;

class rlms::GraphicsManagerImpl : public ILogged {
private:
	friend class GraphicsManager;

	std::unique_ptr<GameRenderer> renderer;
	std::unique_ptr<MeshRegister> meshRegister;

	Chunk chunk;

	std::string getLogName () override {
		return "GraphicsManager";
	};

	void start (Allocator* const& alloc, size_t mesh_pool_size, std::shared_ptr<Logger> funnel = nullptr);
	void stop ();

	MeshRegister* getRegister ();
	void Register (IMODEL_TYPE_ID const& type_id, std::string&& filename, std::string&& meshAlias);
	IMesh* getMesh (IMODEL_TYPE_ID const& type_id);

	void load ();
	void loadModels ();
	void loadRenderer ();
	void draw ();
	void unload ();

	struct dirLight {
		glm::vec3 direction;
		glm::vec3 ambient;
		glm::vec3 diffuse;
		glm::vec3 specular;
	} sun;
};

void rlms::GraphicsManagerImpl::start (Allocator* const& alloc, size_t mesh_pool_size, std::shared_ptr<Logger> funnel) {
	startLogger (funnel);
	logger->tag (LogTags::None) << "Initializing !" << '\n';
	//ok
	meshRegister = std::make_unique<MeshRegister> ();
	meshRegister->start (alloc, mesh_pool_size, logger);

	renderer = std::make_unique<GameRenderer> ();

	logger->tag (LogTags::None) << "Initialized correctly !" << '\n';
}

void rlms::GraphicsManagerImpl::stop () {
	logger->tag (LogTags::None) << "Stopping" << '\n';
	
	renderer.reset ();

	meshRegister->stop ();
	meshRegister.reset ();

	logger->tag (LogTags::None) << "Stopped correctly !" << '\n';
}

MeshRegister* rlms::GraphicsManagerImpl::getRegister () {
	return meshRegister.get ();
}

void rlms::GraphicsManagerImpl::Register (IMODEL_TYPE_ID const& type_id, std::string&& filename, std::string&& meshAlias) {
	meshRegister->Register<StaticMesh> (type_id, std::move(filename));

	if (!meshAlias.empty ()) {
		meshRegister->RegisterAlias (type_id, std::move(meshAlias));
	}
}

IMesh* rlms::GraphicsManagerImpl::getMesh (IMODEL_TYPE_ID const& type_id) {
	return meshRegister->get (type_id);
}

void rlms::GraphicsManagerImpl::load () {
	chunk.origin = glm::vec3 (-8 * 8);

	chunk.create_sample ();
	chunk.optimize ();
	Camera::CreateMainCamera ();
}

void rlms::GraphicsManagerImpl::loadModels () {
	meshRegister->imports ();
	meshRegister->optimises ();
	meshRegister->loads ();

	openGL_Error_Poll ();
	logger->tag (LogTags::Info) << "Meshs import and load successful.\n";
}

void rlms::GraphicsManagerImpl::loadRenderer () {
	Shader vexShad = ShaderPrototype1::vertexShader ();
	Shader geoShad = ShaderPrototype1::geometryShader ();
	Shader fragShad = ShaderPrototype1::fragmentShader ();
	renderer = std::make_unique<GameRenderer> ();

	renderer->setVexShader (std::move (vexShad));
	renderer->setGeoShader (std::move (geoShad));
	renderer->setFrgShader (std::move (fragShad));

	renderer->start (logger);
	openGL_Error_Poll ();
	logger->tag (LogTags::Info) << "Shaders load successful.\n";

	//Loading Palette
	std::array<int, 256> palette;
	VoxFileParser::LoadPalette ("Models/Default/Air.vox", palette);
	logger->tag (LogTags::Info) << "Palette import successful.\n";

	sun = dirLight{
		glm::vec3 (1.0f, -1.0f, -1.0f),
		glm::vec3 (0.6f, 0.6f, 0.6f),
		glm::vec3 (0.8f, 0.8f, 0.8f),
		glm::vec3 (0.0f, 0.0f, 0.0f)
	};

	renderer->use ();
		// directional light
	renderer->getIShader ()->setVec3 ("dirLight.direction", sun.direction);
	renderer->getIShader ()->setVec3 ("dirLight.ambient", sun.ambient);
	renderer->getIShader ()->setVec3 ("dirLight.diffuse", sun.diffuse);
	renderer->getIShader ()->setVec3 ("dirLight.specular", sun.specular);
}

void rlms::GraphicsManagerImpl::draw () {
	glClearColor (0.0f, 0.0f, 0.0f, 1.0f);
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	renderer->use ();
	renderer->setCamera (Camera::MainCamera.get ());
	renderer->bind (meshRegister->get(3));

	auto trans = glm::mat4 (1.0f);
	trans = glm::translate (trans, glm::vec3 (0, 0, 8));
	renderer->setModelTrans (trans);
	renderer->draw (true);

	chunk.render (renderer.get());

	renderer->unbind ();
	/*
	// bind to framebuffer and draw scene as we normally would to color texture 
	//vfb.enable ();

	// Clear the screen to black
	glClearColor (0.0f, 0.0f, 0.0f, 1.0f);
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	renderer->use ();
	renderer->setCamera (Camera::MainCamera.get ());
	renderer->bind (meshRegister->get ("perso_m1"));

	trans = glm::translate (trans, glm::vec3 (0, 0, 8));
	renderer->setModelTrans (trans);
	renderer->draw ();

	chunk.render (renderer.get());

	renderer->unbind ();

	// now bind back to default framebuffer and draw a quad plane with the attached framebuffer color texture
	//ViewportFrameBuffer::Disable ();

	//// Clear the screen to black
	//glClearColor (0.0f, 0.0f, 0.0f, 1.0f);
	//glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//screenShader.use ();
	//glBindVertexArray (quadVAO);
	//screenShader.bind ();
	//screenShader.draw (vfb.getTexture());
	////glDrawArrays (GL_TRIANGLES, 0, 6);

	//try {
	//	openGL_Error_Poll ();
	//	//main_logger->tag (LogTags::Info) << "Rendering ok.\n";
	//} catch (GLException glError) {
	//	main_logger->tag (LogTags::Error) << glError.what () << "\n";
	//	running = false;
	//}
	*/
}

void rlms::GraphicsManagerImpl::unload () {
	renderer.reset ();
}

std::unique_ptr<GraphicsManagerImpl> rlms::GraphicsManager::instance;

std::shared_ptr<LoggerHandler> rlms::GraphicsManager::GetLogger () {
	return instance->getLogger ();
}

void rlms::GraphicsManager::Initialize (Allocator* const& alloc, size_t mesh_pool_size, std::shared_ptr<Logger> funnel) {
	instance = std::make_unique<GraphicsManagerImpl> ();
	instance->start (alloc, mesh_pool_size, funnel);
}

void rlms::GraphicsManager::Terminate () {
	instance->stop ();
	instance.reset ();
}

MeshRegister* rlms::GraphicsManager::GetRegister () {
	return instance->getRegister ();
}

void rlms::GraphicsManager::Register (IMODEL_TYPE_ID const& type_id, std::string&& filename, std::string&& meshAlias) {
	instance->Register (type_id, std::move(filename), std::move(meshAlias));
}

IMesh* rlms::GraphicsManager::GetMesh (IMODEL_TYPE_ID const& type_id) {
	return instance->getMesh(type_id);
}

void rlms::GraphicsManager::Load () {
	instance->load ();
}

void rlms::GraphicsManager::LoadModels () {
	instance->loadModels ();
}

void rlms::GraphicsManager::LoadRenderer () {
	instance->loadRenderer ();
}

void rlms::GraphicsManager::Draw () {
	instance->draw ();
}

void rlms::GraphicsManager::Unload () {
	instance->unload ();
}
