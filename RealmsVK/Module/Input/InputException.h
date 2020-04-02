#pragma once
#include "../../Base/Exception.h"

namespace rlms {
	class InputException : public Exception {
	public:
		InputException (std::string t_type) noexcept : Exception (t_type) {};
		InputException (std::string t_type, std::string t_description) noexcept : Exception (t_type, t_description) {};
	};

	struct InvalidMapName : public InputException {
		InvalidMapName () noexcept : InputException ("Invalide Map Name", "Game tried to querry a non existant map name !") {}
	};

	struct InvalidInputName : public InputException {
		InvalidInputName () noexcept : InputException ("Invalide Input Name", "Game tried to querry a non existant input name !") {}
	};

	struct InvalidFormat : public InputException {
		InvalidFormat () noexcept : InputException ("Invalid map name.", "all input name must be formated as {MapName}::{ActionName}.") {};
	};

	struct UnbindedInput : public InputException {
		UnbindedInput () noexcept : InputException ("Unbinded input.", "Action has not been binded to an input.") {};
	};
}
