#pragma once
#include "IShaderProgram.h"

#include <iostream>

namespace rlms{
	class ScreenShaderProgram : public IShaderProgram {
	private:
		float quadVertices[24] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
			// positions   // texCoords
			-1.0f,  1.0f,  0.0f, 1.0f,
			-1.0f, -1.0f,  0.0f, 0.0f,
			 1.0f, -1.0f,  1.0f, 0.0f,

			-1.0f,  1.0f,  0.0f, 1.0f,
			 1.0f, -1.0f,  1.0f, 0.0f,
			 1.0f,  1.0f,  1.0f, 1.0f
		};

		unsigned int m_vao;
		unsigned int m_vbo;

		int m_posAttrib;
		int m_texCoordsAttrib;

	public:
		ScreenShaderProgram () : m_vao(), m_vbo(), m_posAttrib(), m_texCoordsAttrib() {};

		void load () override {
			link ();
		}

		inline void bind () override {
		}

		inline void draw (unsigned int const& texture) override {
		}

		inline void unbind () override {
		}
	};
}