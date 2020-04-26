#pragma once
#include "OpenGL.h"
#include "../AspectRatio.h"

namespace rlms{
	class ViewportFrameBuffer {
	private:
		unsigned int m_fbo;
		unsigned int m_rbo;
		unsigned int m_texture;

	public:
		ViewportFrameBuffer () : m_fbo (), m_rbo (), m_texture () { }

		~ViewportFrameBuffer () { }

		void load () {}
		void unload () {}
		void enable () {}

		unsigned int getTexture () {
			return m_texture;
		}

		void disable () {
			Disable ();
		}

		static inline void Disable () {
		}
	};
}

