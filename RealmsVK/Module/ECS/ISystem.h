#pragma once
#include "../../CoreTypes.h"

namespace rlms {
	class ISystem {
	public:
		ISystem () {};
		virtual ~ISystem () {};

		virtual void start () = 0;

		virtual void update () = 0;

		virtual void stop () = 0;
	};
}