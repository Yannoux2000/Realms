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

		static const COMPONENT_ID CreateComponent (COMPONENT_TYPE_ID const& c_type, Entity* entity = nullptr);
		static const COMPONENT_ID CreateComponent (COMPONENT_TYPE_ID const& c_type, COMPONENT_ID c_id, Entity* entity = nullptr);

		static const bool HasEntity (COMPONENT_ID c_id);
		static const bool HasComponent (COMPONENT_ID c_id);

		static const ENTITY_ID& GetEntity (COMPONENT_ID c_id);
		static IComponent* GetComponent (COMPONENT_ID c_id);
		static std::vector<IComponent*> GetComponents (COMPONENT_TYPE_ID const& c_type);

		static void DestroyComponent (COMPONENT_TYPE_ID const& c_type, Entity* entity);
		static void DestroyComponent (COMPONENT_ID c_id);
	};
}