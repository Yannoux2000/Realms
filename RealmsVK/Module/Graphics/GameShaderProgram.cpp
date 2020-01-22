#include "GameShaderProgram.h"

GameShaderProgram::GameShaderProgram () 
	: IShaderProgram ()
{}

void GameShaderProgram::load () {
}

void GameShaderProgram::setModelTrans (const glm::mat4& mat) {
}

void GameShaderProgram::setProjTrans (const glm::mat4& mat) {
}

void GameShaderProgram::setViewTrans (const glm::mat4& mat) {
}

void GameShaderProgram::setViewPosTrans (glm::vec3 const& pos) {
}

void GameShaderProgram::setBFCulling (int const& trans) {
}

void GameShaderProgram::setPalette (std::array<int, 256> const& palette) {

	std::array<float, 4> fpalette;
	size_t j;
	for (size_t i = 0; i < palette.size (); i++) {
		j = i * 4;
		int color = palette[i];

		fpalette[0] = static_cast<float>(color & 0xFF) / 255; //extract first byte
		fpalette[1] = static_cast<float>((color >> 8) & 0xFF) / 255; //extract second byte
		fpalette[2] = static_cast<float>((color >> 16) & 0xFF) / 255; //extract third byte
		fpalette[3] = static_cast<float>((color >> 24) & 0xFF) / 255; //extract fourth byte

		std::stringstream ss;
		ss << "palette[" << i << "]";

		//GLint p_it = glGetUniformLocation (m_shaderProg, ss.str ().c_str ());
		//glUniform4fv (p_it, 4 * sizeof (float), &fpalette[0]);
	}
}
