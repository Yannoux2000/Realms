#include "EventManager.h"

using namespace rlms;

int EventManager::n_errors;
std::unique_ptr<EventManagerImpl> EventManager::instance;

std::shared_ptr<LoggerHandler> EventManager::GetLogger () {
	return instance->getLogger ();
}

bool EventManager::Initialize (Allocator* const& alloc, size_t event_pool_size, std::shared_ptr<Logger> funnel) {
	instance = std::make_unique<EventManagerImpl> ();
	return instance->start (alloc, event_pool_size, funnel);
}

void EventManager::Terminate () {
	instance->stop ();
	instance.reset ();
}

void EventManager::ClearEvents () {
	instance->clearEvents ();
}

EventManagerImpl::EventManagerImpl () : _events() {}
EventManagerImpl::~EventManagerImpl () {}

bool EventManagerImpl::start (Allocator* const& alloc, size_t event_pool_size, std::shared_ptr<Logger> funnel) {
	startLogger (funnel);
	logger->tag (LogTags::Debug) << "Initializing !" << '\n';

	_event_Allocator = std::unique_ptr<LinearAllocator> (new LinearAllocator (alloc->allocate (event_pool_size), event_pool_size));

	EventManager::n_errors = 0;
	logger->tag (LogTags::None) << "Initialized correctly !" << '\n';
	return true;
}

void EventManagerImpl::stop () {
	logger->tag (LogTags::Debug) << "Stopping !" << '\n';
	clearEvents ();
	logger->tag (LogTags::None) << "Stopped correctly !" << '\n';
}

void EventManagerImpl::clearEvents () {
	logger->tag (LogTags::Debug) << "Clearing events." << '\n';
	for (auto it = _events.begin (); it != _events.end (); it++) {
		it->second->~IEvent ();
	}

	_events.clear ();
	_event_Allocator->clear ();
	logger->tag (LogTags::Debug) << "Events cleared." << '\n';
}
