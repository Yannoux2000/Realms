#include "lua.h"
#include "../../Base/Exception.h"

using namespace rlms;

void LuaResultCheck (lua_State*&L, int res) {
	if (res != LUA_OK) {
		throw Exception ("Lua error", lua_tostring(L, -1));
	}
}
