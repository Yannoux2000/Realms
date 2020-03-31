#include "IComponent.h"

using namespace rlms;

COMPONENT_TYPE_ID IComponentPrototype::_type_component_id_counter = 0;

const std::string IComponentPrototype::getTypeName () {
	return _type_name;
}

const COMPONENT_TYPE_ID& IComponentPrototype::getTypeId () {
	return _type_id;
}

bool rlms::IComponentPrototype::is (IComponent* const c) {
	return _type_id == c->_type_id;
}

IComponent* IComponentPrototype::Create (Allocator* const& alloc, ENTITY_ID const& entity_id, COMPONENT_ID const& component_id) {
	auto c = new (alloc->allocate (_size, _align)) IComponent (entity_id, component_id, _type_id);
	Populate (c);
	return c;
}

void* IComponentPrototype::Get (IComponent* const c, std::string&& member) {
	void* ret = nullptr;
	if (_type_id != c->_type_id) {
		throw BadComponentProto ();
		return ret;
	}

	ret = GetAttrib (c, member);

	if (member == "c_id" || member == "id") {
		return &c->c_id;
	}
	if (member == "e_id") {
		return &c->e_id;
	}

	if (ret == nullptr) {
		throw BadMemberName ();
	}
	return ret;
}

void IComponentPrototype::Destroy (Allocator* const& alloc, IComponent* const& c) {
	c->~IComponent ();
	alloc->deallocate (c);
}
