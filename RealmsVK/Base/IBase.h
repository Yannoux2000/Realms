#pragma once

namespace rlms {
	class IBase {
	public:
		virtual ~IBase () {};

		template<class b> inline static bool is (IBase* o) {
			return (dynamic_cast<b>(o) != nullptr);
		}

		template<class b> inline static b to (IBase* o) {
			return dynamic_cast<b>(o);
		}
	};
}
