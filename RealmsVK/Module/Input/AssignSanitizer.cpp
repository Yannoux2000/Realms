#include "AssignSanitizer.h"
#include <algorithm>

const std::smatch rlms::AssignSanitizer::GetMatch (std::string& name) {
	std::regex format{ "(\\w+)::(\\w+)" };
	std::smatch m;
	std::regex_match (name, m, format);
	return m;
}

std::string rlms::AssignSanitizer::NameSanitizer (std::string name) {
	name = std::regex_replace (name, std::regex ("\\s+"), "_");
	name = std::regex_replace (name, std::regex ("\\W+"), "");
	std::transform (name.begin (), name.end (), name.begin (), [](unsigned char c) { return std::tolower (c); }); // sets everycaracter to lowercase
	return name;
}

bool rlms::AssignSanitizer::IsGlobal (std::string name) {
	bool isGlobal = false;
	std::smatch m = GetMatch (name);
	return m.empty () && !NameSanitizer(name).empty();
}

std::string rlms::AssignSanitizer::GetMapName (std::string name) {
	std::smatch m = GetMatch (name);
	if (!m.empty ()) {
		return NameSanitizer (m[1].str ());
	}
	return "";
}

std::string rlms::AssignSanitizer::GetInputName (std::string name) {
	std::smatch m = GetMatch (name);
	if (!m.empty ()) {
		return NameSanitizer (m[2].str ());
	}
	return "";
}
