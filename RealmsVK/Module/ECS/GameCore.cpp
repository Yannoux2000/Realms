#include "GameCore.h"

#include "../../Base/Allocators/MasqueradeAllocator.h"

using namespace rlms;

class rlms::GameCoreImpl : public ILogged {
private:
	friend class GameCore;

	std::string getLogName () {
		return "GameCore";
	}
	using alloc_type = MasqueradeAllocator;

	double m_dt_offset;

	std::unique_ptr<alloc_type> m_global_allocator;

	void start (Allocator* const& alloc, size_t comp_pool_size, std::shared_ptr<Logger> funnel);

	void update (double dt);

	//void preUpdate (GAME_TICK_TYPE _current_tick);
	//void midUpdate (GAME_TICK_TYPE _current_tick);
	//void postUpdate (GAME_TICK_TYPE _current_tick);

	void stop ();

public:
	GameCoreImpl ();
	~GameCoreImpl ();
};

std::unique_ptr<GameCoreImpl> GameCore::instance;


std::shared_ptr<LoggerHandler> GameCore::GetLogger () {
	return instance->getLogger();
}

void GameCore::Initialize (Allocator* const& alloc, size_t ecs_pool_size, std::shared_ptr<Logger> funnel) {
	instance = std::make_unique<GameCoreImpl> ();
	instance->start (alloc, ecs_pool_size, funnel);
}

void GameCore::Update (double dt) {
	instance->update (dt);
}

void GameCore::Terminate () {
	instance->stop ();
}

void GameCoreImpl::start (Allocator* const& alloc, size_t ecs_pool_size, std::shared_ptr<Logger> funnel) {
	startLogger (funnel);
	logger->tag (LogTags::None) << "Initializing !" << '\n';

	//Valid
	m_global_allocator = std::unique_ptr<alloc_type> (new alloc_type (alloc->allocate (ecs_pool_size), ecs_pool_size));

	EntityManager::Initialize (m_global_allocator.get (), ecs_pool_size / 4, logger);
	ComponentManager::Initialize (m_global_allocator.get (), ecs_pool_size / 4, logger);

	logger->tag (LogTags::None) << "Initialized correctly !" << '\n';
}

void rlms::GameCoreImpl::update (double dt) {

}

void rlms::GameCoreImpl::stop () {
	logger->tag (LogTags::None) << "Stopping" << '\n';

	m_global_allocator.reset ();

	logger->tag (LogTags::None) << "Stopped correctly !" << '\n';
}

GameCoreImpl::GameCoreImpl () : m_dt_offset(0) {}

GameCoreImpl::~GameCoreImpl () {}
