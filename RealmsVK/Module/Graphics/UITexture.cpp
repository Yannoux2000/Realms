#include "UITexture.h"

using namespace rlms;

void UITexture::load () {

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

void UITexture::reload () {
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

void UITexture::unload () {

}

void UITexture::setTexture (unsigned int&& texture) {
	m_texture = texture;
}

unsigned int UITexture::getTexture () {
	return m_texture;
}

void UITexture::bind () {
}

void UITexture::draw () {
	//glDrawArrays (GL_TRIANGLES, 0, 6);
}
