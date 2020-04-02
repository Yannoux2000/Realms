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

	rlms::Vec2i m_cursor_pos, m_scroll_pos;
	std::unordered_map<INPUT_ADDRESS_SUB_TYPE, rlms::InputMap*> m_maps;
	AssignDictionnary dict;
	std::string text_typed;
	bool m_typing;

	bool start (std::shared_ptr<Logger> funnel);
	void stop ();

	//get will create a new map if no one is found
	std::unordered_map<INPUT_ADDRESS_SUB_TYPE, rlms::InputMap*>::iterator __getMap (INPUT_ADDRESS_SUB_TYPE map);

	//find will search for an existing map, otherwise throws an error
	std::unordered_map<INPUT_ADDRESS_SUB_TYPE, rlms::InputMap*>::iterator __findMap (INPUT_ADDRESS_SUB_TYPE map);

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

	rlms::Vec2i getCursorPos ();
	const rlms::Vec2i getDeltaPos (AssignAddress const& addr);
	const rlms::Vec2i getEndPos (AssignAddress const& addr);
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

bool rlms::InputManagerImpl::start (std::shared_ptr<Logger> funnel) {
	startLogger (funnel);
	logger->tag (LogTags::None) << "Initializing !" << '\n';
	dict.reset ();
	logger->tag (LogTags::None) << "Initialized correctly !" << '\n';
	return true;
}
void rlms::InputManagerImpl::stop () {
	logger->tag (LogTags::None) << "Terminating" << '\n';
	clearInputs ();
	logger->tag (LogTags::None) << "Terminated correctly !" << '\n';
}

//get will create a new map if no one is found
std::unordered_map<INPUT_ADDRESS_SUB_TYPE, rlms::InputMap*>::iterator InputManagerImpl::__getMap (INPUT_ADDRESS_SUB_TYPE map) {
	auto it = m_maps.find (map);
	if (it == m_maps.end ()) {
		logger->tag (LogTags::Info) << "Creating map(" << map << ").\n";
		m_maps.insert (std::make_pair (map, new InputMap ()));
		it = m_maps.find (map);
	}
	return it;
}

//find will search for an existing map, otherwise throws an error
std::unordered_map<INPUT_ADDRESS_SUB_TYPE, rlms::InputMap*>::iterator InputManagerImpl::__findMap (INPUT_ADDRESS_SUB_TYPE map) {
	auto it = m_maps.find (map);
	if (it == m_maps.end ()) {
		throw InvalidFormat ();
		return m_maps.begin ();
	}
	return it;
}

void rlms::InputManagerImpl::bindKey (std::string const& name, int && key) {
	dict.set (name);
	AssignAddress addr = dict.get (name);

	auto it = __getMap (addr.map);
	it->second->bindKey (addr.input, std::move (key));
}
void rlms::InputManagerImpl::bindMouseButton (std::string const& name, int && button) {
	dict.set (name);
	AssignAddress addr = dict.get (name);

	auto it = __getMap (addr.map);
	it->second->bindKey (addr.input, std::move (button));
}
void rlms::InputManagerImpl::bindKeySlide (std::string const& name, int && key) {
	dict.set (name);
	AssignAddress addr = dict.get (name);

	auto it = __getMap (addr.map);
	it->second->bindKeySlide (addr.input, std::move (key));
}
void rlms::InputManagerImpl::bindMouseButtonSlide (std::string const& name, int && button) {
	dict.set (name);
	AssignAddress addr = dict.get (name);

	auto it = __getMap (addr.map);
	it->second->bindKeySlide (addr.input, std::move (button));
}
//void rlms::InputManagerImpl::bindEvent (std::string const& name, sf::Event::EventType && e) {
//	dict.set (name);
//	AssignAddress addr = dict.get (name);
//
//	auto it = __getMap (addr.map);
//	it->second->bindKeySlide (addr.input, std::move (button));
//	it->second->bindEvent (inputName, std::move (e));
//	}
//}

void rlms::InputManagerImpl::enableInputMap (INPUT_ADDRESS_SUB_TYPE const& map) {
	logger->tag (LogTags::Info) << "Enabled Map : " << map << ".\n";
	auto it = __findMap (map);
	it->second->enable ();
}

void rlms::InputManagerImpl::disableInputMap (INPUT_ADDRESS_SUB_TYPE const& map) {
	logger->tag (LogTags::Info) << "Disabled Map : " << map << ".\n";
	auto it = __findMap (map);
	it->second->disable ();
}

void rlms::InputManagerImpl::unbindInput (AssignAddress const& addr) {
	auto it = __findMap (addr.map);
	it->second->unbindInput (addr.input);
}

void rlms::InputManagerImpl::clearInputs () {
	logger->tag (LogTags::Warn) << "Clearing all Inputs.\n";
	while (!m_maps.empty ()) {
		auto it = m_maps.begin ();
		it->second->clearInputs ();
		delete it->second;
		m_maps.erase (it);
	}
	dict.reset ();
}
void rlms::InputManagerImpl::poll (GLFWwindow*& window) {
	//logger->tag (LogTags::None) << "Polling new Inputs" << '\n';
	glfwPollEvents ();
}

void rlms::InputManagerImpl::update (Input&& n) {
	logger->tag (LogTags::None) << "update ! ("<< n.scancode << ", " << static_cast<char>(glfwGetKeyScancode(n.scancode)) << ")" << '\n';
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
	AssignAddress addr = dict.get (name);
	if (!dict.valid (addr)) {
		logger->tag (LogTags::Warn) << "invalid input address : " << name << "\n";
	}
	return dict.valid (addr);
}
const bool rlms::InputManagerImpl::hasInput (AssignAddress const& addr) {
	auto it = __findMap (addr.map);
	return it->second->hasInput (addr.input);
}
const AssignAddress rlms::InputManagerImpl::getInput (std::string const& name) {
	AssignAddress addr = dict.get (name);
	if (!dict.valid (addr)) {
		logger->tag (LogTags::Warn) << "invalid input address : " << name << "\n";
	}
	return addr;
}

rlms::Vec2i rlms::InputManagerImpl::getCursorPos () {
	return m_cursor_pos;
}
const bool rlms::InputManagerImpl::isPressed (AssignAddress const& addr) {
	auto it = __findMap (addr.map);
	return it->second->isPressed (addr.input);
}
const bool rlms::InputManagerImpl::isDown (AssignAddress const& addr) {
	auto it = __findMap (addr.map);
	return it->second->isDown (addr.input);
}
const bool rlms::InputManagerImpl::isReleased (AssignAddress const& addr) {
	auto it = __findMap (addr.map);
	return it->second->isReleased (addr.input);
}

const rlms::Vec2i rlms::InputManagerImpl::getDeltaPos (AssignAddress const& addr) {
	auto it = __findMap (addr.map);
	return it->second->getDeltaPos (addr.input);
}
const rlms::Vec2i rlms::InputManagerImpl::getEndPos (AssignAddress const& addr) {
	auto it = __findMap (addr.map);
	return it->second->getEndPos (addr.input);
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

void rlms::InputManager::EnableInputMap (INPUT_ADDRESS_SUB_TYPE const& map) {
	instance->enableInputMap(map);
}

void rlms::InputManager::EnableInputMap (std::string const& name) {
	instance->enableInputMap(instance->dict.getWord (name));
}

void rlms::InputManager::DisableInputMap (INPUT_ADDRESS_SUB_TYPE const& map) {
	instance->disableInputMap (map);
}

void rlms::InputManager::DisableInputMap (std::string const& name) {
	instance->disableInputMap (instance->dict.getWord (name));
}

void rlms::InputManager::UnBindInput (AssignAddress const& addr) {
	instance->unbindInput (addr);
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
const bool rlms::InputManager::HasInput (AssignAddress const& addr) {
	return instance->hasInput (addr);
}
const AssignAddress rlms::InputManager::GetInput (std::string const& name) {
	return instance->getInput (name);
}

const bool rlms::InputManager::IsPressed (AssignAddress const& addr) {
	return instance->isPressed(addr);
}
const bool rlms::InputManager::IsDown (AssignAddress const& addr) {
	return instance->isDown (addr);
}
const bool rlms::InputManager::IsReleased (AssignAddress const& addr) {
	return instance->isReleased (addr);
}

const rlms::Vec2i rlms::InputManager::GetCursorPos () {
	return instance->getCursorPos();
}
const rlms::Vec2i rlms::InputManager::GetDeltaPos (AssignAddress const& addr) {
	return instance->getDeltaPos (addr);
}
const rlms::Vec2i rlms::InputManager::GetEndPos (AssignAddress const& addr) {
	return instance->getEndPos (addr);
}

//const sf::Event* rlms::InputManager::GetEvent (AssignAddress const& addr) {
//	return instance->getEvent(name);
//}
