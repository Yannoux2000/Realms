#include "GraphicsManager.h"

#include "../../Base/Logging/ILogged.h"
#include "../../Base/Allocators/Allocator.h"

#include "../../Utility/FileIO/VoxFileParser.h"

#include "MeshRegister.h"
#include "StaticMesh.h"
#include "AspectRatio.h"

#include "IContext.h"

#include "Vulkan/VulkanContext.h"
#include "Vulkan/VulkanComputeRenderer.h"
#include "Vulkan/VulkanExempleRenderer.h"

#include "OpenGL/OpenGLContext.h"
#include "OpenGL/OpenGLRatalizationRenderer.h"

using namespace rlms;

class rlms::GraphicsManagerImpl : public ILogged {
private:
	friend class GraphicsManager;

	IContext* context;

	std::string getLogName () override {
		return "GraphicsManager";
	};

	void start (Allocator* const& alloc, size_t mesh_pool_size, GLFWwindow*& window, std::shared_ptr<Logger> funnel = nullptr);
	void stop ();

	MeshRegister* getRegister ();
	void Register (IMESH_TYPE_ID const& type_id, std::string&& filename, std::string&& meshAlias);
	IMesh* getMesh (IMESH_TYPE_ID const& type_id);

	void load ();
	void draw ();
	void unload ();
};

void rlms::GraphicsManagerImpl::start (Allocator* const& alloc, size_t mesh_pool_size, GLFWwindow*& window, std::shared_ptr<Logger> funnel) {
	startLogger (funnel);
	logger->tag (LogTags::None) << "Initializing !" << '\n';

	context = new VulkanContext ();
	context->setRenderer (new VulkanExempleRenderer ());
	context->start (window);

	logger->tag (LogTags::None) << "Initialized correctly !" << '\n';
}

void rlms::GraphicsManagerImpl::stop () {
	logger->tag (LogTags::None) << "Terminating" << '\n';

	context->stop ();
	delete context;

	logger->tag(LogTags::None) << "Terminated correctly !" << '\n';
}

void rlms::GraphicsManagerImpl::load () {}

void rlms::GraphicsManagerImpl::draw () {
	context->draw ();
}

void rlms::GraphicsManagerImpl::unload () {}

std::unique_ptr<GraphicsManagerImpl> rlms::GraphicsManager::instance;

std::shared_ptr<LoggerHandler> rlms::GraphicsManager::GetLogger () {
	return instance->getLogger ();
}

void rlms::GraphicsManager::Initialize (Allocator* const& alloc, size_t mesh_pool_size, GLFWwindow*& window, std::shared_ptr<Logger> funnel) {
	instance = std::make_unique<GraphicsManagerImpl> ();
	instance->start (alloc, mesh_pool_size, window, funnel);
}

void rlms::GraphicsManager::Terminate () {
	instance->stop ();
	instance.reset ();
}

void rlms::GraphicsManager::Load () {
	instance->load ();
}

void rlms::GraphicsManager::Draw () {
	instance->draw ();
}

void rlms::GraphicsManager::Unload () {
	instance->unload ();
}
