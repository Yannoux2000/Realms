#pragma once

namespace rlms {
	class IDrawable {
	public:
		virtual const size_t getVertexCount () const {
			return 0;
		}

		//placing data in the GRam
		virtual void load () {};
		//replacing data in the GRam
		virtual void reload () {};
		//removing data from the GRam
		virtual void unload () {};

		//function called in frameloop encapsulate glBinds functions
		virtual void bind () {};
		//function called in frameloop encapsulate glDrawArrays
		virtual void draw () {};
		//function called in frameloop encapsulate glBinds functions
		virtual void unbind () {};

		virtual ~IDrawable () {};
	};
}
