#pragma once

#include "../../Base/Version.h"
#include <string>
#include <vector>

namespace rlms {
	struct Dependency {
		std::string name;
		VersionConstraint constraint;
	};

	struct ModContainer {
		////////////////////////////////////////////////////////////
		// member data
		////////////////////////////////////////////////////////////
		std::string path = "";

		// exported from info.lua
		std::string name = "undefined";
		std::string title = "Undefined";
		Version version;
		std::string author = "anonymous";
		std::string contact = "";
		Version realms_version;
		std::vector<Dependency> dependencies;
		std::string description = "This mod has no description";

		////////////////////////////////////////////////////////////
		/// \brief check if this mod is valid for a foreign Dependency
		///
		/// \return Logger reference
		///
		////////////////////////////////////////////////////////////
		bool is_usable_for (Dependency const& d) {
			return (
				(name.compare (d.name) != 0) // name match
				&& d.constraint.is_valid (version) // version match
				);
		}
	};
}
