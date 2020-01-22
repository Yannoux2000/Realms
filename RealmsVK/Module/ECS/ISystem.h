#pragma once

#include "CoreTypes.h"

namespace rlms {
	class ISystem {
	public:
		ISystem () {};
		virtual ~ISystem () {};

		virtual void start () = 0;

		virtual void  preUpdate (GAME_TICK_TYPE dt) = 0;
		virtual void  update (GAME_TICK_TYPE dt) = 0;
		virtual void  postUpdate (GAME_TICK_TYPE dt) = 0;

		virtual void stop () = 0;
	};
}