#pragma once

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

//#include "../../Base/Allocators/FreeListAllocator.h"
#include "../../Base/Allocators/MasqueradeAllocator.h"
#include "../../Base/Logging/ILogged.h"
#include "EntityManager.h"
#include "IComponent.h"
#include "Entity.h"

#include <map>
#include <memory>

namespace rlms {
	////////////////////////////////////////////////////////////
	/// \brief PImpl (check out https://en.cppreference.com/w/cpp/language/pimpl)
	///
	////////////////////////////////////////////////////////////
	class ComponentManagerImpl;
	////////////////////////////////////////////////////////////
	/// \brief Class to represent in game objects that are
	///        not grid based.
	///
	////////////////////////////////////////////////////////////
	class ComponentManager : public ILogged {
	private:
		////////////////////////////////////////////////////////////
		// Member data
		////////////////////////////////////////////////////////////
		static std::unique_ptr<ComponentManagerImpl> instance; ///< PImpl (check out https://en.cppreference.com/w/cpp/language/pimpl)

	public:
		////////////////////////////////////////////////////////////
		// Static member data
		////////////////////////////////////////////////////////////
		static int n_errors; ///< Public simple var to check how many errors happenned.

		////////////////////////////////////////////////////////////
		/// \brief access to the Logger reference
		///
		/// \return Logger reference
		///
		////////////////////////////////////////////////////////////
		static std::shared_ptr<LoggerHandler> GetLogger ();

		////////////////////////////////////////////////////////////
		/// \brief Start the ComponentManager
		///
		/// \return bool saying rather or not the start was 
		///
		////////////////////////////////////////////////////////////
		static void Initialize (Allocator* const& alloc, size_t comp_pool_size, std::shared_ptr<Logger> funnel = nullptr);

		////////////////////////////////////////////////////////////
		/// \brief End the ComponentManager in the expected manner.
		///
		////////////////////////////////////////////////////////////
		static void Terminate ();

		static inline const bool isValid (COMPONENT_ID c_id) {
			return c_id != IComponent::NULL_ID;
		}

		template<class C> static const COMPONENT_ID CreateComponent ();
		template<class C> static const COMPONENT_ID CreateComponent (Entity* entity);
		template<class C> static const COMPONENT_ID CreateComponent (Entity* entity, COMPONENT_ID c_id);
		template<class C> static const COMPONENT_ID CreateComponent (COMPONENT_ID c_id);

		static const bool HasEntity (COMPONENT_ID c_id);
		template<class C> static const bool HasComponent (Entity* entity);
		template<class C> static const bool HasComponent (COMPONENT_ID c_id);
		static const bool HasComponent (COMPONENT_ID c_id);

		static const ENTITY_ID& GetEntity (COMPONENT_ID c_id);
		template<class C> static C* GetComponent (Entity* entity);
		template<class C> static C* GetComponent (COMPONENT_ID c_id);
		template<class C> static std::vector<C*> GetComponents ();
		static IComponent* GetComponent (COMPONENT_ID c_id);

		template<class C> static void DestroyComponent (Entity* entity);
		static void DestroyComponent (COMPONENT_ID c_id);
	};

	class ComponentManagerImpl : public ILogged {
	private:
		friend class ComponentManager;

		std::string getLogName () override {
			return "ComponentManager";
		};

		using alloc_type = MasqueradeAllocator;

		std::map<ENTITY_ID, IComponent*> _lookup_table;
		std::unique_ptr<alloc_type> m_comp_Allocator;

		void start (Allocator* const& alloc, size_t entity_pool_size, std::shared_ptr<Logger> funnel);
		void stop ();

		template<class C> const COMPONENT_ID createComponent ();
		template<class C> const COMPONENT_ID createComponent (Entity* entity);
		template<class C> const COMPONENT_ID createComponent (Entity* entity, COMPONENT_ID c_id);
		template<class C> const COMPONENT_ID createComponent (COMPONENT_ID c_id);

		const bool hasEntity (COMPONENT_ID c_id);
		template<class C> const bool hasComponent (Entity* entity);
		template<class C> const bool hasComponent (COMPONENT_ID c_id);
		const bool hasComponent (COMPONENT_ID c_id);

		const ENTITY_ID& getEntity (COMPONENT_ID const& c_id);
		template<class C> C* getComponent (Entity* entity);
		template<class C> C* getComponent (COMPONENT_ID const& c_id);
		template<class C> static std::vector<C*> getComponents ();
		IComponent* getComponent (COMPONENT_ID const& c_id);

		template<class C> void destroyComponent (Entity* entity);
		void destroyComponent (COMPONENT_ID c_id);

		COMPONENT_ID _id_iter;
		inline COMPONENT_ID procedural_id_iter () {
			return _id_iter++;
		}
	public:

		ComponentManagerImpl ();
		~ComponentManagerImpl ();
	};

#include "ComponentManager.inl"
}