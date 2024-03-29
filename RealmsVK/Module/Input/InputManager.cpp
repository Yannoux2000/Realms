#include "InputManager.h"

#include "../../Base/Logging/ILogged.h"

#include <unordered_map>

#include "InputMap.h"
#include "AssignSanitizer.h"

using namespace rlms;

class rlms::InputManagerImpl : public ILogged {
private:
	friend class InputManager;
	
	std::string getLogName () override {
		return "InputManager";
	};

	rlms::Vec2i m_cursor_pos, m_scroll_pos;
	std::unordered_map<std::string, rlms::InputMap*> m_maps;
	rlms::InputMap m_globalMap;
	bool m_typing;
	std::string text_typed;

	bool start (std::shared_ptr<Logger> funnel);
	void stop ();

	//get will create a new map if no one is found
	std::unordered_map<std::string, rlms::InputMap*>::iterator __getMap (std::string mapName);

	//find will search for an existing map, otherwise throws an error
	std::unordered_map<std::string, rlms::InputMap*>::iterator __findMap (std::string mapName);

	void bindKey (std::string const& name, int&& key);
	void bindMouseButton (std::string const& name, int&& button);
	void bindKeySlide (std::string const& name, int&& key);
	void bindMouseButtonSlide (std::string const& name, int&& button);
	//void bindEvent (std::string const& name, sf::Event::EventType&& e);

	void enableInputMap (std::string const& name);
	void disableInputMap (std::string const& name);

	void unbindInput (std::string const& name);
	void clearInputs ();

	void poll (GLFWwindow*& window);
	void update (Input&& n);

	const void setTyping (bool typing = true);
	const bool isTyping ();

	const bool hasInput (std::string const& name);
	const bool isPressed (std::string const& name);
	const bool isDown (std::string const& name);
	const bool isReleased (std::string const& name);

	rlms::Vec2i getCursorPos ();
	const rlms::Vec2i getDeltaPos (std::string const& name);
	const rlms::Vec2i getEndPos (std::string const& name);

	//const sf::Event* getEvent (std::string const& name);

	static void MouseButtonFunction (GLFWwindow* window, int button, int action, int mods) {
		Input mbi = MouseButtonInput (button, action, mods);
		InputManager::Update (std::move(mbi));
	}
	static void CursorPosFunction (GLFWwindow* window, double posx, double posy) {
		auto m_pos = Vec2i (floor<int> (posx), floor<int> (posy));
		InputManager::UpdateCursor (m_pos);
	}
	static void KeyboardFunction (GLFWwindow* window, int key, int scancode, int action, int mods) {
		Input ki = KeyInput (key, scancode, action, mods);
		InputManager::Update (std::move (ki));
	}
	static void MouseScrollFunction (GLFWwindow* window, double xoffset, double yoffset) {
		auto m_offset = Vec2i (floor<int> (xoffset), floor<int> (yoffset));
		InputManager::UpdateScroll (m_offset);
	}

public:
	InputManagerImpl () : ILogged(), m_globalMap(), m_typing (false), text_typed ("") {};
	~InputManagerImpl () {};
};

bool rlms::InputManagerImpl::start (std::shared_ptr<Logger> funnel) {
	startLogger (funnel);
	logger->tag (LogTags::None) << "Initializing !" << '\n';

	m_globalMap.enable ();
	logger->tag (LogTags::None) << "Initialized correctly !" << '\n';

	return true;
}

void rlms::InputManagerImpl::stop () {
	logger->tag (LogTags::None) << "Terminating" << '\n';
	clearInputs ();
	logger->tag (LogTags::None) << "Terminated correctly !" << '\n';
}

//get will create a new map if no one is found
std::unordered_map<std::string, rlms::InputMap*>::iterator rlms::InputManagerImpl::__getMap (std::string mapName) {
	auto it = m_maps.find (mapName);
	if (it == m_maps.end ()) {
		logger->tag (LogTags::Info) << "Creating map : " << mapName << ".\n";
		m_maps.insert (std::make_pair (mapName, new InputMap ()));
		it = m_maps.find (mapName);
	}
	return it;
}

//find will search for an existing map, otherwise throws an error
std::unordered_map<std::string, rlms::InputMap*>::iterator rlms::InputManagerImpl::__findMap (std::string mapName) {
	auto it = m_maps.find (mapName);
	if (it == m_maps.end ()) {
		throw InvalidMapName ();
		return m_maps.begin ();
	}
	return it;
}

void rlms::InputManagerImpl::bindKey (std::string const& name, int && key) {
	if (AssignSanitizer::IsGlobal (name)) {
		std::string inputName = AssignSanitizer::NameSanitizer (name);
		logger->tag (LogTags::Info) << inputName << " Binding Global Key Input.\n";
		m_globalMap.bindKey (inputName, std::move (key));
	} else {
		std::string mapName = AssignSanitizer::GetMapName (name);
		std::string inputName = AssignSanitizer::GetInputName (name);
		logger->tag (LogTags::Info) << mapName << "::" << inputName << " Binding Key Input.\n";

		auto it = __getMap (mapName);

		it->second->bindKey (inputName, std::move (key));
	}
}
void rlms::InputManagerImpl::bindMouseButton (std::string const& name, int && button) {
	if (AssignSanitizer::IsGlobal (name)) {
		std::string inputName = AssignSanitizer::NameSanitizer (name);
		logger->tag (LogTags::Info) << inputName << " Binding Global Mouse Input.\n";
		m_globalMap.bindMouseButton (inputName, std::move (button));
	} else {
		std::string mapName = AssignSanitizer::GetMapName (name);
		std::string inputName = AssignSanitizer::GetInputName (name);
		logger->tag (LogTags::Info) << mapName << "::" << inputName << " Binding Mouse Input.\n";

		auto it = __getMap (mapName);

		it->second->bindMouseButton (inputName, std::move (button));
	}
}
void rlms::InputManagerImpl::bindKeySlide (std::string const& name, int && key) {
	if (AssignSanitizer::IsGlobal (name)) {
		std::string inputName = AssignSanitizer::NameSanitizer (name);
		logger->tag (LogTags::Info) << inputName << "Binding Global Key Slide Input.\n";
		m_globalMap.bindKeySlide (inputName, std::move (key));
	} else {
		std::string mapName = AssignSanitizer::GetMapName (name);
		std::string inputName = AssignSanitizer::GetInputName (name);
		logger->tag (LogTags::Info) << mapName << "::" << inputName << " Binding Key Slide Input.\n";

		auto it = __getMap (mapName);

		it->second->bindKeySlide (inputName, std::move (key));
	}
}
void rlms::InputManagerImpl::bindMouseButtonSlide (std::string const& name, int && button) {

	if (AssignSanitizer::IsGlobal (name)) {
		std::string inputName = AssignSanitizer::NameSanitizer (name);
		logger->tag (LogTags::Info) << inputName << " Binding Global Mouse Slide Input.\n";
		m_globalMap.bindMouseButtonSlide (inputName, std::move (button));
	} else {
		std::string mapName = AssignSanitizer::GetMapName (name);
		std::string inputName = AssignSanitizer::GetInputName (name);
		logger->tag (LogTags::Info) << mapName << "::" << inputName << " Binding Mouse Slide Input.\n";

		auto it = __getMap (mapName);

		it->second->bindMouseButtonSlide (inputName, std::move (button));
	}
}

//void rlms::InputManagerImpl::bindEvent (std::string const& name, sf::Event::EventType && e) {
//
//	if (AssignSanitizer::IsGlobal (name)) {
//		std::string inputName = AssignSanitizer::NameSanitizer (name);
//		logger->tag (LogTags::Info) << inputName << " Binding Global Event Input.\n";
//		m_globalMap.bindEvent (inputName, std::move (e));
//	} else {
//		std::string mapName = AssignSanitizer::GetMapName (name);
//		std::string inputName = AssignSanitizer::GetInputName (name);
//		logger->tag (LogTags::Info) << mapName << "::" << inputName << " Binding Event Input.\n";
//		logger->tag (LogTags::Warning) << "Binding will never be triggered !\n";
//
//		auto it = __getMap (mapName);
//
//		it->second->bindEvent (inputName, std::move (e));
//	}
//}

void rlms::InputManagerImpl::enableInputMap (std::string const& mapName) {
	logger->tag (LogTags::Info) << "Enabled Map : " << mapName << ".\n";
	auto it = __findMap (mapName);
	it->second->enable ();
}

void rlms::InputManagerImpl::disableInputMap (std::string const& mapName) {
	logger->tag (LogTags::Info) << "Disabled Map : " << mapName << ".\n";
	auto it = __findMap (mapName);
	it->second->disable ();
}

void rlms::InputManagerImpl::unbindInput (std::string const& name) {
	if (AssignSanitizer::IsGlobal (name)) {
		logger->tag (LogTags::Info) << "Unbinding Global Input : " << name << ".\n";
		m_globalMap.unbindInput (name);
	} else {
		std::string mapName = AssignSanitizer::GetMapName (name);
		std::string inputName = AssignSanitizer::GetInputName (name);
		logger->tag (LogTags::Info) << "Unbinding Input : " << inputName << ", to map : " << mapName << ".\n";

		auto it = __findMap (mapName);

		it->second->unbindInput (inputName);
	}
}

void rlms::InputManagerImpl::clearInputs () {
	logger->tag (LogTags::Warning) << "Clearing all Inputs.\n";
	while (!m_maps.empty ()) {
		auto it = m_maps.begin ();
		it->second->clearInputs ();
		delete it->second;
		m_maps.erase (it);
	}

	m_globalMap.clearInputs ();
}
void rlms::InputManagerImpl::poll (GLFWwindow*& window) {
	//logger->tag (LogTags::None) << "Polling new Inputs" << '\n';
	glfwPollEvents ();
}

void rlms::InputManagerImpl::update (Input&& n) {
	logger->tag (LogTags::None) << "update ! ("<< n.scancode << ", " << static_cast<char>(glfwGetKeyScancode(n.scancode)) << ")" << '\n';
	m_globalMap.update (n);
	for (auto it = m_maps.begin (); it != m_maps.end (); it++) {
		it->second->update (n);
	}
}

const void rlms::InputManagerImpl::setTyping (bool typing) {
	m_typing = typing;
}
const bool rlms::InputManagerImpl::isTyping () {
	return m_typing;
}

const bool rlms::InputManagerImpl::hasInput (std::string const& name) {
	if (AssignSanitizer::IsGlobal (name)) {
		return m_globalMap.hasInput (name);
	} else {
		std::string mapName = AssignSanitizer::GetMapName (name);
		std::string inputName = AssignSanitizer::GetInputName (name);

		auto it = __findMap (mapName);

		return it->second->hasInput (inputName);
	}
}
const bool rlms::InputManagerImpl::isPressed (std::string const& name) {
	if (AssignSanitizer::IsGlobal (name)) {
		return m_globalMap.isPressed (name);
	} else {
		std::string mapName = AssignSanitizer::GetMapName (name);
		std::string inputName = AssignSanitizer::GetInputName (name);

		auto it = __findMap (mapName);

		return it->second->isPressed (inputName);
	}
}
const bool rlms::InputManagerImpl::isDown (std::string const& name) {
	if (AssignSanitizer::IsGlobal (name)) {
		return m_globalMap.isDown (name);
	} else {
		std::string mapName = AssignSanitizer::GetMapName (name);
		std::string inputName = AssignSanitizer::GetInputName (name);

		auto it = __findMap (mapName);
		return it->second->isDown (inputName);
	}
}
const bool rlms::InputManagerImpl::isReleased (std::string const& name) {
	if (AssignSanitizer::IsGlobal (name)) {
		return m_globalMap.isReleased (name);
	} else {
		std::string mapName = AssignSanitizer::GetMapName (name);
		std::string inputName = AssignSanitizer::GetInputName (name);

		auto it = __findMap (mapName);
		return it->second->isReleased (inputName);
	}
}

rlms::Vec2i rlms::InputManagerImpl::getCursorPos () {
	return m_cursor_pos;
}
const rlms::Vec2i rlms::InputManagerImpl::getDeltaPos (std::string const& name) {
	if (AssignSanitizer::IsGlobal (name)) {
		return m_globalMap.getDeltaPos (name);
	} else {
		std::string mapName = AssignSanitizer::GetMapName (name);
		std::string inputName = AssignSanitizer::GetInputName (name);

		auto it = __findMap (mapName);
		return it->second->getDeltaPos (inputName);
	}
}
const rlms::Vec2i rlms::InputManagerImpl::getEndPos (std::string const& name) {
	if (AssignSanitizer::IsGlobal (name)) {
		return m_globalMap.getEndPos (name);
	} else {
		std::string mapName = AssignSanitizer::GetMapName (name);
		std::string inputName = AssignSanitizer::GetInputName (name);

		auto it = __findMap (mapName);
		return it->second->getEndPos (inputName);
	}
}

//const sf::Event* rlms::InputManagerImpl::getEvent (std::string const& name) {
//	if (AssignSanitizer::IsGlobal (name)) {
//		return m_globalMap.getEvent (name);
//	} else {
//		std::string mapName = AssignSanitizer::GetMapName (name);
//		std::string inputName = AssignSanitizer::GetInputName (name);
//
//		auto it = __findMap (mapName);
//		return it->second->getEvent (inputName);
//	}
//}

//InputManager
std::unique_ptr<rlms::InputManagerImpl> rlms::InputManager::instance;

std::shared_ptr<rlms::LoggerHandler> rlms::InputManager::GetLogger () {
	return instance->getLogger ();
}

bool rlms::InputManager::Initialize (std::shared_ptr<Logger> funnel) {
	instance = std::make_unique<InputManagerImpl> ();
	instance->start (funnel);
	return true;
}

void rlms::InputManager::Terminate () {
	instance->stop ();
	instance.reset ();
}

void rlms::InputManager::SetCallbacks (GLFWwindow*& window) {
	glfwSetMouseButtonCallback (window, InputManagerImpl::MouseButtonFunction);
	glfwSetCursorPosCallback (window, InputManagerImpl::CursorPosFunction);
	glfwSetKeyCallback (window, InputManagerImpl::KeyboardFunction);
	glfwSetScrollCallback (window, InputManagerImpl::MouseScrollFunction);
}

void rlms::InputManager::BindKey (std::string const& name, int && key) {
	instance->bindKey (name, std::move(key));
}
void rlms::InputManager::BindMouseButton (std::string const& name, int && button) {
	instance->bindMouseButton (name, std::move (button));
}
void rlms::InputManager::BindKeySlide (std::string const& name, int && key) {
	instance->bindKeySlide (name, std::move (key));
}
void rlms::InputManager::BindMouseButtonSlide (std::string const& name, int && button) {
	instance->bindMouseButtonSlide (name, std::move (button));
}

//void rlms::InputManager::BindEvent (std::string const& name, sf::Event::EventType && e) {
//	instance->bindEvent (name, std::move (e));
//}

void rlms::InputManager::EnableInputMap (std::string const& name) {
	instance->enableInputMap(name);
}

void rlms::InputManager::DisableInputMap (std::string const& name) {
	instance->disableInputMap (name);
}

void rlms::InputManager::UnBindInput (std::string const& name) {
	instance->unbindInput (name);
}
void rlms::InputManager::ClearInputs () {
	instance->clearInputs();
}

void rlms::InputManager::Poll (GLFWwindow*& window) {
	instance->poll (window);
}

void rlms::InputManager::Update (Input&& n) {
	instance->update (std::move(n));
}

void rlms::InputManager::UpdateCursor (rlms::Vec2i c_pos) {
	instance->m_cursor_pos = c_pos;
}

void rlms::InputManager::UpdateScroll (rlms::Vec2i s_pos) {
	instance->m_scroll_pos = s_pos;
}


const bool rlms::InputManager::HasInput (std::string const& name) {
	return instance->hasInput (name);
}
const bool rlms::InputManager::IsPressed (std::string const& name) {
	return instance->isPressed(name);
}
const bool rlms::InputManager::IsDown (std::string const& name) {
	return instance->isDown (name);
}
const bool rlms::InputManager::IsReleased (std::string const& name) {
	return instance->isReleased (name);
}

const rlms::Vec2i rlms::InputManager::GetCursorPos () {
	return instance->getCursorPos();
}
const rlms::Vec2i rlms::InputManager::GetDeltaPos (std::string const& name) {
	return instance->getDeltaPos (name);
}
const rlms::Vec2i rlms::InputManager::GetEndPos (std::string const& name) {
	return instance->getEndPos (name);
}

//const sf::Event* rlms::InputManager::GetEvent (std::string const& name) {
//	return instance->getEvent(name);
//}
