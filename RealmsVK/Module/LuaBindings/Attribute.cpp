#include "Attribute.h"

using namespace rlms;

Attribute::Attribute (lua_State*& L) {
	key = lua_tostring (L, -2);
	std::string type_name = lua_typename (L, lua_type (L, -1));

	if (type_name == "integer") { //integer

		type = Integer;
		//align = alignof (long);
		size = sizeof (long);
		default_value = new long (std::move (lua_tointeger (L, -1)));

	} else if (type_name == "number") { //number

		type = Number;
		//align = alignof (double);
		size = sizeof (double);
		default_value = new double (std::move (lua_tonumber (L, -1)));

	} else if (type_name == "boolean") { //boolean

		type = Boolean;
		//align = alignof (bool);
		size = sizeof (bool);
		default_value = new bool (std::move (lua_toboolean (L, -1)));

	} else if (type_name == "string") { //string
		size_t* l = new size_t (0);

		luaL_checklstring (L, -1, l);
		if (*l > 32) { //512
			*l = 512;
			type = Text;
		} else { //32
			*l = 32;
			type = String;
		}

		//align = alignof (char);
		std::string* v = new std::string (*l, 0);
		v->assign (lua_tostring (L, -1));
		size = sizeof (*v);
		default_value = v;
		delete l;
	}
}

Attribute::~Attribute () {
	if (type == Integer) { //integer
		long* v = static_cast<long*>(default_value);
		delete v;
	} else if (type == Number) { //number
		double* v = static_cast<double*>(default_value);
		delete v;
	} else if (type == Boolean) { //boolean
		bool* v = static_cast<bool*>(default_value);
		delete v;
	} else if (type == String || type == Text) {
		std::string* v = static_cast<std::string*>(default_value);
		delete v;
	}
}

bool Attribute::is (Attr_Types t) {
	return type == t;
}