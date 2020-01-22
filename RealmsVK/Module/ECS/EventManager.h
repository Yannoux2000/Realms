#pragma once

#include "IO/ILogged.h"
#include "CoreTypes.h"
#include "IEvent.h"
#include "Memory/LinearAllocator.h"

#include <typeinfo>
#include <type_traits>
#include <map>
#include <memory>
namespace rlms{
	class EventManagerImpl;

	class EventManager {
	public:
		static int n_errors;

		static std::shared_ptr<LoggerHandler> GetLogger ();

		static bool Initialize (Allocator* const& alloc, size_t event_pool_size, std::shared_ptr<Logger> funnel = nullptr);
		static void Terminate ();

		template<class E> static bool CreateEvent ();
		template<class E> static E* GetEvent ();
		template<class E> static bool HasEvent ();
		static void ClearEvents ();
	private:
		static std::unique_ptr<EventManagerImpl> instance;
	};

	class EventManagerImpl : public ILogged {
	private:
		friend class EventManager;

		std::string getLogName () override {
			return "EventManager";
		};

		std::map<const std::type_info*, IEvent*> _events;
		std::unique_ptr<LinearAllocator> _event_Allocator;

		bool start (Allocator* const& alloc, size_t event_pool_size, std::shared_ptr<Logger> funnel);
		void stop ();

		template<class E> bool createEvent ();
		template<class E> E* getEvent ();
		template<class E> bool hasEvent ();
		void clearEvents ();
	public:
		EventManagerImpl ();
		~EventManagerImpl ();
	};

#include "EventManager.inl"
}