#pragma once

#include "RealmsCore/EntityManager.h"
#include "RealmsCore/ComponentManager.h"
#include "RealmsCore/SystemManager.h"
#include "RealmsCore/EventManager.h"
#include "RealmsCore/IGameLoaderSystem.h"

#include <memory>


/*
/!\/!\/!\/!\/!\/!\/!\/!\/!\/!\/!\
DEPRECATED (USE GAMECORE INSTEAD)
/!\/!\/!\/!\/!\/!\/!\/!\/!\/!\/!\
*/

namespace rlms{
	class ECSCoreImpl;

	class ECSCore : public ILogged {
	protected:
		std::string getLogName () { return "ECS"; }
	public:
		static const double atomic_tick_time () {
			return m_atomic_tick_time;
		}

		static void atomic_tick_time (double new_atomic_tick) {
			if(new_atomic_tick > 0){
				m_atomic_tick_time = new_atomic_tick;
			}
		}

		static std::shared_ptr<LoggerHandler> GetLogger ();
		static int n_error;

		static unsigned long long game_mem_alloc_size;
		static unsigned long long entity_mem_alloc_size;
		static unsigned long long component_mem_alloc_size;
		static unsigned long long system_mem_alloc_size;
		static unsigned long long event_mem_alloc_size;

		static void Mem_alloc_sizes (unsigned long long ecs, unsigned long long entity, unsigned long long component, unsigned long long system, unsigned long long events) {
			game_mem_alloc_size = ecs;
			entity_mem_alloc_size = entity;
			component_mem_alloc_size = component;
			system_mem_alloc_size = system;
			event_mem_alloc_size = events;
		}

		static bool Initialize (std::shared_ptr<Logger> funnel = nullptr);

		static void PreUpdate (GAME_TICK_TYPE current_tick);
		static void Update (GAME_TICK_TYPE _current_tick);
		static void PostUpdate (GAME_TICK_TYPE current_tick);

		static void Terminate ();

		ECSCore ();
		~ECSCore ();

	private:
		static std::unique_ptr<ECSCore> instance;
		static double m_atomic_tick_time;

		IGameLoaderSystem* _loader_system;
		std::unique_ptr<FreeListAllocator> m_global_allocator;

		bool start (std::shared_ptr<Logger> funnel);

		void setLoaderSystem (IGameLoaderSystem* loader);
		void load ();
		void save ();

		void preUpdate (GAME_TICK_TYPE _current_tick);
		void update (GAME_TICK_TYPE _current_tick);
		void postUpdate (GAME_TICK_TYPE _current_tick);

		void terminate ();
	};
}
