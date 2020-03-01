#include "ComponentManager.h"

template<class C> const COMPONENT_ID ComponentManager::CreateComponent () {
	return instance->createComponent<C> ();
}

template<class C> const COMPONENT_ID ComponentManager::CreateComponent (Entity* entity) {
	return instance->createComponent<C> (entity);
}

template<class C> const COMPONENT_ID ComponentManager::CreateComponent (Entity* entity, COMPONENT_ID c_id) {
	return instance->createComponent<C> (entity, c_id);
}

template<class C> const COMPONENT_ID ComponentManager::CreateComponent (COMPONENT_ID c_id) {
	return instance->createComponent<C> (c_id);
}

template<class C> const bool ComponentManager::HasComponent (Entity* entity) {
	return instance->hasComponent<C> (entity);
}

template<class C> C* ComponentManager::GetComponent (Entity* entity) {
	return instance->getComponent<C> (entity);
}

template<class C> inline C* ComponentManager::GetComponent (ENTITY_ID e_id) {
	return instance->getComponent<C> (e_id);
}

template<class C> std::vector<C*> ComponentManager::GetComponents () {
	return instance->getComponents<C>();
}

template<class C> void ComponentManager::DestroyComponent (Entity* entity) {
	instance->destroyComponent<C> (entity);
}

///////

template<class C> inline const COMPONENT_ID ComponentManagerImpl::createComponent () {
	COMPONENT_ID c_id = procedural_id_iter();
	logger->tag (LogTags::Debug) << "Creating " << typeid(C).name () << " with procedural ID : " << c_id << "." << '\n';

	auto it = _lookup_table.find (c_id);

   //Component id isn't taken
	if (it != _lookup_table.end ()) {
		ComponentManager::n_errors++;
		logger->tag (LogTags::Error) << "ID already taken by a Component !" << '\n';
		return IComponent::NULL_ID;
	}

	//Valid

	C* new_component = new (m_comp_Allocator->allocate (sizeof (C), __alignof(C))) C (Entity::NULL_ID, c_id);
	_lookup_table.insert (std::pair<COMPONENT_ID, IComponent*> (c_id, new_component));
	return c_id;
}

template<class C> inline const COMPONENT_ID ComponentManagerImpl::createComponent (COMPONENT_ID c_id) {
	logger->tag (LogTags::Debug) << "Creating " << typeid(C).name () << " with predefined ID : " << c_id << "." << '\n';

	//Id is invalid
	if (!ComponentManager::isValid (c_id)) {
		ComponentManager::n_errors++;
		logger->tag (LogTags::Error) << "ID is invalid !" << '\n';
		return IComponent::NULL_ID;
	}

	auto it = _lookup_table.find (c_id);

   //Component id isn't taken
	if (it != _lookup_table.end ()) {
		ComponentManager::n_errors++;
		logger->tag (LogTags::Error) << "ID already taken by a Component !" << '\n';
		return IComponent::NULL_ID;
	}

	//Valid

	C* new_component = new (m_comp_Allocator->allocate (sizeof (C), __alignof(C))) C (Entity::NULL_ID, c_id);
	_lookup_table.insert (std::pair<COMPONENT_ID, IComponent*> (c_id, new_component));
	return c_id;
}

template<class C> inline const COMPONENT_ID ComponentManagerImpl::createComponent (Entity* entity) {
	COMPONENT_ID c_id = procedural_id_iter();
	logger->tag (LogTags::Debug) << "Creating " << typeid(C).name () << " with procedural ID : " << c_id << "." << '\n';

	//Entity doesn't exists
	if (entity == nullptr) {
		logger->tag (LogTags::Error) << "Entity ref is null !" << '\n';
		ComponentManager::n_errors++;
		return IComponent::NULL_ID;
	}

	//Component is not duplicate
	if (entity->has<C> ()) {
		logger->tag (LogTags::Error) << "Component already exists for this Entity!" << '\n';
		ComponentManager::n_errors++;
		return IComponent::NULL_ID;
	}

	auto it = _lookup_table.find (c_id);

   //Component id isn't taken
	if (it != _lookup_table.end ()) {
		logger->tag (LogTags::Error) << "ID already taken by a Component !" << '\n';
		ComponentManager::n_errors++;
		return IComponent::NULL_ID;
	}

	//Valid
	C* new_component = new (m_comp_Allocator->allocate (sizeof (C), __alignof(C))) C (entity->id(), c_id);
	_lookup_table.insert (std::pair<COMPONENT_ID, IComponent*> (new_component->id(), new_component));
	entity->add<C> (new_component);
	return new_component->id ();
}

template<class C> inline const COMPONENT_ID ComponentManagerImpl::createComponent (Entity* entity, COMPONENT_ID c_id) {
	logger->tag (LogTags::Debug) << "creating " << typeid(C).name() << " with predefined ID : " << c_id << "." << '\n';

	//Id is invalid
	if (!ComponentManager::isValid(c_id)) {
		logger->tag (LogTags::Error) << "ID is invalid !" << '\n';
		ComponentManager::n_errors++;
		return IComponent::NULL_ID;
	}

	//Entity doesn't exists
	if (entity == nullptr) {
		logger->tag (LogTags::Error) << "Entity ref is null !" << '\n';
		ComponentManager::n_errors++;
		return IComponent::NULL_ID;
	}

	//Component is not duplicate
	if (entity->has<C> ()) {
		logger->tag (LogTags::Error) << "Component already exists for this Entity!" << '\n';
		ComponentManager::n_errors++;
		return IComponent::NULL_ID;
	}

	auto it = _lookup_table.find (c_id);

   //Component id isn't taken
	if (it != _lookup_table.end ()) {
		logger->tag (LogTags::Error) << "ID already taken by a Component !" << '\n';
		ComponentManager::n_errors++;
		return IComponent::NULL_ID;
	}

	//Valid
	C* new_component = new (m_comp_Allocator->allocate (sizeof (C), __alignof(C))) C (entity->id(), c_id);
	_lookup_table.insert (std::pair<COMPONENT_ID, IComponent*> (c_id, new_component));
	entity->add<C> (new_component);
	return c_id;
}

template<class C> inline const bool ComponentManagerImpl::hasComponent (Entity* entity) {
	return entity->has<C>();
}

template<class C>inline const bool ComponentManagerImpl::hasComponent (COMPONENT_ID c_id) {
	return _lookup_table.find (c_id) != _lookup_table.end ();
}

template<class C> inline C* ComponentManagerImpl::getComponent (Entity* entity) {
	logger->tag (LogTags::Debug) << "Getting " << typeid(C).name () << " from Entity at : " << std::hex << entity << "." << '\n';
	COMPONENT_ID c_id = IComponent::NULL_ID;

	//Entity doesn't exists
	if (entity == nullptr) {
		logger->tag (LogTags::Error) << "Entity ref is null !" << '\n';
		ComponentManager::n_errors++;
		return IComponent::NULL_ID;
	}

	//Component exists
	if (!entity->has<C>()) {
		logger->tag (LogTags::Error) << "Entity does not have Component of that type !" << '\n';
		ComponentManager::n_errors++;
		return IComponent::NULL_ID;
	}

	return entity->get<C>();
}

template<class C> inline C* ComponentManagerImpl::getComponent (COMPONENT_ID const& c_id) {
	logger->tag (LogTags::Debug) << "Getting " << typeid(C).name () << " Component with ID : " << c_id << "." << '\n';

	//Id is invalid
	if (!ComponentManager::isValid (c_id)) {
		logger->tag (LogTags::Error) << "ID is invalid !" << '\n';
		ComponentManager::n_errors++;
		return nullptr;
	}

	auto it = _lookup_table.find (c_id);

	//Entity doesn't exists
	if (it == _lookup_table.end ()) {
		logger->tag (LogTags::Error) << "ID is not taken by an Entity !" << '\n';
		ComponentManager::n_errors++;
		return nullptr;
	}

	return static_cast<C*>(it->second);
}

template<class C>
inline std::vector<C*> ComponentManagerImpl::getComponents () {
	std::vector<C*> vec;
	for (auto it = _lookup_table.begin (); it != _lookup_table.end (); it++) {
		if (it->second != nullptr) {
			if(typeid(it->second) == typeid(C))
				vec.push_back (it->second);
		}
	}
	return vec;
}

template<class C> inline void ComponentManagerImpl::destroyComponent (Entity* entity) {
	logger->tag (LogTags::Debug) << "Destroying " << typeid(C).name () << " from Entity at : " << std::hex << entity << "." << '\n';

	//Entity doesn't exists
	if (entity == nullptr) {
		logger->tag (LogTags::Error) << "Entity ref is null !" << '\n';
		ComponentManager::n_errors++;
		return;
	}

	C* comp = entity->get<C>();

	//
	if (comp == nullptr) {
		logger->tag (LogTags::Debug) << "Component does not exist !" << '\n';
		ComponentManager::n_errors++;
		return;
	}

	comp->~C ();
	m_comp_Allocator->deallocate (comp);
	entity->rem<C> ();
	_lookup_table.erase (comp->id ());
}
