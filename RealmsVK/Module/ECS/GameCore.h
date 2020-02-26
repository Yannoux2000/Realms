#pragma once

#include "../../Base/Allocators/IAllocator.h"
#include "../../Base/Logging/ILogged.h"

#include "EntityManager.h"
#include "ComponentManager.h"
//#include "SystemManager.h"
//#include "EventManager.h"

#include "GameCoreSettings.h"

#include "../../CoreTypes.h"

namespace rlms{
	class GameCoreImpl;

	class GameCore {
	public:

		static std::shared_ptr<LoggerHandler> GetLogger ();
		static int n_error;

		static void Initialize (IAllocator* const& alloc, size_t ecs_pool_size, std::shared_ptr<Logger> funnel = nullptr);

		static void Update (double dt);

		static void Terminate ();
	private:
		static std::unique_ptr<GameCoreImpl> instance;
	};
}
