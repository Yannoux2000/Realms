#pragma once

#include "../../Base/Allocators/Allocator.h"
#include "../../Base/Logging/ILogged.h"


namespace rlms{
	class WorldManagerImpl;

	class WorldManager {
	private:
		static std::unique_ptr<WorldManagerImpl> instance;
	public:
		static int n_errors;

		static std::shared_ptr<rlms::LoggerHandler> GetLogger ();
		static bool Initialize (Allocator* const& alloc, size_t chunk_pool_size, std::shared_ptr<Logger> funnel = nullptr);
		static void Terminate ();

	};
}

