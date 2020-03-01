#include "EntityManager.h"

//#include "../../Base/Allocators/PoolAllocator.h"
#include "../../Base/Allocators/FreeListAllocator.h"
#include "../../Base/Allocators/MasqueradeAllocator.h"

using namespace rlms;

class rlms::EntityManagerImpl : public ILogged {
private:
	friend class EntityManager;

	std::string getLogName () override {
		return "EntityManager";
	};
	using alloc_type = MasqueradeAllocator;

	std::map<ENTITY_ID, Entity*> m_lookup_table;
	std::unique_ptr<alloc_type> m_entity_Allocator;

	void start (Allocator* const& alloc, size_t entity_pool_size, std::shared_ptr<Logger> funnel);
	void stop ();

	const ENTITY_ID createEntity ();
	const ENTITY_ID createEntity (ENTITY_ID id);
	bool hasEntity (ENTITY_ID id);
	Entity* getEntity (ENTITY_ID id);
	void destroyEntity (ENTITY_ID id);

	ENTITY_ID _id_iter;
	inline ENTITY_ID procedural_id_iter () {
		return _id_iter++;
	}

public:
	EntityManagerImpl ();
	~EntityManagerImpl ();
};

int EntityManager::n_errors;
std::unique_ptr<EntityManagerImpl> EntityManager::instance;

std::shared_ptr<LoggerHandler> EntityManager::GetLogger () {
	return instance->getLogger ();
}

void EntityManager::Initialize (Allocator* const& alloc, size_t entity_pool_size, std::shared_ptr<Logger> funnel) {
	instance = std::make_unique<EntityManagerImpl> ();
	instance->start(alloc, entity_pool_size, funnel);
}

void EntityManager::Terminate () {
	instance->stop ();
	instance.reset ();
}

const ENTITY_ID EntityManager::Create () {
	return instance->createEntity ();
}

const ENTITY_ID EntityManager::Create (ENTITY_ID id) {
	return instance->createEntity (id);
}

Entity* EntityManager::Get (ENTITY_ID id) {
	return instance->getEntity (id);
}

bool EntityManager::Has (ENTITY_ID id) {
	return instance->hasEntity(id);
}

void EntityManager::Destroy (ENTITY_ID id) {
	instance->destroyEntity (id);
}

EntityManagerImpl::EntityManagerImpl () : _id_iter(1), m_lookup_table () {}
EntityManagerImpl::~EntityManagerImpl () {}

void EntityManagerImpl::start (Allocator* const& alloc, size_t entity_pool_size, std::shared_ptr<Logger> funnel) {
		
	startLogger (funnel);
	logger->tag (LogTags::None) << "Initializing !" << '\n';

	m_entity_Allocator = std::unique_ptr<alloc_type>(new alloc_type (entity_pool_size, alloc->allocate (entity_pool_size)));

	_id_iter = 1;
	EntityManager::n_errors = 0;
	logger->tag (LogTags::None) << "Initialized correctly !" << '\n';
}

void EntityManagerImpl::stop () {
	logger->tag (LogTags::None) << "Stopping !" << '\n';

	for (auto it = m_lookup_table.begin(); it != m_lookup_table.end(); it++) {
		allocator::deallocateDelete (*m_entity_Allocator.get (), it->second);
	}

	m_entity_Allocator.reset ();

	logger->tag (LogTags::None) << "Stopped correctly !" << '\n';
}

const ENTITY_ID EntityManagerImpl::createEntity () {
	Entity* new_entity;
	ENTITY_ID id = procedural_id_iter();
	logger->tag (LogTags::Debug) << "creating Entity with procedural ID : " << id << "." << '\n';

	//Id is valid
	if (!EntityManager::isValid (id)) {
		logger->tag (LogTags::Error) << "ID is invalid !" << '\n';
		EntityManager::n_errors++;
		return Entity::NULL_ID;
	}

	auto it = m_lookup_table.find (id);

	//Entity exists
	if (it != m_lookup_table.end ()) {
		EntityManager::n_errors++;
		logger->tag (LogTags::Error) << "ID already taken by an Entity !" << '\n';
		return Entity::NULL_ID;
	}

	//Valid
	_id_iter++;
	new_entity = allocator::allocateNew<Entity, ENTITY_ID> (*m_entity_Allocator.get(), std::move(id));
	m_lookup_table.insert (std::pair<ENTITY_ID, Entity*> (new_entity->id(), new_entity));
	return new_entity->id ();
}

const ENTITY_ID EntityManagerImpl::createEntity (ENTITY_ID id) {
	Entity* new_entity;
	logger->tag (LogTags::Debug) << "creating Entity with predefined ID : " << id << "." << '\n';

	//Id is invalid
	if (!EntityManager::isValid (id)) {
		logger->tag (LogTags::Error) << "ID is invalid !" << '\n';
		EntityManager::n_errors++;
		return Entity::NULL_ID;
	}

	auto it = m_lookup_table.find (id);

	//Entity exists
	if (it != m_lookup_table.end ()) {
		logger->tag (LogTags::Error) << "ID is not taken by an Entity !" << '\n';
		EntityManager::n_errors++;
		return Entity::NULL_ID;
	}

	//Valid
	new_entity = new (m_entity_Allocator->allocate (sizeof (Entity), __alignof(Entity))) Entity (id);
	m_lookup_table.insert (std::pair<ENTITY_ID, Entity*> (id, new_entity));
	return id;
}

bool EntityManagerImpl::hasEntity (ENTITY_ID id) {
	//Id is invalid
	if (!EntityManager::isValid (id)) {
		logger->tag (LogTags::Error) << "ID is invalid !" << '\n';
		EntityManager::n_errors++;
		return false;
	}

	return m_lookup_table.find (id) != m_lookup_table.end ();
}

Entity* EntityManagerImpl::getEntity (ENTITY_ID id) {
	logger->tag (LogTags::Debug) << "Getting Entity at ID " << id << "." << '\n';

	//Id is invalid
	if (!EntityManager::isValid (id)) {
		logger->tag (LogTags::Error) << "ID is invalid !" << '\n';
		EntityManager::n_errors++;
		return nullptr;
	}

	auto it = m_lookup_table.find (id);

	//Entity doesn't exists
	if (it == m_lookup_table.end()) {
		logger->tag (LogTags::Error) << "ID is not taken by an Entity !" << '\n';
		EntityManager::n_errors++;
		return nullptr;
	}

	return it->second;
}

void EntityManagerImpl::destroyEntity (ENTITY_ID id) {
	logger->tag (LogTags::Debug) << "Destroying Entity at ID " << id << "." << '\n';

	//Id is invalid
	if (!EntityManager::isValid (id)) {
		logger->tag (LogTags::Error) << "ID is invalid !" << '\n';
		EntityManager::n_errors++;
		return;
	}

	 auto it = m_lookup_table.find(id);

	//Entity doesn't exists
	if (it == m_lookup_table.end()) {
		logger->tag (LogTags::Error) << "ID is not taken by an Entity !" << '\n';
		EntityManager::n_errors++;
		return;
	}

	allocator::deallocateDelete (*m_entity_Allocator.get (), it->second);
	m_lookup_table.erase (id);
}
