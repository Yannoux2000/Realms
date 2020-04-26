#pragma once

#include <string>
#include <unordered_map>
#include "InputType.h"
#include "AssignAddress.h"

#include "../../Base/Math/Vec2.h"

namespace rlms {
	class InputMap {
	//represent the multiple bindings between a sf::event and the Actions (action type is std::string typed)
	//can be different depending on the context
	private:
		std::unordered_map<INPUT_ADDRESS_SUB_TYPE, IInput*> m_assignedInputs;
		bool m_active = false;
	public:
		void bindKey (INPUT_ADDRESS_SUB_TYPE addr, int && key);
		void bindMouseButton (INPUT_ADDRESS_SUB_TYPE addr, int && button);
		void bindKeySlide (INPUT_ADDRESS_SUB_TYPE addr, int && key);
		void bindMouseButtonSlide (INPUT_ADDRESS_SUB_TYPE addr, int && button);
		//void bindEvent (INPUT_ADDRESS_SUB_TYPE addr, sf::Event::EventType && e);

		void unbindInput (INPUT_ADDRESS_SUB_TYPE addr);
		void clearInputs ();

		void update (Input& n);
		void reset ();

		void enable ();
		void disable ();
		const bool& active ();

		const bool hasInput (INPUT_ADDRESS_SUB_TYPE const& addr);
		const bool isPressed (INPUT_ADDRESS_SUB_TYPE const& addr) const; //Action
		const bool isDown (INPUT_ADDRESS_SUB_TYPE const& addr) const; //State
		const bool isReleased (INPUT_ADDRESS_SUB_TYPE const& addr) const; //State

		const Vec2i getStartPos (INPUT_ADDRESS_SUB_TYPE const& addr) const; //State
		const Vec2i getDeltaPos (INPUT_ADDRESS_SUB_TYPE const& addr) const; //State
		const Vec2i getEndPos (INPUT_ADDRESS_SUB_TYPE const& addr) const; //State
	};
}

