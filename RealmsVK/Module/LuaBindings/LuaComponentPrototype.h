#pragma once
#include "lua.h"


#include <map>
#include <string>

namespace rlms {
	class LuaComponentPrototype {
	public:

		std::string component_type_name;
		std::map<std::string, std::string> attributes_types;

		LuaComponentPrototype (lua_State* L, std::string& component_type) 
			: component_type_name (component_type), attributes_types ({}) {
			lua_istable (L, -1);
		}

	private:
		void SetAttribute (lua_State*& L, std::string) {

		}



	};
}
