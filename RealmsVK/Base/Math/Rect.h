#pragma once
#include "Vec2.h"

namespace rlms {
	enum AnchorMode {
		top_left = 0,
		top_mid = 1,
		top_right = 2,
		mid_left = 3,
		center = 4,
		mid_right = 5,
		bot_left = 6,
		bot_mid = 7,
		bot_right = 8
	};

	template <typename T> class Rect {
	private:
		AnchorMode m_mode;
		Vec2<T> m_anchor;	//anchor position
		Vec2<T> m_area;		//height & width
	public:

		Rect () : m_mode() ,m_anchor (), m_area () {}
		Rect (Vec2<T> t_anchor = Vec2<T>(), Vec2<T> t_area = Vec2<T> (), AnchorMode t_mode = AnchorMode::top_left) : m_mode (t_mode), m_anchor (t_anchor), m_area (t_area) {}

		void setMode (AnchorMode const& t_mode) {
			m_mode = t_mode;
		}

		const AnchorMode getMode () {
			return m_mode;
		}

		void setAnchor (Vec2<T> const& t_anchor) {
			m_anchor = t_anchor;
		}

		Vec2<T> getAnchor () {
			return m_anchor;
		}

		T& X () {
			return m_anchor.getX();
		}

		T& Y () {
			return m_anchor.getY();
		}

		T& W () {
			return m_area.getX();
		}

		T& H () {
			return m_area.getY();
		}

		//Lua loading
		static Rect Parse (std::string const& s) {
			Rect r;
			return r;
		}

		static std::string Serialize (Rect& r) {
			std::string s("");
			return s;
		}
	};
}
