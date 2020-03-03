#pragma once
#include "lua.h"

#include <map>
#include <string>

#include "LuaComponent.h"
#include "../../Base/Allocators/Allocator.h"

#include <regex>

namespace rlms {
	class LuaComponentPrototype {
	private:
		std::map<std::string, std::string> attributes_types;

		inline void SetAttributeType (lua_State*& L) {
			// uses 'key' (at index -2) and 'value' (at index -1)
			if (lua_isstring (L, -2) && lua_isstring (L, -1)) {
				attributes_types.emplace (lua_tostring (L, -2), lua_tostring (L, -1));
			} else {

				if (lua_isstring (L, -2)) {
					throw std::exception ("Bad Key Type");
				}

				if (lua_isstring (L, -1)) {
					throw std::exception ("Bad Value Type");
				}
			}
		}

		inline void GenerateAddress () {
			for (auto it = attributes_types.begin (); it != attributes_types.end (); it++) {
				size_t a;

				if (it->second == "integer") { //integer
					a = sizeof (long);
				}

				if (it->second == "number") { //float
					a = sizeof (double);
				}

				attributes_sub_address.emplace ();
			}
		}


	public:
		std::string component_type_name;
		std::map<std::string, size_t> attributes_sub_address;

		LuaComponentPrototype (lua_State* L, std::string& component_type, int const& index = -1)  : component_type_name (component_type), attributes_types ({}) {

			if (lua_istable (L, index)) {

				lua_pushnil (L);  // first key
				while (lua_next (L, index) != 0) {

					SetAttributeType (L);

					// removes 'value'; keeps 'key' for next iteration
					lua_pop (L, 1);
				}

			} else throw std::exception ("lua_istable (L, index) = false");

			GenerateAddress ();

		}

		LuaComponent* Create (Allocator* const& alloc) {

		}

	};
}
