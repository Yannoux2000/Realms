#pragma once
#include <string>
#include <sstream>

namespace rlms {
	struct Version {
		unsigned short major = 0;
		unsigned short minor = 0;
		unsigned long revision = 0;
		std::string build = "";

		std::string const toString () const;
	};

	struct VersionConstraint {
		enum VersionConstraintType {
			never, always, exact, above, between
		};
		Version min_version;
		Version max_version;
		VersionConstraintType constraint = never;

		bool is_valid (Version const& v) const;
	};
}
