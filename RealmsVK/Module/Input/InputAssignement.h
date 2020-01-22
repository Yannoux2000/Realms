#pragma once

//#include "SFML/Window.hpp"
#include "../../Base/Math/Vec2.h"
#include "Input.h"

// Action : a single-time thing like openning a door or casting a spell pressed then discarded
// State : a repeatable action, like walking in an fps or arrows in a str
// Range : Mouse movements, they can have variable impulses
// Pointed : Action with a defined screen position
// Slided : Pointed Action, that have been followed by a cursor displacement. (drag and drop)

// Rangeless input : referes to keypressed when interpreted as non-binary i.e.

namespace rlms {
	class IAssignement {
	public:

		IAssignement () {};
		virtual ~IAssignement () {};

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

		virtual const rlms::Vec2i getStartPos () const { //Pointed
			return rlms::Vec2i ();
		}

		virtual const rlms::Vec2i getEndPos () const { //Slided
			return rlms::Vec2i ();
		}

		virtual const rlms::Vec2i getDeltaPos () const { //Slided
			return rlms::Vec2i ();
		}

		//virtual const sf::Event* getEvent () const {
		//	return nullptr;
		//}
	};

	class InputAssignement : public IAssignement {
	protected:
		bool m_pressflag;
		bool m_holdflag;
		bool m_releaseflag;

		void flagUpdate (const bool& new_input);
	public:

		InputAssignement () : m_pressflag (false), m_holdflag (false), m_releaseflag (false) {};
		virtual ~InputAssignement () {};

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

	class SlideInputAssignement : public InputAssignement {
	protected:
		rlms::Vec2i m_pos;
		rlms::Vec2i m_deltaPos;
		rlms::Vec2i m_lastPos;

		void slideUpdate (const bool& new_input, rlms::Vec2i new_pos);
	public:
		SlideInputAssignement () : InputAssignement () {};

		const rlms::Vec2i getStartPos () const override;
		const rlms::Vec2i getEndPos () const override;
		const rlms::Vec2i getDeltaPos () const override;
	};

	class EventAssignement : public InputAssignement {
	private:
	//	sf::Event::EventType m_type;
	//	std::unique_ptr<sf::Event> m_event;
	public:
	//	EventAssignement (sf::Event::EventType && t) : InputAssignement (), m_type(t), m_event () {};
		void update (Input& n) override;
		void reset () override;
	//	const sf::Event* getEvent () const override;

	};

	class KeyInputAssignement : public InputAssignement {
	private:
		int m_key;

	public:
		KeyInputAssignement (int && key) : m_key (key), InputAssignement () {};

		void update (Input& n) override {
			if(n.scancode == m_key) {
				flagUpdate (n.action == Input::InputAction::Press);
			}
		}
	};

	class KeyMouseSlideAssignement : public SlideInputAssignement {
	protected:
		int m_key;

	public:
		KeyMouseSlideAssignement (int && key) : m_key (key), SlideInputAssignement () {};

		void update (Input& n) override {
			//slideUpdate (sf::Keyboard::isKeyPressed (m_key), sf::Mouse::getPosition ());
		}
	};

	class MouseButtonInputAssignement : public InputAssignement {
	protected:
		int m_button;

	public:
		MouseButtonInputAssignement (int && button) : m_button (button), InputAssignement () {};

		void update (Input& n) override {
			//flagUpdate (sf::Mouse::isButtonPressed (m_button));
		}
	};

	class MouseButtonSlideAssignement : public SlideInputAssignement {
	protected:
		int m_button;

	public:
		MouseButtonSlideAssignement (int && button) : m_button (button), SlideInputAssignement () {};

		void update (Input& n) override {
			//slideUpdate (sf::Mouse::isButtonPressed (m_button), sf::Mouse::getPosition ());
		}
	};
}
