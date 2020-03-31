#pragma once
#include "../../Base/Exception.h"

namespace rlms {
	class ECSException : public Exception {
	public:
		ECSException (std::string t_type) noexcept : Exception (t_type) {};
		ECSException (std::string t_type, std::string t_description) noexcept : Exception (t_type, t_description) {};
	};

	struct BadMemberName : public ECSException {
		BadMemberName () : ECSException ("Bad member name", "Make sure you enter a valid member name for this Data Structure") {};
	};

	struct BadComponentProto : public ECSException {
		BadComponentProto () : ECSException ("Bad prototype", "Invalid prototype was used for this component.") {};
	};
}
