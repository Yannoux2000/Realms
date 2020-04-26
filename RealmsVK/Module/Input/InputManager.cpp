#include "InputManager.h"

#include "../../Base/Logging/ILogged.h"

#include <unordered_map>

#include "InputMap.h"
#include "AssignSanitizer.h"
#include "AssignDictionnary.h"
#include "AssignAddress.h"

using namespace rlms;

class rlms::InputManagerImpl : public ILogged {
private:
	friend class InputManager;
	
	std::string getLogName () override {
		return "InputManager";
	};

	Vec2i m_cursor_pos, m_scroll_pos;
	std::unordered_map<INPUT_ADDRESS_SUB_TYPE, InputMap*> m_maps;
	AssignDictionnary dict;
	std::string text_typed;
	bool m_typing;

	bool start (std::shared_ptr<Logger> funnel);
	void stop ();

	//get will create a new map if no one is found
	std::unordered_map<INPUT_ADDRESS_SUB_TYPE, InputMap*>::iterator __getMap (INPUT_ADDRESS_SUB_TYPE map);

	//find will search for an existing map, otherwise throws an error
	std::unordered_map<INPUT_ADDRESS_SUB_TYPE, InputMap*>::iterator __findMap (INPUT_ADDRESS_SUB_TYPE map);

	void bindKey (std::string const& name, int&& key);
	void bindMouseButton (std::string const& name, int&& button);
	void bindKeySlide (std::string const& name, int&& key);
	void bindMouseButtonSlide (std::string const& name, int&& button);
	//void bindEvent (INPUT_ADDRESS_SUB_TYPE const& addr, sf::Event::EventType&& e);

	void enableInputMap (INPUT_ADDRESS_SUB_TYPE const& map);
	void disableInputMap (INPUT_ADDRESS_SUB_TYPE const& map);

	void unbindInput (AssignAddress const& name);
	void clearInputs ();

	void poll (GLFWwindow*& window);
	void update (Input&& n);

	const void setTyping (bool typing = true);
	const bool isTyping ();

	const bool hasInput (std::string const& name);
	const bool hasInput (AssignAddress const& addr);
	const AssignAddress getInput (std::string const& name);

	const bool isPressed (AssignAddress const& addr);
	const bool isDown (AssignAddress const& addr);
	const bool isReleased (AssignAddress const& addr);

	Vec2i getCursorPos ();
	const Vec2i getDeltaPos (AssignAddress const& addr);
	const Vec2i getEndPos (AssignAddress const& addr);
	//const sf::Event* getEvent (std::string const& name);

	static void MouseButtonFunction (GLFWwindow* window, int button, int action, int mods) {
		Input mbi = CreatingMouseButtonInput (button, action, mods);
		InputManager::Update (std::move(mbi));
	}
	static void CursorPosFunction (GLFWwindow* window, double posx, double posy) {
		auto m_pos = Vec2i (static_cast<int>(floor (posx)), static_cast<int>(floor (posy)));
		InputManager::UpdateCursor (m_pos);
	}
	static void KeyboardFunction (GLFWwindow* window, int key, int scancode, int action, int mods) {
		Input ki = CreatingKeyboardButtonInput (key, scancode, action, mods);
		InputManager::Update (std::move (ki));
	}
	static void MouseScrollFunction (GLFWwindow* window, double xoffset, double yoffset) {
		auto m_offset = Vec2i (static_cast<int>(floor (xoffset)), static_cast<int>(floor (yoffset)));
		InputManager::UpdateScroll (m_offset);
	}

public:
	InputManagerImpl () : ILogged(), m_typing (false), text_typed ("") {};
	~InputManagerImpl () {};
};

bool InputManagerImpl::start (std::shared_ptr<Logger> funnel) {
	startLogger (funnel);
	logger->tag (LogTags::None) << "Initializing !" << '\n';
	dict.reset ();
	logger->tag (LogTags::None) << "Initialized correctly !" << '\n';
	return true;
}
void InputManagerImpl::stop () {
	logger->tag (LogTags::None) << "Terminating" << '\n';
	clearInputs ();
	logger->tag (LogTags::None) << "Terminated correctly !" << '\n';
}

//get will create a new map if no one is found
std::unordered_map<INPUT_ADDRESS_SUB_TYPE, InputMap*>::iterator InputManagerImpl::__getMap (INPUT_ADDRESS_SUB_TYPE map) {
	auto it = m_maps.find (map);
	if (it == m_maps.end ()) {
		logger->tag (LogTags::Info) << "Creating map(" << map << ").\n";
		m_maps.insert (std::make_pair (map, new InputMap ()));
		it = m_maps.find (map);
	}
	return it;
}

//find will search for an existing map, otherwise throws an error
std::unordered_map<INPUT_ADDRESS_SUB_TYPE, InputMap*>::iterator InputManagerImpl::__findMap (INPUT_ADDRESS_SUB_TYPE map) {
	auto it = m_maps.find (map);
	if (it == m_maps.end ()) {
		throw InvalidFormat ();
		return m_maps.begin ();
	}
	return it;
}

void InputManagerImpl::bindKey (std::string const& name, int && key) {
	dict.set (name);
	AssignAddress addr = dict.get (name);

	auto it = __getMap (addr.map);
	it->second->bindKey (addr.input, std::move (key));
}
void InputManagerImpl::bindMouseButton (std::string const& name, int && button) {
	dict.set (name);
	AssignAddress addr = dict.get (name);

	auto it = __getMap (addr.map);
	it->second->bindKey (addr.input, std::move (button));
}
void InputManagerImpl::bindKeySlide (std::string const& name, int && key) {
	dict.set (name);
	AssignAddress addr = dict.get (name);

	auto it = __getMap (addr.map);
	it->second->bindKeySlide (addr.input, std::move (key));
}
void InputManagerImpl::bindMouseButtonSlide (std::string const& name, int && button) {
	dict.set (name);
	AssignAddress addr = dict.get (name);

	auto it = __getMap (addr.map);
	it->second->bindKeySlide (addr.input, std::move (button));
}
//void InputManagerImpl::bindEvent (std::string const& name, sf::Event::EventType && e) {
//	dict.set (name);
//	AssignAddress addr = dict.get (name);
//
//	auto it = __getMap (addr.map);
//	it->second->bindKeySlide (addr.input, std::move (button));
//	it->second->bindEvent (inputName, std::move (e));
//	}
//}

void InputManagerImpl::enableInputMap (INPUT_ADDRESS_SUB_TYPE const& map) {
	logger->tag (LogTags::Info) << "Enabled Map : " << map << ".\n";
	auto it = __findMap (map);
	it->second->enable ();
}

void InputManagerImpl::disableInputMap (INPUT_ADDRESS_SUB_TYPE const& map) {
	logger->tag (LogTags::Info) << "Disabled Map : " << map << ".\n";
	auto it = __findMap (map);
	it->second->disable ();
}

void InputManagerImpl::unbindInput (AssignAddress const& addr) {
	auto it = __findMap (addr.map);
	it->second->unbindInput (addr.input);
}

void InputManagerImpl::clearInputs () {
	logger->tag (LogTags::Warn) << "Clearing all Inputs.\n";
	while (!m_maps.empty ()) {
		auto it = m_maps.begin ();
		it->second->clearInputs ();
		delete it->second;
		m_maps.erase (it);
	}
	dict.reset ();
}
void InputManagerImpl::poll (GLFWwindow*& window) {
	//logger->tag (LogTags::None) << "Polling new Inputs" << '\n';
	glfwPollEvents ();
}

void InputManagerImpl::update (Input&& n) {
	logger->tag (LogTags::None) << "update ! ("<< n.scancode << ", " << static_cast<char>(glfwGetKeyScancode(n.scancode)) << ")" << '\n';
	for (auto it = m_maps.begin (); it != m_maps.end (); it++) {
		it->second->update (n);
	}
}

const void InputManagerImpl::setTyping (bool typing) {
	m_typing = typing;
}
const bool InputManagerImpl::isTyping () {
	return m_typing;
}

const bool InputManagerImpl::hasInput (std::string const& name) {
	AssignAddress addr = dict.get (name);
	if (!dict.valid (addr)) {
		logger->tag (LogTags::Warn) << "invalid input address : " << name << "\n";
	}
	return dict.valid (addr);
}
const bool InputManagerImpl::hasInput (AssignAddress const& addr) {
	auto it = __findMap (addr.map);
	return it->second->hasInput (addr.input);
}
const AssignAddress InputManagerImpl::getInput (std::string const& name) {
	AssignAddress addr = dict.get (name);
	if (!dict.valid (addr)) {
		logger->tag (LogTags::Warn) << "invalid input address : " << name << "\n";
	}
	return addr;
}

Vec2i InputManagerImpl::getCursorPos () {
	return m_cursor_pos;
}
const bool InputManagerImpl::isPressed (AssignAddress const& addr) {
	auto it = __findMap (addr.map);
	return it->second->isPressed (addr.input);
}
const bool InputManagerImpl::isDown (AssignAddress const& addr) {
	auto it = __findMap (addr.map);
	return it->second->isDown (addr.input);
}
const bool InputManagerImpl::isReleased (AssignAddress const& addr) {
	auto it = __findMap (addr.map);
	return it->second->isReleased (addr.input);
}

const Vec2i InputManagerImpl::getDeltaPos (AssignAddress const& addr) {
	auto it = __findMap (addr.map);
	return it->second->getDeltaPos (addr.input);
}
const Vec2i InputManagerImpl::getEndPos (AssignAddress const& addr) {
	auto it = __findMap (addr.map);
	return it->second->getEndPos (addr.input);
}

//const sf::Event* InputManagerImpl::getEvent (std::string const& name) {
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
std::unique_ptr<InputManagerImpl> InputManager::instance;

std::shared_ptr<LoggerHandler> InputManager::GetLogger () {
	return instance->getLogger ();
}

bool InputManager::Initialize (std::shared_ptr<Logger> funnel) {
	instance = std::make_unique<InputManagerImpl> ();
	instance->start (funnel);
	return true;
}

void InputManager::Terminate () {
	instance->stop ();
	instance.reset ();
}

void InputManager::SetCallbacks (GLFWwindow*& window) {
	glfwSetMouseButtonCallback (window, InputManagerImpl::MouseButtonFunction);
	glfwSetCursorPosCallback (window, InputManagerImpl::CursorPosFunction);
	glfwSetKeyCallback (window, InputManagerImpl::KeyboardFunction);
	glfwSetScrollCallback (window, InputManagerImpl::MouseScrollFunction);
}

void InputManager::BindKey (std::string const& name, int && key) {
	instance->bindKey (name, std::move(key));
}
void InputManager::BindMouseButton (std::string const& name, int && button) {
	instance->bindMouseButton (name, std::move (button));
}
void InputManager::BindKeySlide (std::string const& name, int && key) {
	instance->bindKeySlide (name, std::move (key));
}
void InputManager::BindMouseButtonSlide (std::string const& name, int && button) {
	instance->bindMouseButtonSlide (name, std::move (button));
}

//void InputManager::BindEvent (std::string const& name, sf::Event::EventType && e) {
//	instance->bindEvent (name, std::move (e));
//}

void InputManager::EnableInputMap (INPUT_ADDRESS_SUB_TYPE const& map) {
	instance->enableInputMap(map);
}

void InputManager::EnableInputMap (std::string const& name) {
	instance->enableInputMap(instance->dict.getWord (name));
}

void InputManager::DisableInputMap (INPUT_ADDRESS_SUB_TYPE const& map) {
	instance->disableInputMap (map);
}

void InputManager::DisableInputMap (std::string const& name) {
	instance->disableInputMap (instance->dict.getWord (name));
}

void InputManager::UnBindInput (AssignAddress const& addr) {
	instance->unbindInput (addr);
}
void InputManager::ClearInputs () {
	instance->clearInputs();
}

void InputManager::Poll (GLFWwindow*& window) {
	instance->poll (window);
}

void InputManager::Update (Input&& n) {
	instance->update (std::move(n));
}

void InputManager::UpdateCursor (Vec2i c_pos) {
	instance->m_cursor_pos = c_pos;
}
void InputManager::UpdateScroll (Vec2i s_pos) {
	instance->m_scroll_pos = s_pos;
}

const bool InputManager::HasInput (std::string const& name) {
	return instance->hasInput (name);
}
const bool InputManager::HasInput (AssignAddress const& addr) {
	return instance->hasInput (addr);
}
const AssignAddress InputManager::GetInput (std::string const& name) {
	return instance->getInput (name);
}

const bool InputManager::IsPressed (AssignAddress const& addr) {
	return instance->isPressed(addr);
}
const bool InputManager::IsDown (AssignAddress const& addr) {
	return instance->isDown (addr);
}
const bool InputManager::IsReleased (AssignAddress const& addr) {
	return instance->isReleased (addr);
}

const Vec2i InputManager::GetCursorPos () {
	return instance->getCursorPos();
}
const Vec2i InputManager::GetDeltaPos (AssignAddress const& addr) {
	return instance->getDeltaPos (addr);
}
const Vec2i InputManager::GetEndPos (AssignAddress const& addr) {
	return instance->getEndPos (addr);
}

//const sf::Event* InputManager::GetEvent (AssignAddress const& addr) {
//	return instance->getEvent(name);
//}
