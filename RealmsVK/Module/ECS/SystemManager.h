#pragma once

#include "../../Base/Logging/ILogged.h"
#include "../../CoreTypes.h"
#include "ISystem.h"
#include "../../Base/Allocators/MasqueradeAllocator.h"

#include <typeinfo>
#include <type_traits>
#include <map>
#include <memory>

namespace rlms{
	class SystemManagerImpl;

	class SystemManager {
	public:
		static int n_errors;

		static std::shared_ptr<LoggerHandler> GetLogger ();

		static bool Initialize (Allocator* const& alloc, size_t system_pool_size, std::shared_ptr<Logger> funnel = nullptr);
		static void Terminate ();

		//static void Update (GAME_TICK_TYPE dt);

		template<class S> static bool CreateSystem ();
		template<class S> static S* GetSystem ();
		template<class S> static bool HasSystem ();
		template<class S> static void DestroySystem ();

	private:
		static std::unique_ptr<SystemManagerImpl> instance;
	};


	class SystemManagerImpl : public ILogged {
	private:
		friend class SystemManager;

		std::string getLogName () override {
			return "SystemManager";
		};

		using alloc_type = MasqueradeAllocator;

		std::map<const std::type_info*, ISystem*> _systems;
		std::unique_ptr<alloc_type> m_object_Allocator;

		bool start (Allocator* const& alloc, size_t system_pool_size, std::shared_ptr<Logger> funnel);
		void stop ();

		//void  update (GAME_TICK_TYPE dt);

		template<class S> bool createSystem ();
		template<class S> S* getSystem ();
		template<class S> bool hasSystem ();
		template<class S> void destroySystem ();
	public:
		SystemManagerImpl ();
		~SystemManagerImpl ();
	};

#include "SystemManager.inl"
}