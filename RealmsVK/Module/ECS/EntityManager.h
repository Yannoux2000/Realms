#pragma once

#include "Entity.h"
#include "../../Base/Allocators/Allocator.h"
#include "../../Base/Logging/ILogged.h"

#include <map>
#include <memory>

namespace rlms {
	class EntityManagerImpl;

	class EntityManager {
	private:
		static std::unique_ptr<EntityManagerImpl> instance;

	public:
		static int n_errors;

		static std::shared_ptr<LoggerHandler> GetLogger ();

		static void Initialize (Allocator* const& alloc, size_t entity_pool_size, std::shared_ptr<Logger> funnel = nullptr);
		static void Terminate ();

		static inline bool isValid (ENTITY_ID id) {
			return id != Entity::NULL_ID;
		}

		static const ENTITY_ID Create ();
		static const ENTITY_ID Create (ENTITY_ID id);
		static Entity* Get (ENTITY_ID id);
		static bool Has (ENTITY_ID id);
		static void Destroy (ENTITY_ID id);
	};
}
