//#include "RealmsVK.h"
//#include "VKExemple.h"
#include "RealmApplication.h"
//#include "RealmApplicationMT.h"
#include "_MemLeakMonitor.h"
#include "lua.hpp"
#include <iostream>

using namespace rlms;

int lua_tests ();
//int application_test();

int application_test () {
	RealmApplication app;
	RealmApplication::ApplicationSettings settings;

	try {
		app.start (settings);
	} catch (const std::exception& e) {
		std::cerr << e.what () << std::endl;
		return EXIT_FAILURE;
	}

	try {
		app.run ();
	} catch (const std::exception& e) {
		std::cerr << e.what () << std::endl;
	}

	try {
		app.stop ();
	} catch (const std::exception& e) {
		std::cerr << e.what () << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

#include "Module/LuaBindings/Bindings_EntityManager.h"
#include "Base/Allocators/MasqueradeAllocator.h"

#include <iostream>

int checkbindings () {
	void* mem = malloc (4096);
	MasqueradeAllocator alloc (4096, mem);
	EntityManager::Initialize (&alloc, 4096);
	lua_State* L = luaL_newstate ();

	Bindings_EntityManager::Bind (L);

	auto e_id = EntityManager::Create ();
	auto e = EntityManager::Get (e_id);
	EntityManager::Destroy (e_id);

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
	EntityManager::Terminate ();
	free (mem);
	return 0;
}
static void stackDump (lua_State* L) {
	int i;
	int top = lua_gettop (L);
	for (i = -1; i >= -top; i--) {  /* repeat for each level */
		int t_start = lua_type (L, i);

		std::cout << "( " << i << " = ";
		switch (t_start) {
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

	Entity* e = new Entity (5);
	IComponent* c = nullptr;
	if (IBase::is <IComponent> ((IBase*)e)) {
		c = IBase::to <IComponent>((IBase*)e);
	} else {
		std::cout << "pute";
	}

	//c->id ();
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

#include "Module/LuaBindings/LuaComponent.h"

int testingLuaComponentPrototype () {

	void* mem = malloc (4096);
	MasqueradeAllocator alloc = MasqueradeAllocator (4096, mem);
	lua_State* L = luaL_newstate ();

	int i = luaL_dostring (L, R"(
Prototypes = {}
Prototypes["StatsComponent"] = {
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
	}

Prototypes["GaletteSaucice"] = {
		unChamp = "je sais pas mdr"
	}
)");

	if (i != LUA_OK) {
		std::string b = lua_tostring (L, -1);
		std::cout << b;
		return -1;
	}

	lua_getglobal (L, "Prototypes");
	lua_pushnil (L);
	while (lua_next (L, -2) != 0) {

		std::string name = lua_tostring (L, -2);
		if (lua_istable (L, -1)) {
			LuaComponentPrototype proto (L, name);
			std::cout << proto.debug () << "\n";

			IComponent* c = proto.Create (&alloc, 50, 1);
			proto.Destroy (&alloc, c);

			c = proto.Create (&alloc, 200, 20);
			std::cout << proto.debug (c) << "\n";
		} else {
			std::cout << "nope c'est pas bon ça\n";
		}

		std::cout << "\n";

		lua_pop (L, 1);
	}

	lua_close (L);
	free (mem);
	return 0;
}

#include "Utility/MultiThreading/JobSystem.h"
#include <future>

int test_scheduler () {
	JobSystem::Initialize ();

	Job jba1 ([]() { std::cout << "(s)"; }, 101);
	Job jbb1 ([]() { std::cout << "(a)"; }, 102);
	Job jbc1 ([]() { std::cout << "(l)"; }, 103);
	Job jbd1 ([]() { std::cout << "(u)"; }, 104);
	Job jbe1 ([]() { std::cout << "(t_start)"; }, 105);

	Job jba2 ([]() { std::cout << "[s]"; }, 101);
	Job jbb2 ([]() { std::cout << "[a]"; }, 102);
	Job jbc2 ([]() { std::cout << "[l]"; }, 103);
	Job jbd2 ([]() { std::cout << "[u]"; }, 104);
	Job jbe2 ([]() { std::cout << "[t_start]"; }, 105);

	Job jba3 ([]() { std::cout << "{s}"; }, 101);
	Job jbb3 ([]() { std::cout << "{a}"; }, 102);
	Job jbc3 ([]() { std::cout << "{l}"; }, 103);
	Job jbd3 ([]() { std::cout << "{u}"; }, 104);
	Job jbe3 ([]() { std::cout << "{t_start}"; }, 105);

	Job jba4 ([]() { std::cout << "\n"; }, 101);
	Job jbb4 ([]() { std::cout << "\n"; }, 102);
	Job jbc4 ([]() { std::cout << "\n"; }, 103);
	Job jbd4 ([]() { std::cout << "\n"; }, 104);
	Job jbe4 ([]() { std::cout << "\n"; }, 105);


	Job finaljob ([]() { JobSystem::FreeMainThread (); }, 200);

	JobSystem::Register (std::move (jba1));
	JobSystem::Register (std::move (jbb1));
	JobSystem::Register (std::move (jbc1));
	JobSystem::Register (std::move (jbd1));
	JobSystem::Register (std::move (jbe1));

	JobSystem::Register (std::move (jba2));
	JobSystem::Register (std::move (jbb2));
	JobSystem::Register (std::move (jbc2));
	JobSystem::Register (std::move (jbd2));
	JobSystem::Register (std::move (jbe2));

	JobSystem::Register (std::move (jba3));
	JobSystem::Register (std::move (jbb3));
	JobSystem::Register (std::move (jbc3));
	JobSystem::Register (std::move (jbd3));
	JobSystem::Register (std::move (jbe3));

	JobSystem::Register (std::move (jba4));
	JobSystem::Register (std::move (jbb4));
	JobSystem::Register (std::move (jbc4));
	JobSystem::Register (std::move (jbd4));
	JobSystem::Register (std::move (jbe4));

	JobSystem::Register (std::move (finaljob));

	JobSystem::WakeUp ();

	JobSystem::CaptureMainWorker ();

	//while (!JobSystem::IsBusy ());
	JobSystem::Terminate ();

	return 0;
}

#include "Utility/MultiThreading/PeriodicJob.h"

int test_applicationtime () {
	auto a = ApplicationTime::Now ();

	while (ApplicationTime::Now () < a) {
		std::cout << "wtf ça déconne en faite\n";
	}

	auto b = a + std::chrono::seconds (3);
	while (!(ApplicationTime::Now () > b)) {
		std::cout << "echeance non atteinte...\n";
	}

	return 0;
}

int test_scheduler2 () {

	ApplicationTime::Initialize ();
	JobSystem::Initialize ();

	std::atomic_bool stop = false;

	Job jb1 ([]() { std::cout << "(=)"; }, 101);
	PeriodicJob<std::ratio<1, 1>> jba1 ([]() { std::cout << "(Display)"; }, stop);
	PeriodicJob<std::ratio<100, 120>> jba2 ([]() { std::cout << "(Update)"; }, stop);
	PeriodicJob<std::ratio<100, 120>> jba3 ([]() { std::cout << "(Inputs)\n"; }, stop);

	JobSystem::Register (std::move (jb1));
	JobSystem::Register (std::move (jba1));
	JobSystem::Register (std::move (jba2));
	JobSystem::Register (std::move (jba3));

	JobSystem::WakeUp ();

	while (ApplicationTime::Since () < std::chrono::seconds (3)) {
		JobSystem::MainWorker ();
	}

	stop.store (true);

	JobSystem::Terminate ();
	return 0;
}

int test_scheduler3 () {
	ApplicationTime::Initialize ();
	JobSystem::Initialize ();

	std::atomic_bool stop_a = false;
	std::atomic_bool stop_b = false;

	JobSystem::Execute (std::move (Job ([&stop_a, &stop_b]() {
		while (!stop_a || !stop_b);
		std::cout << "salut" << std::endl;
		}, 200)));

	while (ApplicationTime::Since () < std::chrono::seconds (3)) { }

	JobSystem::Execute (std::move (Job ([&stop_a]() {
		std::cout << "wow a" << std::endl;
		stop_a.store (true);
		}, 100)));

	while (ApplicationTime::Since () < std::chrono::seconds (6)) { }

	JobSystem::Execute (std::move (Job ([&stop_b]() {
		std::cout << "wow b" << std::endl;
		stop_b.store (true);
		}, 100)));

	std::cout << "timed out ?" << std::endl;

	JobSystem::Terminate ();
	return 0;
}

#include "fmod/fmod.hpp"
#include "fmod/fmod_errors.h" // Only if you want error checking

int test_fmod () {
	FMOD::System* m_pSystem;

	if (FMOD::System_Create (&m_pSystem) != FMOD_OK) {
   // Report Error
		return 1;
	}

	int driverCount = 0;
	m_pSystem->getNumDrivers (&driverCount);

	if (driverCount == 0) {
	   // Report Error
		return 2;
	}

	// Initialize our Instance with 36 Channels
	m_pSystem->init (36, FMOD_INIT_NORMAL, NULL);
	return 0;
}

int test_glfw () {
	GLFWwindow* window = NULL;

	if (!glfwInit ()) {
		return 0;
	}

	glfwTerminate ();
	return 0;
}

int main () {
	MemCheck ();
	return application_test ();
}

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
			std::cout << "ghost doesn't_start exists ???" << std::endl;
			lua_close (L);
			return EXIT_FAILURE;
		}
	}

	lua_getglobal (L2, "ghost");
	if (lua_istable (L2, -1)) {
		std::cout << "ghost is table" << std::endl;
	} else {
		if (lua_isnil (L2, -1)) {
			std::cout << "ghost doesn't_start exists ???" << std::endl;
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
