#include "InputMap.h"

using namespace rlms;

void InputMap::bindKey (INPUT_ADDRESS_SUB_TYPE addr, int && key) {

	auto it = m_assignedInputs.find (addr);

	//Entity exists
	if (it != m_assignedInputs.end ()) {
		unbindInput (addr);
	}

	m_assignedInputs.insert (std::make_pair (addr, new KeyboardButtonInput (std::move (key))));
}

void InputMap::bindMouseButton (INPUT_ADDRESS_SUB_TYPE addr, int && button) {
	auto it = m_assignedInputs.find (addr);

	//Assign exists
	if (it != m_assignedInputs.end ()) {
		unbindInput (addr);
	}

	m_assignedInputs.insert (std::make_pair (addr, new MouseButtonInput (std::move (button))));
}

void InputMap::bindKeySlide (INPUT_ADDRESS_SUB_TYPE addr, int && key) {

	auto it = m_assignedInputs.find (addr);

	//Assign exists
	if (it != m_assignedInputs.end ()) {
		unbindInput (addr);
	}

	m_assignedInputs.insert (std::make_pair (addr, new KeyboardSlideInput (std::move (key))));
}

void InputMap::bindMouseButtonSlide (INPUT_ADDRESS_SUB_TYPE addr, int && button) {
	auto it = m_assignedInputs.find (addr);

	//Assign exists
	if (it != m_assignedInputs.end ()) {
		unbindInput (addr);
	}

	m_assignedInputs.insert (std::make_pair (addr, new MouseSlideInput (std::move (button))));
}

//void InputMap::bindEvent (std::string const& name, sf::Event::EventType && e) {
//	auto it = m_assignedInputs.find (name);
//
//	//Assign exists
//	if (it != m_assignedInputs.end ()) {
//		unbindInput (name);
//	}
//
//	m_assignedInputs.insert (std::make_pair (name, new EventAssignement (std::move (e))));
//}

void InputMap::unbindInput (INPUT_ADDRESS_SUB_TYPE addr) {
	auto it = m_assignedInputs.find (addr);

	//Assign exists
	if (it != m_assignedInputs.end ()) {
		it->second->~IInput ();
		delete it->second;
		m_assignedInputs.erase (addr);
	}
}

void InputMap::clearInputs () {
	while (!m_assignedInputs.empty ()) {
		auto it = m_assignedInputs.begin ();
		it->second->~IInput ();
		delete it->second;
		m_assignedInputs.erase (it);
	}
}

void InputMap::update (Input &n) {
	if(m_active){
		for (auto it = m_assignedInputs.begin (); it != m_assignedInputs.end (); ++it) {
			it->second->update (n);
		}
	}
}

void InputMap::reset () {
	if (m_active) {
		for (auto it = m_assignedInputs.begin (); it != m_assignedInputs.end (); ++it) {
			it->second->reset ();
		}
	}
}

void InputMap::enable () {
	m_active = true;
}

void InputMap::disable () {
	m_active = false;
}

const bool& InputMap::active () {
	return m_active;
}

const bool InputMap::hasInput (INPUT_ADDRESS_SUB_TYPE const& addr) {
	return m_assignedInputs.find (addr) != m_assignedInputs.end ();
}

const bool InputMap::isPressed (INPUT_ADDRESS_SUB_TYPE const& addr) const { //Action

	auto it = m_assignedInputs.find (addr);

	if (it != m_assignedInputs.end ()) {
		return it->second->isPressed ();
	}
	return false;
}
const bool InputMap::isDown (INPUT_ADDRESS_SUB_TYPE const& addr) const { //State

	auto it = m_assignedInputs.find (addr);

	if (it != m_assignedInputs.end ()) {
		return it->second->isDown ();
	}
	return false;
}
const bool InputMap::isReleased (INPUT_ADDRESS_SUB_TYPE const& addr) const { //State

	auto it = m_assignedInputs.find (addr);

	if (it != m_assignedInputs.end ()) {
		return it->second->isReleased ();
	}
	return false;
}

const Vec2i InputMap::getStartPos (INPUT_ADDRESS_SUB_TYPE const& addr) const {
	auto it = m_assignedInputs.find (addr);

	if (it != m_assignedInputs.end ()) {
		return it->second->getStartPos ();
	}
	return Vec2i();
}

const Vec2i InputMap::getDeltaPos (INPUT_ADDRESS_SUB_TYPE const& addr) const { //State

	auto it = m_assignedInputs.find (addr);

	if (it != m_assignedInputs.end ()) {
		return it->second->getDeltaPos ();
	}
	return Vec2i ();
}

const Vec2i InputMap::getEndPos (INPUT_ADDRESS_SUB_TYPE const& addr) const { //State

	auto it = m_assignedInputs.find (addr);

	if (it != m_assignedInputs.end ()) {
		return it->second->getEndPos ();
	}
	return Vec2i ();
}

//const sf::Event* InputMap::getEvent (INPUT_ADDRESS_SUB_TYPE const& addr) const { //State
//
//	auto it = m_assignedInputs.find (addr);
//
//	if (it != m_assignedInputs.end ()) {
//		return it->second->getEvent ();
//	}
//	return nullptr;
//}