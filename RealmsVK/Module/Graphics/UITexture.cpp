#include "UITexture.h"

void rlms::UITexture::load () {

	float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
		// positions   // texCoords
		-0.9f,  0.9f,  0.0f, 1.0f,
		-0.9f, -0.9f,  0.0f, 0.0f,
		 0.9f, -0.9f,  1.0f, 0.0f,

		-0.9f,  0.9f,  0.0f, 1.0f,
		 0.9f, -0.9f,  1.0f, 0.0f,
		 0.9f,  0.9f,  1.0f, 1.0f
	};

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

}

void rlms::UITexture::unload () {

}

void rlms::UITexture::setTexture (unsigned int&& texture) {
	m_texture = texture;
}

unsigned int rlms::UITexture::getTexture () {
	return m_texture;
}

void rlms::UITexture::bind () {
}

void rlms::UITexture::draw () {
	//glDrawArrays (GL_TRIANGLES, 0, 6);
}
