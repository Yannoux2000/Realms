#pragma once


////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include "../../CoreTypes.h"
#include "../../Base/IBase.h"
#include "../../Base/Allocators/Allocator.h"
#include "ECSException.h"

#include <string>
#include <regex>

namespace rlms {

	////////////////////////////////////////////////////////////
	/// \brief Factory interface for Components
	///
	////////////////////////////////////////////////////////////
	class IComponentPrototype;

	////////////////////////////////////////////////////////////
	/// \brief Interface for attatching data onto entities
	///        according to the ECS Pattern
	///
	////////////////////////////////////////////////////////////
	struct IComponent : public IBase {
	private:
		friend IComponentPrototype;
		////////////////////////////////////////////////////////////
		// Member data
		////////////////////////////////////////////////////////////
		COMPONENT_ID c_id; ///< internal id of this component
		const COMPONENT_TYPE_ID _type_id;
	public:
		ENTITY_ID e_id; ///< internal reference of this component's entity, carried by id

		////////////////////////////////////////////////////////////
		/// \brief Construct from entity id and defined id
		///
		/// Component is generated with defined ID's
		///
		/// \param e_id		Entity's id to which the component is attached to (can be Entity::NULL_ID !)
		/// \param c_id		Component's id to allow Manager call
		///
		////////////////////////////////////////////////////////////
		IComponent (ENTITY_ID const& entity_id, COMPONENT_ID const& component_id, COMPONENT_TYPE_ID const& type_id)
			: e_id (entity_id), c_id (component_id), _type_id(type_id) {}

		////////////////////////////////////////////////////////////
		/// \brief virtual Destructor, apply inherited Destructor
		///
		////////////////////////////////////////////////////////////
		virtual ~IComponent () {};

		////////////////////////////////////////////////////////////
		/// \brief Component's id getter
		///
		/// \return Component's id
		///
		////////////////////////////////////////////////////////////
		COMPONENT_ID const& id () const {
			return c_id;
		}

		////////////////////////////////////////////////////////////
		/// \brief Component's type id getter
		///
		/// \return Component's type id
		///
		////////////////////////////////////////////////////////////
		COMPONENT_TYPE_ID const type_id () const {
			return _type_id;
		}

		////////////////////////////////////////////////////////////
		// Static member data
		////////////////////////////////////////////////////////////
		static constexpr COMPONENT_ID NULL_ID = 0; ///< Define an null id to manage invalid components
	};

	class IComponentPrototype {
	private:
		static COMPONENT_TYPE_ID _type_component_id_counter;
		COMPONENT_TYPE_ID _type_id;
	protected:
		std::string _type_name;
		size_t _size;
		uint8_t _align;

		virtual void Populate (IComponent* const& c) {

		}

		virtual void* GetAttrib (IComponent* const& c, std::string& member) {
			return nullptr;
		}

	public:
		IComponentPrototype() 
			: _type_name ("UNDEFINED"), _size(sizeof (IComponent)), _align(alignof (IComponent)),
			_type_id(_type_component_id_counter++) {}
		
		IComponentPrototype (std::string const name, size_t const size, uint8_t const align) 
			: _type_name(name), _size(size), _align(align),
			_type_id (_type_component_id_counter++) {}

		const std::string getTypeName ();
		const COMPONENT_TYPE_ID& getTypeId ();

		bool is (IComponent* const c);

		IComponent* Create (Allocator* const& alloc, ENTITY_ID const& entity_id, COMPONENT_ID const& component_id);
		void* Get (IComponent* const c, std::string&& member);
		void Destroy (Allocator* const& alloc, IComponent* const& c);

		template<typename T> T* Get (IComponent* const c, std::string&& member) {
			return static_cast<T*>(Get (c, std::move (member)));
		}

		template<typename T> void Set (IComponent* const c, std::string&& member, T& data) {
			T* addr = Get<T> (c, member);
			if (addr != nullptr) {
				*addr = data;
			}
		}
	};
} //namespace rlms

////////////////////////////////////////////////////////////
/// \class IComponent
/// \ingroup RealmsCore
///
/// IComponent provides an interface for data to be 
/// attached to entities and be managed dynamicaly.
/// 
/// This class allows mods and game Developper to attach
/// relevent data, to then be managed with dedicated systems.
///
/// Usage example:
/// \code
/// struct HealthComponent : public IComponent {
/// 	int max_hp;
/// 	int cur_hp;
/// 	int reg_hp;
/// }
/// \endcode
///
/// \see Entity, ISystem, ECS_Core
///
////////////////////////////////////////////////////////////
