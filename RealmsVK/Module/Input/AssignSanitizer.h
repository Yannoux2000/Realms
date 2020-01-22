#pragma once
#include <string>
#include <regex>

namespace rlms{
	class AssignSanitizer {
	private:
		static const std::smatch GetMatch (std::string& name);
	public:
		static std::string NameSanitizer (std::string name);
		static bool IsGlobal (std::string name);
		static std::string GetMapName (std::string name);
		static std::string GetInputName (std::string name);
	};
}
