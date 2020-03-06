#include "IComponent.h"
#include "ECSException.h"

#include <regex>

using namespace rlms;

IComponentPrototype* IComponent::c_proto;

IComponent* IComponentPrototype::Create (Allocator* const& alloc, ENTITY_ID const& entity_id, COMPONENT_ID const& component_id) {
	return new (alloc->allocate (_size, _align)) IComponent (entity_id, component_id);
}

void* IComponentPrototype::_Get (IComponent* const c, std::string& member) {
	if (std::regex_match (member, std::regex("[(c_id)(component_id)(id)]"))) {
		return &c->c_id;
	}
	if (std::regex_match (member, std::regex ("[(e_id)(entity_id)]"))) {
		return &c->e_id;
	}

	throw BadMemberName ();
	return nullptr;
}

void IComponentPrototype::Destroy (Allocator* const& alloc, IComponent* const& c) {
	c->~IComponent ();
	alloc->deallocate (c);
}
