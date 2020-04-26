#pragma once

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include "../../CoreTypes.h"
#include "../../Base/IBase.h"
#include "IComponent.h"

#include <map>
#include <string>
#include <vector>
#include <typeindex>

namespace rlms {
	////////////////////////////////////////////////////////////
	/// \brief Class to represent in game objects that are
	///        not grid based.
	///
	////////////////////////////////////////////////////////////
	class Entity : public IBase {
	private:

		////////////////////////////////////////////////////////////
		// Member data
		////////////////////////////////////////////////////////////
		std::string type; ///< name for this type of entity
		std::map<COMPONENT_TYPE_ID, IComponent*> _components; ///< internal references to components, the map forces all components to be unique
		ENTITY_ID _id;	///< internal id of this entity
	public:

		////////////////////////////////////////////////////////////
		/// \brief Entity constructor
		///
		/// This constructor is used by the Entity manager
		///
		/// \param id	this entity id
		///
		////////////////////////////////////////////////////////////
		Entity (ENTITY_ID const& id) : _id (id), _components ({}) {}

		////////////////////////////////////////////////////////////
		/// \brief Entity's id getter
		///
		/// \return Entity's id
		///
		////////////////////////////////////////////////////////////
		ENTITY_ID const id () const {
			return _id;
		}

		////////////////////////////////////////////////////////////
		/// \brief add a Component to entity's references
		///
		/// \template C	the component type to be added
		///
		/// \param C*	a reference to the component's instance
		///
		////////////////////////////////////////////////////////////
		void add (IComponent* c);

		////////////////////////////////////////////////////////////
		/// \brief check if this entity has a C type Component reference
		///
		/// \template C	the component type to be found
		///
		/// \return true if found ,false otherwise
		///
		////////////////////////////////////////////////////////////
		bool has (COMPONENT_TYPE_ID const& c_type_id);
		
		////////////////////////////////////////////////////////////
		/// \brief get the Component from the entity's references
		///
		/// hasComponent should be called before this function's call
		///
		/// \template C	the component type to be found
		///
		/// \return C*	a reference to the component's instance, returns a nullptr otherwise !
		///
		////////////////////////////////////////////////////////////
		IComponent* get (COMPONENT_TYPE_ID const& c_type_id);

		////////////////////////////////////////////////////////////
		/// \brief remove the Component from the entity's references
		///
		/// \template C	the component type to be found
		///
		////////////////////////////////////////////////////////////
		void rem (COMPONENT_TYPE_ID const& c_type_id);

		////////////////////////////////////////////////////////////
		/// \brief get all the components attached to this entity
		///
		/// \return std::vector<IComponent*> of all the components attached
		///
		////////////////////////////////////////////////////////////
		std::vector<IComponent*> getComponents ();

		////////////////////////////////////////////////////////////
		/// \brief remove the reference if equals the param
		///
		/// \param an address to the component (if it's same, the component pointed erased from the map)
		///
		////////////////////////////////////////////////////////////
		void rem (IComponent*& comp_ptr);

		////////////////////////////////////////////////////////////
		/// \brief getter & setter for this entity's type
		///
		/// \param a new type to be given
		///
		////////////////////////////////////////////////////////////
		void setType (const std::string& type) {
			this->type = type.substr(0,32);
		}

		std::string getType () const {
			return type;
		}

		////////////////////////////////////////////////////////////
		// Static member data
		////////////////////////////////////////////////////////////
		static constexpr ENTITY_ID NULL_ID = 0; ///< Define an null id to manage invalid entities

	};
} //namespace rlms

////////////////////////////////////////////////////////////
/// \class Entity
/// \ingroup RealmsCore
///
/// Entity provides an interface for components to be 
/// attached onto and be managed dynamicaly.
/// 
/// This class allows mods and game Developper to attach
/// relevent data, to then be managed with dedicated systems.
///
/// Usage example:
/// \code
/// Entity e = EntityManager::Get (e_id);
/// if(e.hasComponent<HealthComponent>()){
/// 	auto hps = e.getComponent<HealthComponent> ();
/// 	hps.cur_hp += reg_hp;
/// }
/// \endcode
/// ! This code would only be present in a related ISystem inherited class.
///
/// \see IComponent, ISystem, ECS_Core
///
////////////////////////////////////////////////////////////
