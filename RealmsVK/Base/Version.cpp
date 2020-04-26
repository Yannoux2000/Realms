#include "Version.h"

using namespace rlms;

std::string const Version::toString () const {
	std::ostringstream ss;
	ss << "{ " << major << "." << minor << "." << revision;
	if (!build.empty ()) {
		ss << ", " << build;
	}
	ss << "}";
	return ss.str ();
}

bool VersionConstraint::is_valid (Version const& v) const {
	switch (constraint) {
		case never:
			return false;

		case always:
			return true;

		case exact:
			return (v.major == min_version.major)
				&& (v.minor == min_version.minor)
				&& (v.revision == min_version.revision);

		case above:
			return (v.major > min_version.major)
				|| (v.major == min_version.major && v.minor > min_version.minor)
				|| (v.major == min_version.major && v.minor == min_version.minor && v.revision >= min_version.revision);

		case between:
			bool valid = (v.major > min_version.major)
				|| (v.major == min_version.major && v.minor > min_version.minor)
				|| (v.major == min_version.major && v.minor == min_version.minor && v.revision >= min_version.revision);

			valid &= (v.major < max_version.major)
				|| (v.major == max_version.major && v.minor < max_version.minor)
				|| (v.major == max_version.major && v.minor == max_version.minor && v.revision <= max_version.revision);
			return valid;
	}
}
