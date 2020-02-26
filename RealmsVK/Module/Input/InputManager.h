#pragma once

#include <string>
#include <memory>

#include "../../Base/Logging/LoggerHandler.h"
#include "../../Base/Math/Vec2.h"
#include "../../Base/Exception.h"

#include "GLFW/glfw3.h"
#include "Input.h"

//Voc:
// *hardware* input : the key being pressed (as presented from the OS)

// Command : an input that has been processed and validated always results as an ingame action
// the following can be concidered as Commands

// Action : a single-time thing like openning a door or casting a spell pressed then discarded
// State : a repeatable action, like walking in an fps or arrows in a str
// Range : Mouse movements, they can have variable impulses
// Pointed : Action with a defined screen position
// Slided : Pointed Action, that have been followed by a cursor displacement. (drag and drop)

// Rangeless input : referes to keypressed when interpreted as non-binary i.e.

// Mapped : hardware inputs being 

/*
the idea :

	Startup :
		Load All Assignement requested
		read controls config file, and bind them

	Loop start:
		Event being polled from OS,

		For each Assignement Mapped, check inputs

		generate command list

*/
namespace rlms {
	class InvalidInput : public Exception {
	public:
		InvalidInput (std::string t_type, std::string t_description) noexcept : Exception (t_type, t_description) {};
	};

	struct InvalidMapName : public InvalidInput {
		InvalidMapName () noexcept : InvalidInput ("Invalid map name.", "all input name must be formated as {MapName}::{ActionName}.") {};
	};

	struct UnbindedInput : public InvalidInput {
		UnbindedInput () noexcept : InvalidInput ("Unbinded input.", "action has not been binded to an input.") {};
	};

	class InputManagerImpl;

	class InputManager {
		// This class is a Singleton
		// Used to Generate InputMap
		// to assign them to ingame actions.
	private:
		static std::unique_ptr<InputManagerImpl> instance;
	public:

		static std::shared_ptr<LoggerHandler> GetLogger ();
		static bool Initialize (std::shared_ptr<Logger> funnel = nullptr);
		static void Terminate ();

		static void SetCallbacks (GLFWwindow*& window);

		static void BindKey (std::string const& name, int&& key);
		static void BindMouseButton (std::string const& name, int&& button);
		static void BindKeySlide (std::string const& name, int&& key);
		static void BindMouseButtonSlide (std::string const& name, int&& button);
		//static void BindEvent (std::string const& name, sf::Event::EventType&& e);

		//map names would be separated by a :: from the key tag, to separate different maps, enabeling a map would be throught the use of 
		static void EnableInputMap (std::string const& name);
		static void DisableInputMap (std::string const& name);

		static void UnBindInput (std::string const& name);
		static void ClearInputs ();

		static void Poll (GLFWwindow*& window);
		static void Update (Input&& n);
		static void UpdateCursor (rlms::Vec2i c_pos);
		static void UpdateScroll (rlms::Vec2i s_pos);

		static const bool HasInput (std::string const& name);
		static const bool IsPressed (std::string const& name);
		static const bool IsDown (std::string const& name);
		static const bool IsReleased (std::string const& name);

		static const rlms::Vec2i GetCursorPos ();
		static const rlms::Vec2i GetDeltaPos (std::string const& name);
		static const rlms::Vec2i GetEndPos (std::string const& name);

		//static const sf::Event* GetEvent (std::string const& name);
	};
}