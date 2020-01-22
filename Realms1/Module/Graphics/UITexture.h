#pragma once
#include "IDrawable.h"
#include <glm/glm.hpp>

namespace rlms{
	class UITexture : public IDrawable {
	private:
		//glm::vec4 m_borders; //
		glm::vec2 m_posA;
		glm::vec2 m_posB;
		glm::vec2 m_UVA;
		glm::vec2 m_UVB;

		GLuint m_vao, m_vbo;

		GLuint m_texture;

	public:
		UITexture () : m_posA (glm::vec2 (-1.0f, -1.0f)), m_posB (glm::vec2 (1.0f, 1.0f)), m_UVA (glm::vec2 (0.0f, 0.0f)), m_UVB (glm::vec2 (1.0f, 1.0f)) {}
		UITexture (glm::vec2 t_posA, glm::vec2 t_posB) : m_posA (t_posA), m_posB (t_posB), m_UVA (glm::vec2 (0.0f, 0.0f)), m_UVB (glm::vec2 (1.0f, 1.0f)) {}
		UITexture (glm::vec2 t_posA, glm::vec2 t_posB, glm::vec2 t_UVA, glm::vec2 t_UVB) : m_posA (t_posA), m_posB (t_posB), m_UVA (t_UVA), m_UVB (t_UVB) {}

		void load () override;
		void reload () override;
		void unload () override;

		void setTexture (GLuint&& texture);
		GLuint getTexture ();

		void bind () override;
		void draw () override;
	};
}
