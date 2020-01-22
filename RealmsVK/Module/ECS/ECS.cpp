#include "RealmsCore/ECS.h"

using namespace rlms;

int ECSCore::n_error;
unsigned long long ECSCore::game_mem_alloc_size;
unsigned long long ECSCore::entity_mem_alloc_size;
unsigned long long ECSCore::component_mem_alloc_size;
unsigned long long ECSCore::system_mem_alloc_size;
unsigned long long ECSCore::event_mem_alloc_size;
std::unique_ptr<ECSCore> ECSCore::instance;

//GAME_TICK_TYPE ECSCore::_current_tick;
double ECSCore::m_atomic_tick_time;

std::shared_ptr<LoggerHandler> ECSCore::GetLogger () {
	return instance->getLogger ();
}

bool ECSCore::Initialize (std::shared_ptr<Logger> funnel) {
	instance = std::make_unique<ECSCore> ();
	return instance->start(funnel);
}

void ECSCore::PreUpdate (GAME_TICK_TYPE current_tick) {
	instance->preUpdate (current_tick);
}

void ECSCore::Update (GAME_TICK_TYPE current_tick) {
	instance->update (current_tick);
}

void ECSCore::PostUpdate (GAME_TICK_TYPE current_tick) {
	instance->postUpdate (current_tick);
}

void ECSCore::Terminate () {
	instance->terminate();
}

ECSCore::ECSCore () : _loader_system() {}

ECSCore::~ECSCore () {}

bool ECSCore::start (std::shared_ptr<Logger> funnel) {
	startLogger (funnel);

	//invalid amount of memory
	if (game_mem_alloc_size <= 0) {
		logger->tag (LogTags::Error) << "Please be more reasonable with the memory amount : 1024M is good enouth." << '\n';
		return false;
	}

	//invalid amount of memory
	if (game_mem_alloc_size < (entity_mem_alloc_size + component_mem_alloc_size + system_mem_alloc_size + event_mem_alloc_size)) {
		logger->tag (LogTags::Error) << "Amount of memory requested does not add up." << '\n';
		return false;
	}

	//Valid
	void* alloc = malloc (game_mem_alloc_size);
	m_global_allocator = std::make_unique<FreeListAllocator,void*, size_t> (std::move(alloc), std::move(game_mem_alloc_size));


	EntityManager::Initialize (m_global_allocator.get (), entity_mem_alloc_size, logger);
	ComponentManager::Initialize (m_global_allocator.get (), component_mem_alloc_size, logger);
	SystemManager::Initialize (m_global_allocator.get (), system_mem_alloc_size, logger);
	EventManager::Initialize (m_global_allocator.get (), event_mem_alloc_size, logger);
	
	return true;
}

void ECSCore::setLoaderSystem (IGameLoaderSystem* loader) {
	_loader_system = loader;
}

void ECSCore::load () {
	_loader_system->load ();
}

void ECSCore::save () {}

void ECSCore::preUpdate (GAME_TICK_TYPE _current_tick) {
	logger->tag (LogTags::Debug) << "[" << _current_tick << "] Tick Update started ." << '\n';
	SystemManager::PreUpdate (_current_tick);
}

void ECSCore::update (GAME_TICK_TYPE _current_tick) {
	logger->tag (LogTags::Debug) << "[" << _current_tick << "] Tick Update started ." << '\n';
	SystemManager::Update (_current_tick);
}

void ECSCore::postUpdate (GAME_TICK_TYPE _current_tick) {
	logger->tag (LogTags::Debug) << "[" << _current_tick << "] Tick Update started ." << '\n';
	SystemManager::PostUpdate (_current_tick);
}

void ECSCore::terminate () {
	EntityManager::Terminate ();
	ComponentManager::Terminate ();
	SystemManager::Terminate ();
	EventManager::Terminate ();
	free(m_global_allocator->getStart ());
}
