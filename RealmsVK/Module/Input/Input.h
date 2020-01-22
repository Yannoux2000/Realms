#pragma once
#include "GLFW/glfw3.h"

namespace rlms {
	struct Input {
		enum InputType {
			Other,
			Keyboard,
			Mouse,
			Joystick,
			Event
		};
		InputType type = Other;

		enum InputAction {
			None,
			Press,
			Release
		};

		InputAction action = None;

		int scancode = 0;
		int mods = 0;
	};

	Input KeyInput (int key, int scancode, int action, int mods);
	Input MouseButtonInput (int button, int action, int mods);
}