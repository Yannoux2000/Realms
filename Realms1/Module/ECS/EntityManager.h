#pragma once
/*
#include "RealmsCore/Entity.h"
#include "Memory/PoolAllocator.h"
#include "Memory/FreeListAllocator.h"
#include "IO/ILogged.h"

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

		static bool Initialize (Allocator* const& alloc, size_t entity_pool_size, std::shared_ptr<Logger> funnel = nullptr);
		static void Terminate ();

		static inline bool isValid (ENTITY_ID id) {
			return id != Entity::NULL_ID;
		}

		static const ENTITY_ID CreateEntity ();
		static const ENTITY_ID CreateEntity (ENTITY_ID id);
		static Entity* GetEntity (ENTITY_ID id);
		static bool HasEntity (ENTITY_ID id);
		static void DestroyEntity (ENTITY_ID id);
	};
}
*/