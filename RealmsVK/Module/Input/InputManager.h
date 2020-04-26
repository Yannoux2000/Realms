#pragma once

#include <string>
#include <memory>

#include "../../Base/Logging/LoggerHandler.h"
#include "../../Base/Math/Vec2.h"
#include "../../Base/Exception.h"

#include "GLFW/glfw3.h"
#include "Input.h"
#include "AssignAddress.h"
#include "InputException.h"

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
		static void EnableInputMap (INPUT_ADDRESS_SUB_TYPE const& map);
		static void EnableInputMap (std::string const& name);
		static void DisableInputMap (INPUT_ADDRESS_SUB_TYPE const& map);
		static void DisableInputMap (std::string const& name);

		static void UnBindInput (AssignAddress const& addr);
		static void ClearInputs ();

		static void Poll (GLFWwindow*& window);
		static void Update (Input&& n);
		static void UpdateCursor (Vec2i c_pos);
		static void UpdateScroll (Vec2i s_pos);

		static const bool HasInput (std::string const& name);
		static const bool HasInput (AssignAddress const& addr);
		static const AssignAddress GetInput (std::string const& name);

		static const bool IsPressed (AssignAddress const& addr);
		static const bool IsDown (AssignAddress const& addr);
		static const bool IsReleased (AssignAddress const& addr);

		static const Vec2i GetCursorPos ();
		static const Vec2i GetDeltaPos (AssignAddress const& addr);
		static const Vec2i GetEndPos (AssignAddress const& addr);
		//static const sf::Event* GetEvent (AssignAddress const& addr);
	};
}