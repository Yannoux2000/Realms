#include "Input.h"

using namespace rlms;

Input rlms::CreatingKeyboardButtonInput (int key, int scancode, int action, int mods) {
	Input n;
	n.type = Input::InputType::Keyboard;
	n.scancode = scancode;
	switch (action) {
		case GLFW_PRESS:
			n.action = Input::InputAction::Press;
			break;
		case GLFW_REPEAT:
			n.action = Input::InputAction::Press;
			break;
		case GLFW_RELEASE:
			n.action = Input::InputAction::Release;
			break;
	}
	n.mods = mods;
	return n;
}

Input rlms::CreatingMouseButtonInput (int button, int action, int mods) {
	Input n;
	n.type = Input::InputType::Mouse;
	n.scancode = button;
	switch (action) {
		case GLFW_PRESS:
			n.action = Input::InputAction::Press;
			break;
		case GLFW_RELEASE:
			n.action = Input::InputAction::Release;
			break;
	}
	n.mods = mods;
	return n;
}

Input rlms::CreatingEventInput (int button, int action, int mods) {
	Input n;
	n.type = Input::InputType::Mouse;
	n.scancode = button;
	switch (action) {
		case GLFW_PRESS:
			n.action = Input::InputAction::Press;
			break;
		case GLFW_RELEASE:
			n.action = Input::InputAction::Release;
			break;
	}
	n.mods = mods;
	return n;
}
