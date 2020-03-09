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
	/// \brief Interface for attatching data onto entities
	///        according to the ECS Pattern
	///
	////////////////////////////////////////////////////////////
	class IComponentPrototype;

	struct IComponent : public IBase {
	private:
		////////////////////////////////////////////////////////////
		// Member data
		////////////////////////////////////////////////////////////
		friend IComponentPrototype;

		COMPONENT_ID c_id; ///< internal id of this component
	public:
		static IComponentPrototype* c_proto;
		ENTITY_ID e_id; ///< internal reference of this component's entity, carried by id

		////////////////////////////////////////////////////////////
		/// \brief Default IComponent constructor
		///
		/// This constructor is needed for the Component manager
		///
		////////////////////////////////////////////////////////////
		IComponent () : e_id (), c_id () {}

		////////////////////////////////////////////////////////////
		/// \brief Construct from entity id and defined id
		///
		/// Component is generated with defined ID's
		///
		/// \param e_id		Entity's id to which the component is attached to (can be Entity::NULL_ID !)
		/// \param c_id		Component's id to allow Manager call
		///
		////////////////////////////////////////////////////////////
		IComponent (ENTITY_ID const& entity_id, COMPONENT_ID const& component_id)
			: e_id (entity_id), c_id (component_id) {}

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
		// Static member data
		////////////////////////////////////////////////////////////
		static constexpr COMPONENT_ID NULL_ID = 0; ///< Define an null id to manage invalid components
	};

	class IComponentPrototype {
	protected:
		std::string _type_name;

		size_t _size = sizeof (IComponent);
		uint8_t _align = alignof (IComponent);

		void* _Get (IComponent* const c, std::string& member) {
			if (std::regex_match (member, std::regex ("[(c_id)(component_id)(id)]"))) {
				return &c->c_id;
			}
			if (std::regex_match (member, std::regex ("[(e_id)(entity_id)]"))) {
				return &c->e_id;
			}

			throw BadMemberName ();
			return nullptr;
		}

	public:
		IComponentPrototype() : _type_name ("UNDEFINED"), _size(sizeof (IComponent)), _align(alignof (IComponent)) {}
		IComponentPrototype (std::string const name, size_t const size, uint8_t const align) : _type_name(name), _size(size), _align(align) {}

		virtual IComponent* Create (Allocator* const& alloc, ENTITY_ID const& entity_id, COMPONENT_ID const& component_id) {
			return new (alloc->allocate (_size, _align)) IComponent (entity_id, component_id);
		}

		virtual void* Get (IComponent* const c, std::string&& member) {
			return _Get (c, member);
		}

		void Destroy (Allocator* const& alloc, IComponent* const& c);
	};
} //namespace rlms

////////////////////////////////////////////////////////////
/// \class rlms::IComponent
/// \ingroup RealmsCore
///
/// rlms::IComponent provides an interface for data to be 
/// attached to entities and be managed dynamicaly.
/// 
/// This class allows mods and game Developper to attach
/// relevent data, to then be managed with dedicated systems.
///
/// Usage example:
/// \code
/// struct HealthComponent : public rlms::IComponent {
/// 	int max_hp;
/// 	int cur_hp;
/// 	int reg_hp;
/// }
/// \endcode
///
/// \see rlms::Entity, rlms::ISystem, rlms::ECS_Core
///
////////////////////////////////////////////////////////////
