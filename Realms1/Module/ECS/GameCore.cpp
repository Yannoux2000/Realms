#include "GameCore.h"

using namespace rlms;

class rlms::GameCoreImpl : public ILogged {
private:
	friend class GameCore;

	std::string getLogName () {
		return "GameCore";
	}

	double m_dt_offset;

	GameCoreSettings stgs;

	IGameLoaderSystem* _loader_system;
	std::unique_ptr<FreeListAllocator> m_global_allocator;

	bool start (std::shared_ptr<Logger> funnel);

	void update (double dt);

	void preUpdate (GAME_TICK_TYPE _current_tick);
	void midUpdate (GAME_TICK_TYPE _current_tick);
	void postUpdate (GAME_TICK_TYPE _current_tick);

	void stop ();

public:
	GameCoreImpl ();
	~GameCoreImpl ();
};

std::unique_ptr<GameCoreImpl> GameCore::instance;


std::shared_ptr<LoggerHandler> GameCore::GetLogger () {
	return instance->getLogger();
}

bool GameCore::Initialize (std::shared_ptr<Logger> funnel) {
	instance = std::make_unique<GameCoreImpl> ();
	return instance->start (funnel);
}

bool GameCore::LoadSettings (std::string config_file) {
	


	return true;
}

void GameCore::setGameLoaderSystem (IGameLoaderSystem* loader) {
	instance->_loader_system = loader;
}

void GameCore::Update (double dt) {
	instance->update (dt);
}

void GameCore::Terminate () {
	instance->stop ();
}

bool GameCoreImpl::start (std::shared_ptr<Logger> funnel) {
	startLogger (funnel);

	//invalid amount of memory
	if (stgs.game_mem_alloc_size <= 0) {
		logger->tag (LogTags::Error) << "Please be more reasonable with the memory amount : 1024M is good enouth." << '\n';
		return false;
	}

	//invalid amount of memory
	if (stgs.game_mem_alloc_size < (
		stgs.entity_mem_alloc_size
		+ stgs.component_mem_alloc_size
		+ stgs.system_mem_alloc_size
		+ stgs.event_mem_alloc_size
		)) {
		logger->tag (LogTags::Error) << "Amount of memory requested does not add up." << '\n';
		return false;
	}

	//Valid
	void* alloc = malloc (stgs.game_mem_alloc_size);
	m_global_allocator = std::make_unique<FreeListAllocator, void*, size_t> (std::move (alloc), std::move (stgs.game_mem_alloc_size));


	EntityManager::Initialize (m_global_allocator.get (), stgs.entity_mem_alloc_size, logger);
	ComponentManager::Initialize (m_global_allocator.get (), stgs.component_mem_alloc_size, logger);
	SystemManager::Initialize (m_global_allocator.get (), stgs.system_mem_alloc_size, logger);
	EventManager::Initialize (m_global_allocator.get (), stgs.event_mem_alloc_size, logger);
	WorldManager::Initialize (m_global_allocator.get (), stgs.world_mem_alloc_size, logger);

	return true;
}

GameCoreImpl::GameCoreImpl () : _loader_system(nullptr), m_dt_offset(0) {}

GameCoreImpl::~GameCoreImpl () {}
