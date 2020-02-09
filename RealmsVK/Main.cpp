//#include "RealmsVK.h"
//#include "VKExemple.h"
#include "RealmApplication.h"
#include "_MemLeakMonitor.h"

int main () {
	MemCheck ();

	rlms::RealmApplication app;
	rlms::RealmApplication::ApplicationSettings settings;
	
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
