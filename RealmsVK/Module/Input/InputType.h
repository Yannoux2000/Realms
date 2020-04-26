#pragma once

//#include "SFML/Window.hpp"
#include "../../Base/Math/Vec2.h"
#include "Input.h"
#include "InputManager.h"

// Action : a single-time thing like openning a door or casting a spell pressed then discarded
// State : a repeatable action, like walking in an fps or arrows in a str
// Range : Mouse movements, they can have variable impulses
// Pointed : Action with a defined screen position
// Slided : Pointed Action, that have been followed by a cursor displacement. (drag and drop)

// Rangeless input : referes to keypressed when interpreted as non-binary i.e.

namespace rlms {
	class IInput {
	public:

		IInput () {};
		virtual ~IInput () {};

		virtual void update (Input &n) {}
		virtual void reset () {}
		virtual const bool isPressed () const { //Action
			return false;
		}

		virtual const bool isDown () const { //State
			return false;
		}

		virtual const bool isReleased () const { //Action
			return false;
		}

		virtual const Vec2i getStartPos () const { //Pointed
			return Vec2i ();
		}

		virtual const Vec2i getEndPos () const { //Slided
			return Vec2i ();
		}

		virtual const Vec2i getDeltaPos () const { //Slided
			return Vec2i ();
		}

		//virtual const sf::Event* getEvent () const {
		//	return nullptr;
		//}
	};

	class ButtonInput : public IInput {
	protected:
		bool m_pressflag;
		bool m_holdflag;
		bool m_releaseflag;

		void flagUpdate (const bool& new_input);
	public:

		ButtonInput () : m_pressflag (false), m_holdflag (false), m_releaseflag (false) {};
		virtual ~ButtonInput () {};

		virtual const bool isPressed () const { //Action
			return m_pressflag;
		}

		virtual const bool isDown () const { //State
			return m_holdflag;
		}

		virtual const bool isReleased () const { //Action
			return m_releaseflag;
		}
	};

	class SlideInput : public ButtonInput {
	protected:
		Vec2i m_pos;
		Vec2i m_deltaPos;
		Vec2i m_lastPos;

		void slideUpdate (const bool& new_input, Vec2i new_pos);
	public:
		SlideInput () : ButtonInput () {};

		const Vec2i getStartPos () const override;
		const Vec2i getEndPos () const override;
		const Vec2i getDeltaPos () const override;
	};

	class EventInput : public ButtonInput {
	private:
	//	sf::Event::EventType m_type;
	//	std::unique_ptr<sf::Event> m_event;
	public:
	//	EventAssignement (sf::Event::EventType && t) : InputAssignement (), m_type(t), m_event () {};
		void update (Input& n) override;
		void reset () override;
	//	const sf::Event* getEvent () const override;
	};

	class KeyboardButtonInput : public ButtonInput {
	private:
		int m_key;

	public:
		KeyboardButtonInput (int && key) : m_key (key), ButtonInput () {};

		void update (Input& n) override {
			if(n.scancode == m_key) {
				flagUpdate (n.action == Input::InputAction::Press);
			}
		}
	};

	class KeyboardSlideInput : public SlideInput {
	protected:
		int m_key;

	public:
		KeyboardSlideInput (int && key) : m_key (key), SlideInput () {};

		void update (Input& n) override {
			if (n.scancode == m_key) {
				slideUpdate (n.action == Input::InputAction::Press, InputManager::GetCursorPos());
			}
		}
	};

	class MouseButtonInput : public ButtonInput {
	protected:
		int m_button;

	public:
		MouseButtonInput (int && button) : m_button (button), ButtonInput () {};

		void update (Input& n) override {
			if (n.scancode == m_button) {
				flagUpdate (n.action == Input::InputAction::Press);
			}
		}
	};

	class MouseSlideInput : public SlideInput {
	protected:
		int m_button;

	public:
		MouseSlideInput (int && button) : m_button (button), SlideInput () {};

		void update (Input& n) override {
			if (n.scancode == m_button) {
				slideUpdate (n.action == Input::InputAction::Press, InputManager::GetCursorPos ());
			}
		}
	};
}
