#include "InputAssignement.h"

void rlms::InputAssignement::flagUpdate (const bool& new_input) {
	m_pressflag = (new_input && new_input != m_holdflag); //
	m_releaseflag = (!new_input && new_input != m_holdflag);
	m_holdflag = new_input;
}

void rlms::SlideInputAssignement::slideUpdate (const bool& new_input, rlms::Vec2i new_pos) {
	flagUpdate (new_input);

	if (m_pressflag) {
		m_pos = new_pos;
		m_lastPos = rlms::Vec2i ();
	}

	if (m_holdflag) {
		m_deltaPos = (new_pos - m_pos) - m_lastPos;
		m_lastPos = new_pos - m_pos;
	}
}

const rlms::Vec2i rlms::SlideInputAssignement::getStartPos () const {
	return m_pos;
}

const rlms::Vec2i rlms::SlideInputAssignement::getEndPos () const {
	return m_lastPos;
}

const rlms::Vec2i rlms::SlideInputAssignement::getDeltaPos () const {
	return m_deltaPos;
}

//void rlms::EventAssignement::update (sf::Event* e) {
//	bool flag = false;
//	if(e !=nullptr){
//		if(e->type == m_type){
//			m_event = std::make_unique<sf::Event>(std::move(*e));
//			flag = true;
//		}
//	}
//	flagUpdate (flag);
//}

//void rlms::EventAssignement::reset () {
//	m_event.reset ();
//}

//const sf::Event* rlms::EventAssignement::getEvent () const {
//	return m_event.get();
//}
