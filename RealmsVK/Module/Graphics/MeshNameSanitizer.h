#pragma once

#include <string>

namespace rlms {
	class MeshNameSanitizer {
	public:
		static std::string Sanitize (std::string name);
		static std::string GetAlias (std::string filename);
		static std::string IncrementAlias (std::string name);
	};
}