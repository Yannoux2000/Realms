#include "WorldManager.h"
#include "../../Base/Allocators/FreeListAllocator.h"

#include "BlockPrototype.h"
#include <map>

using namespace rlms;

class rlms::WorldManagerImpl : public ILogged {
private:
	friend class WorldManager;

	std::string getLogName () override {
		return "WorldManager";
	};

	std::map<BLOCK_TYPE_ID, BlockPrototype> m_register;
	std::unique_ptr<FreeListAllocator> m_chunk_allocator;

	bool start (Allocator* const& alloc, size_t chunk_pool_size, std::shared_ptr<Logger> funnel = nullptr);
	void stop ();

};

bool rlms::WorldManagerImpl::start (Allocator* const& alloc, size_t chunk_pool_size, std::shared_ptr<Logger> funnel) {
	startLogger (funnel);
	logger->tag (LogTags::None) << "Initializing !" << '\n';

	m_chunk_allocator = std::unique_ptr<FreeListAllocator> (new FreeListAllocator (alloc->allocate (chunk_pool_size), chunk_pool_size));

	WorldManager::n_errors = 0;
	logger->tag (LogTags::None) << "Initialized correctly !" << '\n';
	return true;
}

void rlms::WorldManagerImpl::stop () {
	logger->tag (LogTags::None) << "Stopping !" << '\n';

	logger->tag (LogTags::None) << "Stopped correctly !" << '\n';
}

int WorldManager::n_errors;
std::unique_ptr<WorldManagerImpl> WorldManager::instance;

std::shared_ptr<rlms::LoggerHandler> rlms::WorldManager::GetLogger () {
	return instance->getLogger ();
}

bool rlms::WorldManager::Initialize (Allocator* const& alloc, size_t chunk_pool_size, std::shared_ptr<Logger> funnel) {
	instance = std::make_unique<WorldManagerImpl> ();
	return instance->start (alloc, chunk_pool_size, funnel);
}

void rlms::WorldManager::Terminate () {
	instance->stop ();
	instance.reset ();
}
