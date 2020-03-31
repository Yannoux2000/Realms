#include "ComponentManager.h"

using namespace rlms;

class rlms::ComponentManagerImpl : public ILogged {
private:
	friend class ComponentManager;

	std::string getLogName () override {
		return "ComponentManager";
	};

	using alloc_type = MasqueradeAllocator;

	std::map<COMPONENT_ID, IComponent*> _lookup_table;
	std::map<COMPONENT_TYPE_ID, std::unique_ptr<IComponentPrototype>> _proto_table;
	std::unique_ptr<alloc_type> m_comp_Allocator;

	void start (Allocator* const& alloc, size_t entity_pool_size, std::shared_ptr<Logger> funnel);
	void stop ();

	const COMPONENT_ID createComponent (COMPONENT_TYPE_ID const& c_type, Entity* entity);
	const COMPONENT_ID createComponent (COMPONENT_TYPE_ID const& c_type, COMPONENT_ID c_id, Entity* entity);

	const bool hasEntity (COMPONENT_ID c_id);
	const bool hasComponent (COMPONENT_ID c_id);

	const ENTITY_ID& getEntity (COMPONENT_ID c_id);
	IComponent* getComponent (COMPONENT_ID c_id);
	std::vector<IComponent*> getComponents (COMPONENT_TYPE_ID const& c_type);

	void destroyComponent (COMPONENT_TYPE_ID const& c_type, Entity* entity);
	void destroyComponent (COMPONENT_ID c_id);

	COMPONENT_ID _id_iter;
	inline COMPONENT_ID procedural_id_iter () {
		return _id_iter++;
	}
public:
	ComponentManagerImpl ();
	~ComponentManagerImpl ();
};

int ComponentManager::n_errors;
std::unique_ptr<rlms::ComponentManagerImpl> ComponentManager::instance;

std::shared_ptr<LoggerHandler> ComponentManager::GetLogger () {
	return instance->getLogger();
}

void ComponentManager::Initialize (Allocator* const& alloc, size_t comp_pool_size, std::shared_ptr<Logger> funnel) {
	instance = std::make_unique<ComponentManagerImpl> ();
	instance->start (alloc, comp_pool_size, funnel);
}

void ComponentManager::Terminate () {
	instance->stop ();
	instance.reset ();
}

const ENTITY_ID& ComponentManager::GetEntity (COMPONENT_ID c_id) {
	return instance->getEntity (c_id);
}

const COMPONENT_ID rlms::ComponentManager::CreateComponent (COMPONENT_TYPE_ID const& c_type, Entity* entity) {
	return instance->createComponent (c_type, entity);
}

const COMPONENT_ID rlms::ComponentManager::CreateComponent (COMPONENT_TYPE_ID const& c_type, COMPONENT_ID c_id, Entity* entity) {
	return instance->createComponent (c_type, c_id, entity);
}

const bool ComponentManager::HasEntity (COMPONENT_ID c_id) {
	return instance->hasEntity (c_id);
}

const bool ComponentManager::HasComponent (COMPONENT_ID c_id) {
	return instance->hasComponent(c_id);
}

IComponent* ComponentManager::GetComponent (COMPONENT_ID c_id) {
	return instance->getComponent (c_id);
}

std::vector<IComponent*> rlms::ComponentManager::GetComponents (COMPONENT_TYPE_ID const& c_type) {
	return instance->getComponents (c_type);
}

void rlms::ComponentManager::DestroyComponent (COMPONENT_TYPE_ID const& c_type, Entity* entity) {
	instance->destroyComponent (c_type, entity);
}

void ComponentManager::DestroyComponent (COMPONENT_ID c_id) {
	instance->destroyComponent (c_id);
}

//////

ComponentManagerImpl::ComponentManagerImpl () : _id_iter (1), _lookup_table(), _proto_table() {}
ComponentManagerImpl::~ComponentManagerImpl () {}

void ComponentManagerImpl::start (Allocator* const& alloc, size_t comp_pool_size, std::shared_ptr<Logger> funnel) {
	startLogger (funnel);
	logger->tag (LogTags::None) << "Initializing !" << '\n';

	m_comp_Allocator = std::unique_ptr<alloc_type> (new alloc_type (comp_pool_size, alloc->allocate (comp_pool_size)));

	_id_iter = 1;
	ComponentManager::n_errors = 0;
	logger->tag (LogTags::None) << "Initialized correctly !" << '\n';
}
void ComponentManagerImpl::stop () {
	logger->tag (LogTags::None) << "Stopping !" << '\n';

	for (auto it = _lookup_table.begin (); it != _lookup_table.end (); it++) {
		it->second->~IComponent ();
		allocator::deallocateDelete (*m_comp_Allocator.get(), it->second);
	}

	m_comp_Allocator.reset ();

	logger->tag (LogTags::None) << "Stopped correctly !" << '\n';
}

const COMPONENT_ID ComponentManagerImpl::createComponent (COMPONENT_TYPE_ID const& c_type, Entity* entity) {
	COMPONENT_ID c_id = procedural_id_iter ();
	logger->tag (LogTags::Debug) << "Creating component of type_id : " << c_type << " with procedural ID : " << c_id << "." << '\n';

	ENTITY_ID e_id = ((entity != nullptr) ? entity->id () : Entity::NULL_ID);

	//Id is invalid
	if (!ComponentManager::isValid (c_id)) {
		logger->tag (LogTags::Error) << "ID is invalid !" << '\n';
		ComponentManager::n_errors++;
		return IComponent::NULL_ID;
	}

   //Component id isn't taken
	auto it = _lookup_table.find (c_id);
	if (it != _lookup_table.end ()) {
		logger->tag (LogTags::Error) << "ID already taken by a Component !" << '\n';
		ComponentManager::n_errors++;
		return IComponent::NULL_ID;
	}

	//Type_id is invalid
	auto it_proto = _proto_table.find (c_type);
	if (it_proto != _proto_table.end ()) {
		logger->tag (LogTags::Error) << "Type_ID is invalid !" << '\n';
		ComponentManager::n_errors++;
		return IComponent::NULL_ID;
	}

	//Component is not duplicate
	if (entity->has (c_type)) {
		logger->tag (LogTags::Error) << "Component already exists for this Entity!" << '\n';
		ComponentManager::n_errors++;
		return IComponent::NULL_ID;
	}

	//Valid
	IComponent* c = it_proto->second->Create (m_comp_Allocator.get (), e_id, c_id);
	_lookup_table.insert (std::pair<COMPONENT_ID, IComponent*> (c_id, c));
	entity->add (c);
	return c_id;
}
const COMPONENT_ID ComponentManagerImpl::createComponent (COMPONENT_TYPE_ID const& c_type, COMPONENT_ID c_id, Entity* entity) {
	logger->tag (LogTags::Debug) << "creating component of type_id : " << c_type << " with predefined ID : " << c_id << "." << '\n';

	ENTITY_ID e_id = ((entity != nullptr) ? entity->id() : Entity::NULL_ID);

	//Id is invalid
	if (!ComponentManager::isValid (c_id)) {
		logger->tag (LogTags::Error) << "ID is invalid !" << '\n';
		ComponentManager::n_errors++;
		return IComponent::NULL_ID;
	}

	//Type_id is invalid
	auto it_proto = _proto_table.find (c_type);
	if (it_proto != _proto_table.end ()) {
		logger->tag (LogTags::Error) << "Type_ID is invalid !" << '\n';
		ComponentManager::n_errors++;
		return IComponent::NULL_ID;
	}

	//Component is not duplicate
	if (entity->has (c_type)) {
		logger->tag (LogTags::Error) << "Component already exists for this Entity!" << '\n';
		ComponentManager::n_errors++;
		return IComponent::NULL_ID;
	}

   //Component id isn't taken
	auto it = _lookup_table.find (c_id);
	if (it != _lookup_table.end ()) {
		logger->tag (LogTags::Error) << "ID already taken by a Component !" << '\n';
		ComponentManager::n_errors++;
		return IComponent::NULL_ID;
	}

	//Valid
	IComponent* c = it_proto->second->Create (m_comp_Allocator.get (), e_id, c_id);
	_lookup_table.insert (std::pair<COMPONENT_ID, IComponent*> (c_id, c));
	entity->add (c);
	return c_id;
}

const bool ComponentManagerImpl::hasEntity (COMPONENT_ID c_id) {
	auto it = _lookup_table.find (c_id);

	//Component id is not taken
	if (it == _lookup_table.end ()) {
		return IComponent::NULL_ID;
	}

	return (it->second->e_id != Entity::NULL_ID);
}
const bool ComponentManagerImpl::hasComponent (COMPONENT_ID c_id) {
	return _lookup_table.find (c_id) != _lookup_table.end ();
}

const ENTITY_ID& ComponentManagerImpl::getEntity (COMPONENT_ID c_id) {
	logger->tag (LogTags::Debug) << "Getting Entity ID from Component at ID " << c_id << "." << '\n';

	//Id is invalid
	if (!ComponentManager::isValid (c_id)) {
		logger->tag (LogTags::Error) << "ID is invalid !" << '\n';
		ComponentManager::n_errors++;
		return Entity::NULL_ID;
	}

   //Component doesn't exists
	auto it = _lookup_table.find (c_id);
	if (it == _lookup_table.end ()) {
		logger->tag (LogTags::Error) << "ID is not taken by a Component !" << '\n';
		ComponentManager::n_errors++;
		return Entity::NULL_ID;
	}

	return it->second->e_id;
}
IComponent* ComponentManagerImpl::getComponent (COMPONENT_ID c_id) {
	logger->tag (LogTags::Debug) << "Getting Component at ID " << c_id << "." << '\n';

	//Id is invalid
	if (!ComponentManager::isValid (c_id)) {
		logger->tag (LogTags::Error) << "ID is invalid !" << '\n';
		ComponentManager::n_errors++;
		return nullptr;
	}

   //Component doesn't exists
	auto it = _lookup_table.find (c_id);
	if (it == _lookup_table.end ()) {
		logger->tag (LogTags::Error) << "ID is not taken by a Component !" << '\n';
		ComponentManager::n_errors++;
		return nullptr;
	}

	return it->second;
}

std::vector<IComponent*> rlms::ComponentManagerImpl::getComponents (COMPONENT_TYPE_ID const& c_type) {
	std::vector<IComponent*> vec;
	for (const auto& c : _lookup_table) {
		if (c.second->type_id () == c_type) {
			vec.push_back (c.second);
		}
	}
	return vec;
}

void rlms::ComponentManagerImpl::destroyComponent (COMPONENT_TYPE_ID const& c_type, Entity* entity) {
	if (entity != nullptr) {
		IComponent* c = entity->get (c_type);
		COMPONENT_ID c_id = c->id ();

		//Type_id is invalid
		auto it_proto = _proto_table.find (c_type);
		if (it_proto != _proto_table.end ()) {
			logger->tag (LogTags::Error) << "Type_ID is invalid !" << '\n';
			ComponentManager::n_errors++;
			return;
		}

		it_proto->second->Destroy (m_comp_Allocator.get (), c);
		_lookup_table.erase (c_id);
	}
}

void ComponentManagerImpl::destroyComponent (COMPONENT_ID c_id) {
	logger->tag (LogTags::Debug) << "Destroying Component at ID " << c_id << "." << '\n';

	//Id is invalid
	if (!ComponentManager::isValid (c_id)) {
		logger->tag (LogTags::Error) << "ID is invalid !" << '\n';
		ComponentManager::n_errors++;
		return;
	}

	//Component doesn't exists
	auto it = _lookup_table.find (c_id);
	if (it == _lookup_table.end ()) {
		logger->tag (LogTags::Error) << "ID is not taken by a Component !" << '\n';
		ComponentManager::n_errors++;
		return;
	}


	_lookup_table.erase (it);
}
