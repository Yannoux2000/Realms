#include "Entity.h"

using namespace rlms;

std::vector<IComponent*> Entity::getComponents () {
	std::vector<IComponent*> vec;

	for (auto it = _components.begin (); it != _components.end (); it++) {
		vec.push_back (it->second);
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
