#pragma once
#include "../../Base/Logging/ILogged.h"
#include "IDrawable.h"

#include <memory>

namespace rlms {
	class IRenderer {
	public:

		IRenderer () { }
		virtual ~IRenderer () { }

		virtual void start () {

		}

		virtual void draw () {

		}

		virtual void stop () {

		}
	};
}
