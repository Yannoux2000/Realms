#include "Entity.h"

using namespace rlms;

void Entity::add (IComponent* c) {
	_components[c->type_id()] = c;
}

bool Entity::has (COMPONENT_TYPE_ID const& c_type_id) {
	return (_components.find (c_type_id) != _components.end ());
}

IComponent* Entity::get (COMPONENT_TYPE_ID const& c_type_id) {
	auto it = _components.find (c_type_id);
	if (it != _components.end ()) {
		return it->second;
	}
	return nullptr;
}

void Entity::rem (COMPONENT_TYPE_ID const& c_type_id) {
	_components.erase (c_type_id);
}

std::vector<IComponent*> Entity::getComponents () {
	std::vector<IComponent*> vec;
	for (const auto& c : _components) {
		vec.push_back (c.second);
	}
	return vec;
}

void Entity::rem (IComponent*& comp_ptr) {
	for (auto it = _components.begin (); it != _components.end (); it++) {
		if (it->second == comp_ptr) {
			_components.erase (it);
			break;
		}
	}
}
