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

		GLint m_posAttrib;
		GLint m_texCoordsAttrib;

	public:
		ScreenShaderProgram () : m_vao(), m_vbo(), m_posAttrib(), m_texCoordsAttrib() {};

		void load () override {
			link ();

			m_posAttrib = glGetAttribLocation (m_shaderProg, "aPos");
			m_texCoordsAttrib = glGetAttribLocation (m_shaderProg, "aTexCoords");
		}

		inline void bind () override {
			glEnableVertexAttribArray (m_posAttrib);
			try {
				std::cout << "a" << std::endl;
				openGL_Error_Poll ();
			} catch (GLException glError) {
				std::cout << m_posAttrib << std::endl;
				std::cout << glError.what () << std::endl;
			}
			glVertexAttribPointer (m_posAttrib, 2, GL_FLOAT, GL_FALSE, 4 * sizeof (float), (void*)0);
			try {
				std::cout << "a" << std::endl;
				openGL_Error_Poll ();
			} catch (GLException glError) {
				std::cout << m_posAttrib << std::endl;
				std::cout << glError.what () << std::endl;
			}
			glEnableVertexAttribArray (m_texCoordsAttrib);
			try {
				std::cout << "a" << std::endl;
				openGL_Error_Poll ();
			} catch (GLException glError) {
				std::cout << m_posAttrib << std::endl;
				std::cout << glError.what () << std::endl;
			}
			glVertexAttribPointer (m_texCoordsAttrib, 2, GL_FLOAT, GL_FALSE, 4 * sizeof (float), (void*)(2 * sizeof (float)));
			try {
				std::cout << "a" << std::endl;
				openGL_Error_Poll ();
			} catch (GLException glError) {
				std::cout << m_posAttrib << std::endl;
				std::cout << glError.what () << std::endl;
			}
		}

		inline void draw (unsigned int const& texture) override {
			try {
				std::cout << "a" << std::endl;
				openGL_Error_Poll ();
			} catch (GLException glError) {
				std::cout << m_posAttrib << std::endl;
				std::cout << glError.what () << std::endl;
			}
			glBindTexture (GL_TEXTURE_2D, texture);	// use the color attachment texture as the texture of the quad plane
			glDrawArrays (GL_TRIANGLES, 0, 6);
		}

		inline void unbind () override {
			//pareil

		}
	};
}