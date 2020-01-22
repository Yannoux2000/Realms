#include "ModLoader.h"

#include <vector>

using namespace rlms;

std::unique_ptr<ModLoaderImpl> ModLoader::instance;

class rlms::ModLoaderImpl : public ILogged {
private:
	static std::vector<HMODULE> DLLs;
public:

	ModLoaderImpl () {};
	~ModLoaderImpl () {};

	bool loadMod (std::string Ldll_filename) {
		logger->tag (LogTags::Info) << "Loading DLL :\"" << Ldll_filename << "\"" << '\n';
		//auto dll_filename = Ldll_filename;
		HMODULE LDLL = LoadLibraryA ((LPCSTR)Ldll_filename.c_str ());
		void (*_pfunc)();

		if (LDLL) {
			logger->tag (LogTags::None) << "DLL Loaded\n";

			_pfunc = (void(*)()) GetProcAddress ((HMODULE)LDLL, "LoadTester");
			if (_pfunc) {
				_pfunc ();
				return true;
			}

			_pfunc = (void(*)()) GetProcAddress ((HMODULE)LDLL, (LPCSTR)TEXT ("void IModLoader::LoadTester(void)"));
			if (_pfunc) {
				_pfunc ();
				return true;
			}
			return false;

		} else {
			return false;
		}
		return false;
	}
};
std::vector<HMODULE> rlms::ModLoaderImpl::DLLs;

bool ModLoader::Intialize () {
	instance = std::make_unique<ModLoaderImpl> ();
	return true;
}

void ModLoader::Terminate () {
	instance.reset ();
}

bool ModLoader::LoadMod (std::string Ldll_filename) {
	return instance->loadMod(Ldll_filename);
}
