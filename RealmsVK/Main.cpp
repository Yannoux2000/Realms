//#include "RealmsVK.h"
//#include "VKExemple.h"
//#include "RealmApplication.h"
#include "_MemLeakMonitor.h"
#include "lua.hpp"
#include <iostream>

int main () {
	MemCheck ();

	//rlms::RealmApplication app;
	//rlms::RealmApplication::ApplicationSettings settings;
	
	//try {
	//	app.start (settings);
	//} catch (const std::exception& e) {
	//	std::cerr << e.what () << std::endl;
	//	return EXIT_FAILURE;
	//}

	//try {
	//	app.run ();
	//} catch (const std::exception& e) {
	//	std::cerr << e.what () << std::endl;
	//}

	//try {
	//	app.stop ();
	//} catch (const std::exception& e) {
	//	std::cerr << e.what () << std::endl;
	//	return EXIT_FAILURE;
	//}

	lua_State* L = luaL_newstate ();

	luaL_dofile (L, "");
	luaL_dostring (L,
		R"(ghost = {
					TransformComponent = {
						phrase = "I'M A SCARY GHOST!!!"
					}
				}
		)");

	lua_getglobal (L, "ghost");
	if (lua_istable (L, -1)) {
		std::cout << "ghost is table" << std::endl;
	}
	lua_getfield (L, -1, "TransformComponent");
	if (lua_istable (L, -1)) {
		std::cout << "TransformComponent is table" << std::endl;
	}
	lua_getfield (L, -1, "phrase");
	if (lua_isstring (L, -1)) {
		std::string phrase (lua_tostring (L, -1));
		std::cout << "phrase is "<< phrase << std::endl;
	}
	lua_close (L);

	return EXIT_SUCCESS;
}
