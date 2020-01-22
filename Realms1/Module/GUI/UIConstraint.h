#pragma once
#include "../../Base/Math/Rect.h"

#include <array>

namespace rlms {
	struct IConstraint {
		double& var;
		double& parent_var;
		virtual void apply(Rect<double>const& parent) {}
	};

	struct PixelConstraint : public IConstraint{
		int pixels;
		void apply (Rect<double>const& parent) override {
			
		}
	};

	class UIConstraints {
	private:
		IConstraint* m_const_x;
		IConstraint* m_const_y;
		IConstraint* m_const_w;
		IConstraint* m_const_h;
	public:

		UIConstraints () : m_const_x (), m_const_y (), m_const_w (), m_const_h () {}

		template<class C> void setX (C*& c) {
			m_const_x = c;
		}


		void operator() (Rect<double>& parent) {
			Rect<double> child (parent);

			m_const_x->apply (parent);
		}
	};


}
