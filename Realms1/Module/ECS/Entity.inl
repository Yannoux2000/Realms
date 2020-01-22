#pragma once

namespace rlms{
	template<class C> inline void Entity::addComponent (C* c) {
		_components[&typeid(C)] = c;
	}

	template<class C> inline C* Entity::getComponent () {
		if (hasComponent<C> ()) {
			return static_cast<C*>(_components[&typeid(C)]);
		}
		return nullptr;
	}

	template<class C> inline bool Entity::hasComponent () {
		return (_components.find (&typeid(C)) != _components.end ());
	}

	template<class C> inline void Entity::remComponent () {
		_components.erase (&typeid(C));
	}
}