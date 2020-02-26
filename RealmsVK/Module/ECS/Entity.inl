#pragma once

namespace rlms {
	template<class C> inline void Entity::add (C* c) {
		_components[std::type_index (typeid(C))] = c;
		//_components.insert ({ std::type_index (typeid(C)), c });
	}

	template <typename C> inline C* Entity::get () {
		auto it = _components.find (std::type_index (typeid(C)));
		if (it != _components.end ()) {
			return dynamic_cast<C*>(it->second);
		}
		return nullptr;
	}

	template<class C> inline bool Entity::has () {
		return (_components.find (std::type_index (typeid(C))) != _components.end ());
	}

	template<class C> inline void Entity::rem () {
		_components.erase (std::type_index (typeid(C)));
	}
}