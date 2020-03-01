#include "SystemManager.h"

using namespace rlms;

int SystemManager::n_errors;
std::unique_ptr<SystemManagerImpl> SystemManager::instance;

std::shared_ptr<LoggerHandler> SystemManager::GetLogger () {
	return instance->getLogger();
}

bool SystemManager::Initialize (Allocator* const& alloc, size_t system_pool_size, std::shared_ptr<Logger> funnel) {
	instance = std::make_unique<SystemManagerImpl> ();
	return instance->start (alloc, system_pool_size, funnel);
}

void SystemManager::Terminate () {
	instance->stop ();
	instance.reset ();
}

SystemManagerImpl::SystemManagerImpl () : _systems () {}
SystemManagerImpl::~SystemManagerImpl () {}

bool SystemManagerImpl::start (Allocator* const& alloc, size_t system_pool_size, std::shared_ptr<Logger> funnel) {
	startLogger (funnel);
	logger->tag (LogTags::None) << "Initializing !" << '\n';

	m_object_Allocator = std::unique_ptr<alloc_type> (new alloc_type (system_pool_size, alloc->allocate (system_pool_size)));

	SystemManager::n_errors = 0;
	logger->tag (LogTags::None) << "Initialized correctly !" << '\n';
	return true;
}

void SystemManagerImpl::stop () {
	logger->tag (LogTags::None) << "Stopping !" << '\n';

	for (auto it = _systems.begin (); it != _systems.end (); it++) {
		it->second->~ISystem ();
		m_object_Allocator->deallocate (it->second);
	}

	logger->tag (LogTags::None) << "Stopped correctly !" << '\n';
}

//void SystemManagerImpl::update (GAME_TICK_TYPE dt) {
//	logger->tag (LogTags::Debug) << "Updating by " << dt << "ticks." << '\n';
//
//	for (auto it = _systems.begin (); it != _systems.end (); it++) {
//		it->second->update (dt);
//	}
//
//	logger->tag (LogTags::Debug) << "Updating done." << '\n';
//}