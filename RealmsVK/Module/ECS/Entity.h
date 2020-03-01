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
		std::map<std::type_index, IComponent*> _components; ///< internal references to components, the map forces all components to be unique
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
		template<class C> void add (C* c);

		////////////////////////////////////////////////////////////
		/// \brief check if this entity has a C type Component reference
		///
		/// \template C	the component type to be found
		///
		/// \return true if found ,false otherwise
		///
		////////////////////////////////////////////////////////////
		template<class C> bool has ();
		
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
		template<class C> C* get ();

		////////////////////////////////////////////////////////////
		/// \brief remove the Component from the entity's references
		///
		/// \template C	the component type to be found
		///
		////////////////////////////////////////////////////////////
		template<class C> void rem ();

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

#include "Entity.inl"

////////////////////////////////////////////////////////////
/// \class rlms::Entity
/// \ingroup RealmsCore
///
/// rlms::Entity provides an interface for components to be 
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
/// \see rlms::IComponent, rlms::ISystem, rlms::GameCore
///
////////////////////////////////////////////////////////////
