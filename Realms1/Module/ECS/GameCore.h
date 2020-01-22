#pragma once

#include "IO/ILogged.h"

#include "Memory/FreeListAllocator.h"
#include "RealmsCore/IGameLoaderSystem.h"

#include "RealmsCore/EntityManager.h"
#include "RealmsCore/ComponentManager.h"
#include "RealmsCore/SystemManager.h"
#include "RealmsCore/EventManager.h"
#include "RealmsCore/WorldManager.h"

#include "RealmsCore/GameCoreSettings.h"

#include "CoreTypes.h"

namespace rlms{
	class GameCoreImpl;

	class GameCore : public ILogged {
	public:

		static std::shared_ptr<LoggerHandler> GetLogger ();
		static int n_error;

		static bool Initialize (std::shared_ptr<Logger> funnel = nullptr);

		static bool LoadSettings (std::string config_file);
		void setGameLoaderSystem (IGameLoaderSystem* loader);

		static void Update (double dt);

		static void Terminate ();
	private:
		static std::unique_ptr<GameCoreImpl> instance;
	};
}
