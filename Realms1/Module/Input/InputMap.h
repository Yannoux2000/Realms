#pragma once

#include <string>
#include <unordered_map>
#include "InputAssignement.h"

namespace rlms {
	class InputMap {
	//represent the multiple bindings between a sf::event and the Actions (action type is std::string typed)
	//can be different depending on the context
	private:
		std::unordered_map<std::string, IAssignement*> m_assignedInputs;
		bool m_active = false;
	public:
		void bindInput (std::string name, sf::Keyboard::Key && key);
		void bindInput (std::string name, sf::Mouse::Button && button);
		void bindSlide (std::string name, sf::Keyboard::Key && key);
		void bindSlide (std::string name, sf::Mouse::Button && button);
		void bindEvent (std::string const& name, sf::Event::EventType && e);

		void unbindInput (std::string name);
		void clearInputs ();

		void update (sf::Event* e = nullptr);
		void reset ();

		void enable ();
		void disable ();
		const bool& active ();

		const bool hasInput (std::string const& name);
		const bool isPressed (std::string const& name) const; //Action
		const bool isDown (std::string const& name) const; //State
		const bool isReleased (std::string const& name) const; //State

		const sf::Vector2i getStartPos (std::string const& name) const; //State
		const sf::Vector2i getDeltaPos (std::string const& name) const; //State
		const sf::Vector2i getEndPos (std::string const& name) const; //State

		const sf::Event* getEvent (std::string const& name) const;
	};
}

