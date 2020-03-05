//#include "RealmsVK.h"
//#include "VKExemple.h"
//#include "RealmApplication.h"
#include "_MemLeakMonitor.h"
#include "lua.hpp"
#include <iostream>

int lua_tests ();
//int application_test();

#include "Module/LuaBindings/Bindings_EntityManager.h"
#include "Base/Allocators/MasqueradeAllocator.h"

#include <iostream>

int checkbindings () {
	void* mem = malloc (4096);
	rlms::MasqueradeAllocator alloc (4096, mem);
	rlms::EntityManager::Initialize (&alloc, 4096);
	lua_State* L = luaL_newstate ();

	rlms::Bindings_EntityManager::Bind (L);

	auto e_id = rlms::EntityManager::Create ();
	auto e = rlms::EntityManager::Get (e_id);
	rlms::EntityManager::Destroy (e_id);

	auto LuaPrint = [](lua_State* L) -> int {
		if (lua_isstring (L, -1)) {
			std::cout << lua_tostring (L, -1) << std::endl;
		} else std::cout << "failed to print" << std::endl;
		return 0;
	};

	lua_pushcfunction (L, LuaPrint);
	lua_setglobal (L, "Cprint");

	luaL_dostring (L, R"(
e_id = EntityManager.Create()
e = EntityManager.Get(e_id)
EntityManager.Destroy(e_id)
)");

	lua_close (L);
	rlms::EntityManager::Terminate ();
	free (mem);
	return 0;
}
static void stackDump (lua_State* L) {
	int i;
	int top = lua_gettop (L);
	for (i = -1; i >= -top; i--) {  /* repeat for each level */
		int t = lua_type (L, i);

		std::cout << "( " << i << " = ";
		switch (t) {
			case LUA_TSTRING:  /* strings */
				std::cout << lua_tostring (L, i);
				break;

			case LUA_TBOOLEAN:  /* booleans */
				std::cout << (lua_toboolean (L, i) ? "true" : "false");
				break;

			case LUA_TNUMBER:  /* numbers */
				std::cout << (lua_tonumber (L, i));
				break;

			default:  /* other values */
				std::cout << lua_typename (L, i);
				break;

		}
		std::cout << " ),";  /* put a separator */
	}
	std::cout << std::endl;  /* end the listing */

}

#include "Module/ECS/Entity.h"
#include "Module/ECS/IComponent.h"

int lua_statistics () {

	lua_State* L = luaL_newstate ();

	rlms::Entity* e = new rlms::Entity (5);
	rlms::IComponent* c;
	if (rlms::IBase::is <rlms::IComponent*> ((rlms::IBase*)e)) {
		c = rlms::IBase::to <rlms::IComponent*>((rlms::IBase*)e);
	} else {
		c = new rlms::IComponent (5, 2);
		std::cout << "pute";
	}

	c->id ();
	if (c != nullptr) delete c;
	delete e;

	stackDump (L);
	lua_pushinteger (L, 10);
	stackDump (L);
	lua_pushinteger (L, 25);
	stackDump (L);
	int a = lua_tointeger (L, -1);
	stackDump (L);
	lua_remove (L, -1);
	stackDump (L);

	lua_close (L);
	return 0;
}

#include "Module/LuaBindings/LuaComponentPrototype.h"

int testingLuaComponentPrototype () {

	lua_State* L = luaL_newstate ();

	int i = luaL_dostring (L, R"(
Prototypes = {
	StatsComponent = {
		health = 100,
		health_max = 100,
		health_regen = 1,
		
		mana = 100.0,
		mana_max = 100.0,
		mana_regen = 1.0,

		attack = true,
		defence = 0,
		says = "si tu chie plus fort que tu parle, va au wc pour me parler",
		name = "Jean Naymaloquoxis"
	},

	GaletteSaucice = {
		unChamp = "je sais pas mdr"
	}
}
)");

	if (i != LUA_OK) {
		std::string b = lua_tostring (L, -1);
		std::cout << b;
	}

	lua_getglobal (L, "a");
	if (lua_isinteger (L, -1)) {
	
		std::cout << "importation worked\n";
	}

	lua_getglobal (L, "Prototypes");
	lua_pushnil (L);
	while (lua_next (L, -2) != 0) {

		std::string name = lua_tostring (L, -2);
		if (lua_istable (L, -1)) {
			rlms::LuaComponentPrototype proto (L, name);
			std::cout << proto.debug ();
		} else {
			std::cout << "nope c'est pas bon ça\n";
		}

		lua_pop (L, 1);
	}

	

	lua_close (L);

	return 0;
}

int main () {
	MemCheck ();

	return testingLuaComponentPrototype ();

}

//int application_test () {
//	rlms::RealmApplication app;
//	rlms::RealmApplication::ApplicationSettings settings;
//
//	try {
//		app.start (settings);
//	} catch (const std::exception& e) {
//		std::cerr << e.what () << std::endl;
//		return EXIT_FAILURE;
//	}
//
//	try {
//		app.run ();
//	} catch (const std::exception& e) {
//		std::cerr << e.what () << std::endl;
//	}
//
//	try {
//		app.stop ();
//	} catch (const std::exception& e) {
//		std::cerr << e.what () << std::endl;
//		return EXIT_FAILURE;
//	}
//
//	return EXIT_SUCCESS;
//}

int lua_tests() {
	auto luaprint = [](lua_State* L) -> int {
		if (lua_isstring (L, -1)) {
			std::string phrase (lua_tostring (L, -1));
			std::cout << "Lua : " << phrase << std::endl;
			return 0;
		}
		std::cout << "Error: Not a string" << std::endl;
		return 1;
	};

	lua_State* L = luaL_newstate ();

	lua_pushcfunction (L, luaprint);
	lua_setglobal (L, "Cprint");

	auto ret = luaL_dostring (L,
		R"(
			prototypes = {
				SampleComponent = {
					health = number,
					mana = number
				}
			}
			
			ghost = {
					TransformComponent = {
						phrase = "I'M A SCARY GHOST!!!"
					}
				}
		)");

	if (ret != LUA_OK) {
		std::string phrase (lua_tostring (L, -1));
		std::cout << "LuaError : " << phrase << std::endl;
	}


	lua_State* L2 = luaL_newstate ();

	lua_pushcfunction (L2, luaprint);
	lua_setglobal (L2, "Cprint");

	ret = luaL_dostring (L2,
		R"(
			prototypes = {
				SampleComponent = {
					health = number,
					mana = number
				}
			}
			
			ghost = {
					TransformComponent = {
						phrase = "I'M An other SCARY GHOST!!!"
					}
				}
		)");

	if (ret != LUA_OK) {
		std::string phrase (lua_tostring (L2, -1));
		std::cout << "LuaError : " << phrase << std::endl;
	}

	lua_getglobal (L, "ghost");
	if (lua_istable (L, -1)) {
		std::cout << "ghost is table" << std::endl;
	} else {
		if (lua_isnil (L, -1)) {
			std::cout << "ghost doesn't exists ???" << std::endl;
			lua_close (L);
			return EXIT_FAILURE;
		}
	}

	lua_getglobal (L2, "ghost");
	if (lua_istable (L2, -1)) {
		std::cout << "ghost is table" << std::endl;
	} else {
		if (lua_isnil (L2, -1)) {
			std::cout << "ghost doesn't exists ???" << std::endl;
			lua_close (L2);
			return EXIT_FAILURE;
		}
	}

	lua_getfield (L, -1, "TransformComponent");
	if (lua_istable (L, -1)) {
		std::cout << "TransformComponent is table" << std::endl;
	}


	lua_getfield (L, -1, "phrase");
	if (lua_isstring (L, -1)) {
		std::string phrase (lua_tostring (L, -1));
		std::cout << "phrase is " << phrase << std::endl;
	}

	lua_getfield (L2, -1, "TransformComponent");
	if (lua_istable (L2, -1)) {
		std::cout << "TransformComponent is table" << std::endl;
	}


	lua_getfield (L2, -1, "phrase");
	if (lua_isstring (L2, -1)) {
		std::string phrase (lua_tostring (L2, -1));
		std::cout << "phrase is " << phrase << std::endl;
	}


	/*
	struct LuaComponent : public IComponent {
		// Data is given on the go with LuaComponentPrototype
		// Everything is registered in the LuaComponentPrototype
	}
	*/

	luaL_dostring (L2, "Cprint(ghost[\"TransformComponent\"][\"phrase\"])");
	//luaL_dostring (L2, "Cprint(ghost[\"TransformComponent\"])");

	luaL_dostring (L, "Cprint(ghost[\"TransformComponent\"][\"phrase\"])");
	//luaL_dostring (L, "Cprint(ghost[\"TransformComponent\"])");

	lua_close (L2);

	lua_close (L);
}
