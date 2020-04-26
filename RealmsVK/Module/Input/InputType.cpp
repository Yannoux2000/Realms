#include "InputType.h"

using namespace rlms;

void ButtonInput::flagUpdate (const bool& new_input) {
	m_pressflag = (new_input && new_input != m_holdflag); //
	m_releaseflag = (!new_input && new_input != m_holdflag);
	m_holdflag = new_input;
}

void SlideInput::slideUpdate (const bool& new_input, Vec2i new_pos) {
	flagUpdate (new_input);

	if (m_pressflag) {
		m_pos = new_pos;
		m_lastPos = Vec2i ();
	}

	if (m_holdflag) {
		m_deltaPos = (new_pos - m_pos) - m_lastPos;
		m_lastPos = new_pos - m_pos;
	}
}

const Vec2i SlideInput::getStartPos () const {
	return m_pos;
}

const Vec2i SlideInput::getEndPos () const {
	return m_lastPos;
}

const Vec2i SlideInput::getDeltaPos () const {
	return m_deltaPos;
}

//void EventAssignement::update (sf::Event* e) {
//	bool flag = false;
//	if(e !=nullptr){
//		if(e->type == m_type){
//			m_event = std::make_unique<sf::Event>(std::move(*e));
//			flag = true;
//		}
//	}
//	flagUpdate (flag);
//}

//void EventAssignement::reset () {
//	m_event.reset ();
//}

//const sf::Event* EventAssignement::getEvent () const {
//	return m_event.get();
//}
