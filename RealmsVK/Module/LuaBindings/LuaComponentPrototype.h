#pragma once
#include "lua.h"

#include "LuaComponent.h"
#include "../../Base/Allocators/Allocator.h"

#include <regex>
#include <map>
#include <string>
#include <vector>
#include <sstream>

namespace rlms {
	class LuaComponentPrototype {
	private:
		static std::vector<LuaComponentPrototype*> all_protos;

		std::string component_type_name;
		std::map<std::string, size_t> attributes_sub_address;

		size_t size, align;

		inline void SetAttributeType (lua_State*& L) {
			// uses 'key' (at index -2) and 'value' (at index -1)
			if (lua_isstring (L, -2) && !(lua_istable (L, -1) || lua_isfunction(L, -1) || lua_isnoneornil(L, -1))) {
				size_t a;

				std::string key (lua_tostring (L, -2));
				std::string value (lua_typename (L, lua_type (L, -1)));

				if (value == "integer") { //integer

					align = alignof (long);
					a = sizeof (long);

				} else if (value == "number") { //number

					align = alignof (double);
					a = sizeof (double);

				} else if (value == "boolean") { //boolean

					align = alignof (bool);
					a = sizeof (bool);

				} else if (value == "string") { //string
					size_t* l = new size_t(0);

					luaL_checklstring (L, -1, l);
					if (*l > 32) { //512
						*l = 512;
					} else { //32
						*l = 32;
					}
					align = alignof (char);
					a = sizeof (char) * *l;

					delete l;
				}

				attributes_sub_address.emplace (key, a);
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

		static std::vector<LuaComponentPrototype*>& getVec () {
			return all_protos;
		}

		LuaComponentPrototype (lua_State* L, std::string& component_type)  : size(0), align(0), component_type_name (component_type), attributes_sub_address ({}) {
			if (lua_istable (L, -1)) {

				lua_pushnil (L);  // first key
				while (lua_next (L, -2) != 0) {
					SetAttributeType (L);

					// removes 'value'; keeps 'key' for next iteration
					lua_pop (L, 1);
				}

			} else throw std::exception ("lua_istable (L, index) = false");

			all_protos.push_back (this);
		}

		LuaComponent* Create (Allocator* const& alloc) {
			return (LuaComponent*) alloc->allocate (size);
		}

		void Destroy (Allocator* const& alloc, LuaComponent* const& c) {
			alloc->deallocate (c);
		}

		//accessor
		void* GetAttrib (LuaComponent* const& c, std::string&& name) {
			uintptr_t addr = (uintptr_t)c;
			for (auto it = attributes_sub_address.begin (); it != attributes_sub_address.end (); it++) {
				addr += it->second;
			}
			return (void*)addr;
		}

		std::string debug () {
			std::stringstream ss("");
			ss << "\t" << component_type_name << " :\n";
			for (auto it = attributes_sub_address.begin(); it != attributes_sub_address.end(); it++) {
				ss << "" << it->first << " = " << it->second << "\n";
			}
			ss << "}\n";
			return ss.str ();
		}
	};
}
