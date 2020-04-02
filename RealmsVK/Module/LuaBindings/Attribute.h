#pragma once
#include "lua.h"
#include <string>

namespace rlms {

	enum Attr_Types {
		Undefined = 0,
		Integer,
		Number,
		Boolean,
		String,
		Text,
	};

	struct Attribute {
		std::string key;
		Attr_Types type;
		void* default_value;
		size_t size;

		Attribute (lua_State*& L);
		~Attribute ();

		bool is (Attr_Types t);

	};
}
