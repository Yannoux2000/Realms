#include "lua.h"
#include "../../Base/Exception.h"

void LuaResultCheck (lua_State*&L, int res) {
	if (res != LUA_OK) {
		throw rlms::Exception ("Lua error", lua_tostring(L, -1));
	}
}
