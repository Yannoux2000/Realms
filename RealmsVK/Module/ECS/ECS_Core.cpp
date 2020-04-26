#include "ECS_Core.h"

#include "../../Base/Allocators/MasqueradeAllocator.h"

using namespace rlms;

class rlms::ECS_CoreImpl : public ILogged {
private:
	friend class ECS_Core;

	std::string getLogName () {
		return "ECS_Core";
	}
	using alloc_type = MasqueradeAllocator;

	double m_dt_offset;

	std::unique_ptr<alloc_type> m_global_allocator;

	void start (Allocator* const& alloc, size_t comp_pool_size, std::shared_ptr<Logger> funnel);

	void update (double dt);

	void stop ();

public:
	ECS_CoreImpl ();
	~ECS_CoreImpl ();
};

std::unique_ptr<ECS_CoreImpl> ECS_Core::instance;


std::shared_ptr<LoggerHandler> ECS_Core::GetLogger () {
	return instance->getLogger();
}

void ECS_Core::Initialize (Allocator* const& alloc, size_t ecs_pool_size, std::shared_ptr<Logger> funnel) {
	instance = std::make_unique<ECS_CoreImpl> ();
	instance->start (alloc, ecs_pool_size, funnel);
}

void ECS_Core::Update (double dt) {
	instance->update (dt);
}

void ECS_Core::Terminate () {
	instance->stop ();
}

void ECS_CoreImpl::start (Allocator* const& alloc, size_t ecs_pool_size, std::shared_ptr<Logger> funnel) {
	startLogger (funnel);
	logger->tag (LogTags::None) << "Initializing !" << '\n';

	//Valid
	m_global_allocator = std::unique_ptr<alloc_type> (new alloc_type (ecs_pool_size, alloc->allocate (ecs_pool_size)));

	EntityManager::Initialize (m_global_allocator.get (), ecs_pool_size / 4, logger);
	ComponentManager::Initialize (m_global_allocator.get (), ecs_pool_size / 4, logger);

	logger->tag (LogTags::None) << "Initialized correctly !" << '\n';
}

void ECS_CoreImpl::update (double dt) {

}

void ECS_CoreImpl::stop () {
	logger->tag (LogTags::None) << "Stopping" << '\n';

	m_global_allocator.reset ();

	logger->tag (LogTags::None) << "Stopped correctly !" << '\n';
}

ECS_CoreImpl::ECS_CoreImpl () : m_dt_offset(0) {}

ECS_CoreImpl::~ECS_CoreImpl () {}
