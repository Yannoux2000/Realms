#pragma once
#include "../ECS/IComponent.h"

#include <vector>
#include <map>
#include <sstream>

#include "lua.h"
#include "Attribute.h"

namespace rlms {
	class LuaComponentPrototype;
	class LuaComponent : public IComponent {
	public:
		LuaComponent(ENTITY_ID const& entity_id, COMPONENT_ID const& component_id, COMPONENT_TYPE_ID const& type_id) : IComponent(entity_id, component_id, type_id) {}
	};

	class LuaComponentPrototype : public IComponentPrototype {
	private:
		std::map<std::string, Attribute> attributes_sub_address;

		inline void SetAttributeType (lua_State*& L) {
			// uses 'key' (at index -2) and 'value' (at index -1)
			if (lua_isstring (L, -2) && !(lua_istable (L, -1) || lua_isfunction (L, -1) || lua_isnoneornil (L, -1))) {
				std::string key = lua_tostring (L, -2);
				attributes_sub_address.emplace (key, L);
			} else {
				if (!lua_isstring (L, -2)) {
					throw std::exception ("Bad Key Type");
				}

				if (lua_istable (L, -1) || lua_isfunction (L, -1) || lua_isnoneornil (L, -1)) {
					throw std::exception ("Bad Value Type");
				}
			}
		}

	public:
		LuaComponentPrototype (lua_State* L, std::string& component_type) 
			: IComponentPrototype (component_type,0 ,0), attributes_sub_address ({}) {
			if (lua_istable (L, -1)) {

				lua_pushnil (L);  // first key
				while (lua_next (L, -2) != 0) {
					SetAttributeType (L);

					// removes 'value'; keeps 'key' for next iteration
					lua_pop (L, 1);
				}
				_size = sizeof (IComponent);
				for (auto it = attributes_sub_address.begin (); it != attributes_sub_address.end (); it++) {
					_size += it->second.size;
				}

				//if (_size == 0) throw std::exception ("luacomponent prototype is invalid"); //should never happend
			} else throw std::exception ("lua_istable (L, index) = false");
		}

		void Populate (IComponent* const& c) override {
			uintptr_t addr = (uintptr_t)c + sizeof(IComponent);
			for (auto it = attributes_sub_address.begin (); it != attributes_sub_address.end (); it++) {
				std::memcpy ((void*)addr, it->second.default_value, it->second.size);
				addr += it->second.size;
			}
		}

		void* GetAttrib (IComponent* const& c, std::string& member) override {
			if (IBase::is<LuaComponent> (c)) {
				uintptr_t addr = (uintptr_t)c + sizeof (IComponent);
				for (auto it = attributes_sub_address.begin (); it != attributes_sub_address.end (); it++) {
					if (it->first == member) {
						return (void*)addr;
					} else {
						addr += it->second.size;
					}
				}
			}
		}

		void Set (IComponent* const c, std::string&& member) {
			if (IBase::is<LuaComponent> (c)) {

				uintptr_t addr = (uintptr_t)c + sizeof (IComponent);
				for (auto it = attributes_sub_address.begin (); it != attributes_sub_address.end (); it++) {
					if (it->first == member) {
						break;
					} else {
						addr += it->second.size;
					}
				}
			} else throw std::exception("object is not a Component");
		}

		std::string debug () {
			std::stringstream ss ("");
			ss << "\t Prototype " << _type_name << " :\n";
			for (auto it = attributes_sub_address.begin (); it != attributes_sub_address.end (); it++) {
				ss << "" << it->first << " = (" << it->second.size << ", ";

				if (it->second.is(Attr_Types::Integer)) {
					ss << *((long*)it->second.default_value);

				} else if (it->second.is (Attr_Types::Number)) {
					ss << *((double*)it->second.default_value);

				} else if (it->second.is (Attr_Types::Boolean)) {
					ss << *((bool*)it->second.default_value);

				} else if (it->second.is (Attr_Types::String)) {
					ss << *(std::string*)it->second.default_value;

				} else if (it->second.is (Attr_Types::Text)) {
					ss << *(std::string*)it->second.default_value;
				}

				ss << ")\n";
			}
			ss << "}\n";
			return ss.str ();
		}

		std::string debug (IComponent* const& c) {
			std::stringstream ss ("");
			uintptr_t addr = (uintptr_t)c + sizeof (IComponent);

			ss << "\t " << _type_name << " : " << addr << "\n";
			ss << "( c_id = " << *(Get<COMPONENT_ID> (c, "c_id")) << ", e_id = " << *(Get<ENTITY_ID> (c, "e_id")) << ") : \n";

			for (auto it = attributes_sub_address.begin (); it != attributes_sub_address.end (); it++) {
				ss << "" << it->first << " = (" << it->second.size << ", " << addr << " : ";
				
				if (it->second.is(Attr_Types::Integer)) {
					ss << *((long*)addr);

				} else if (it->second.is (Attr_Types::Number)) {
					ss << *((double*)addr);

				} else if (it->second.is (Attr_Types::Boolean)) {
					ss << *((bool*)addr);

				} else if (it->second.is (Attr_Types::String)) {
					ss << *(std::string*)addr;

				} else if (it->second.is (Attr_Types::Text)) {
					ss << *(std::string*)addr;
				}

				ss << ")\n";
				addr += it->second.size;
			}
			return ss.str ();
		}
	};
}
