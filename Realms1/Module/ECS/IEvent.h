#pragma once

#include <typeinfo>



namespace rlms{
	class IEvent {
	private:
		const std::type_info*& _type;
	protected:
		IEvent (const std::type_info* type) : _type (type), handled(true) {};
	public:
		virtual ~IEvent () {};
		bool handled;

		const std::type_info*& getType () const {
			return _type;
		};

		template<class E> inline static bool is (IEvent& e) {
			return e.getType () == &typeid(E);
		}

		template<class E> inline static const E& as (IEvent& e) {
			return dynamic_cast<E>(e);
		}
	};
}