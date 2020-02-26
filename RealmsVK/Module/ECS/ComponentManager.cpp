#include "ComponentManager.h"

using namespace rlms;

int ComponentManager::n_errors;
std::unique_ptr<ComponentManagerImpl> ComponentManager::instance;

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

const bool ComponentManager::HasEntity (COMPONENT_ID c_id) {
	return instance->hasEntity (c_id);
}

const bool ComponentManager::HasComponent (COMPONENT_ID c_id) {
	return instance->hasComponent(c_id);
}

IComponent* ComponentManager::GetComponent (COMPONENT_ID c_id) {
	return instance->getComponent (c_id);
}

void ComponentManager::DestroyComponent (COMPONENT_ID c_id) {
	instance->destroyComponent (c_id);
}

//////

ComponentManagerImpl::ComponentManagerImpl () : _id_iter (1), _lookup_table() {}

ComponentManagerImpl::~ComponentManagerImpl () {}

void ComponentManagerImpl::start (Allocator* const& alloc, size_t comp_pool_size, std::shared_ptr<Logger> funnel) {
	startLogger (funnel);
	logger->tag (LogTags::None) << "Initializing !" << '\n';

	m_comp_Allocator = std::unique_ptr<alloc_type> (new alloc_type (alloc->allocate (comp_pool_size), comp_pool_size));

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

const bool ComponentManagerImpl::hasEntity (COMPONENT_ID c_id) {
	auto it = _lookup_table.find (c_id);

	//Component id is taken
	if (it == _lookup_table.end ()) {
		return IComponent::NULL_ID;
	}

	return it->second->entity_id();
}

const bool ComponentManagerImpl::hasComponent (COMPONENT_ID c_id) {
	return _lookup_table.find (c_id) != _lookup_table.end ();
}

const ENTITY_ID& ComponentManagerImpl::getEntity (COMPONENT_ID const& c_id) {
	logger->tag (LogTags::Debug) << "Getting Entity ID from Component at ID " << c_id << "." << '\n';

	//Id is invalid
	if (!ComponentManager::isValid (c_id)) {
		logger->tag (LogTags::Error) << "ID is invalid !" << '\n';
		ComponentManager::n_errors++;
		return Entity::NULL_ID;
	}

	auto it = _lookup_table.find (c_id);

   //Component doesn't exists
	if (it == _lookup_table.end ()) {
		logger->tag (LogTags::Error) << "ID is not taken by a Component !" << '\n';
		ComponentManager::n_errors++;
		return Entity::NULL_ID;
	}

	return it->second->entity_id ();
}

IComponent* ComponentManagerImpl::getComponent (COMPONENT_ID const& c_id) {
	logger->tag (LogTags::Debug) << "Getting Component at ID " << c_id << "." << '\n';

	//Id is invalid
	if (!ComponentManager::isValid (c_id)) {
		logger->tag (LogTags::Error) << "ID is invalid !" << '\n';
		ComponentManager::n_errors++;
		return nullptr;
	}

	auto it = _lookup_table.find (c_id);

   //Component doesn't exists
	if (it == _lookup_table.end ()) {
		logger->tag (LogTags::Error) << "ID is not taken by a Component !" << '\n';
		ComponentManager::n_errors++;
		return nullptr;
	}

	return it->second;
}

void ComponentManagerImpl::destroyComponent (COMPONENT_ID c_id) {
	logger->tag (LogTags::Debug) << "Destroying Component at ID " << c_id << "." << '\n';

	//Id is invalid
	if (!ComponentManager::isValid (c_id)) {
		logger->tag (LogTags::Error) << "ID is invalid !" << '\n';
		ComponentManager::n_errors++;
		return;
	}

	auto it = _lookup_table.find (c_id);

   //Component doesn't exists
	if (it == _lookup_table.end ()) {
		logger->tag (LogTags::Error) << "ID is not taken by a Component !" << '\n';
		ComponentManager::n_errors++;
		return;
	}


	it->second->~IComponent ();

	allocator::deallocateDelete (*m_comp_Allocator.get (), it->second);
	if (EntityManager::Has (it->second->entity_id ())) {
		EntityManager::Destroy (it->second->entity_id ());
	}
	_lookup_table.erase (c_id);
}
