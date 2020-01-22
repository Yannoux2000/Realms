#include "UITexture.h"

void rlms::UITexture::load () {
	glGenVertexArrays (1, &m_vao);
	glGenBuffers (1, &m_vbo);

	float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
		// positions   // texCoords
		-0.9f,  0.9f,  0.0f, 1.0f,
		-0.9f, -0.9f,  0.0f, 0.0f,
		 0.9f, -0.9f,  1.0f, 0.0f,

		-0.9f,  0.9f,  0.0f, 1.0f,
		 0.9f, -0.9f,  1.0f, 0.0f,
		 0.9f,  0.9f,  1.0f, 1.0f
	};

	glBindVertexArray (m_vao);
	glBindBuffer (GL_ARRAY_BUFFER, m_vbo);
	glBufferData (GL_ARRAY_BUFFER, sizeof (quadVertices), &quadVertices, GL_STATIC_DRAW);
}

void rlms::UITexture::reload () {
	float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
		// positions   // texCoords
		-0.9f,  0.9f,  0.0f, 1.0f,
		-0.9f, -0.9f,  0.0f, 0.0f,
		 0.9f, -0.9f,  1.0f, 0.0f,

		-0.9f,  0.9f,  0.0f, 1.0f,
		 0.9f, -0.9f,  1.0f, 0.0f,
		 0.9f,  0.9f,  1.0f, 1.0f
	};

	glBindVertexArray (m_vao);
	glBindBuffer (GL_ARRAY_BUFFER, m_vbo);
	glBufferData (GL_ARRAY_BUFFER, sizeof (quadVertices), &quadVertices, GL_STATIC_DRAW);
}

void rlms::UITexture::unload () {

}

void rlms::UITexture::setTexture (GLuint&& texture) {
	m_texture = texture;
}

GLuint rlms::UITexture::getTexture () {
	return m_texture;
}

void rlms::UITexture::bind () {
	glBindVertexArray (m_vao);
	glBindBuffer (GL_ARRAY_BUFFER, m_vbo);
}

void rlms::UITexture::draw () {
	//glDrawArrays (GL_TRIANGLES, 0, 6);
}
