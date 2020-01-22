#pragma once
#include "../../_Preprocess.h"
#include "../../Base/Logging/ILogged.h"

#ifndef USE_PRECOMPILED_HEADERS

#ifdef RLMS_PLATFORM_WIN
#include <direct.h>
#include <windows.h>
#elif RLMS_PLATFORM_LINUX
#include <sys/types.h>
#include <dlfcn.h>
#endif
#include <iostream>
#endif

#ifdef RLMS_PLATFORM_WIN
typedef HINSTANCE lib_handle;
#elif RLMS_PLATFORM_LINUX
typedef void* lib_handle;
#endif

// Where retType is the pointer to a return type of the function
// This return type can be int, float, double, etc or a struct or class.
/*
void test_load_lib () {


	// load the library -------------------------------------------------
#ifdef WINDOWS
	std::string nameOfLibToLoad ("C:\opt\lib\libctest.dll");
	lib_handle lib = LoadLibrary (nameOfLibToLoad.c_str ());
	if (!lib) {
		cerr << "Cannot load library: " << nameOfLibToLoad.c_str () << endl;
	}
#else
	string nameOfLibToLoad ("/opt/lib/libctest.so");
	lib_handle = dlopen (nameOfLibToLoad.c_str (), RTLD_LAZY);
	if (!lib_handle) {
		cerr << "Cannot load library: " << dlerror () << endl;
	}
#endif

	// load the symbols -------------------------------------------------
#ifdef WINDOWS
	void* func = (void*) GetProcAddress (lib, "superfunctionx");
	if (!func) {
		cerr << "Cannot load symbol superfunctionx: " << GetLastError () << endl;
	}
#else
	// reset errors
	dlerror ();

	// load the symbols (handle to function "superfunctionx")
	void* fn_handle = (void*)dlsym (lib_handle, "superfunctionx");
	const char* dlsym_error = dlerror ();
	if (dlsym_error) {
		cerr << "Cannot load symbol superfunctionx: " << dlsym_error << endl;
	}
#endif


	// unload the library -----------------------------------------------

#ifdef WINDOWS
	FreeLibrary (lib);
#else
	dlclose (lib);
#endif

}
*/

namespace rlms {
	class ModLoaderImpl;

	class ModLoader  {
	private:
		static std::unique_ptr<ModLoaderImpl> instance;
	public:

		static void loadTester () {
			std::cout << "Load Test validated" << std::endl;
		}

		bool Intialize ();
		void Terminate ();

		bool LoadMod (std::string Ldll_filename);

	};
}