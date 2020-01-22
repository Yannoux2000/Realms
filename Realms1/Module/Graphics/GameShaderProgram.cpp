#include "GameShaderProgram.h"

GameShaderProgram::GameShaderProgram () 
	: IShaderProgram (),
	m_posAttrib (),
	m_cullingAttrib (),
	m_colAttrib (),
	m_bfcullingUniform (),
	m_viewUniform (),
	m_modelUniform (),
	m_projUniform (),
	m_viewPosUniform () 
{}

void GameShaderProgram::load () {
	link ();
	m_posAttrib = glGetAttribLocation (m_shaderProg, "aPos");
	m_cullingAttrib = glGetAttribLocation (m_shaderProg, "aCulling");
	m_colAttrib = glGetAttribLocation (m_shaderProg, "aCol");

	m_bfcullingUniform = glGetUniformLocation (m_shaderProg, "bfculling");
	m_viewUniform = glGetUniformLocation (m_shaderProg, "view");
	m_modelUniform = glGetUniformLocation (m_shaderProg, "model");
	m_projUniform = glGetUniformLocation (m_shaderProg, "projection");
	m_viewPosUniform = glGetUniformLocation (m_shaderProg, "viewPos");
}

void GameShaderProgram::setModelTrans (const glm::mat4& mat) {
	glUniformMatrix4fv (m_modelUniform, 1, GL_FALSE, &mat[0][0]);
}

void GameShaderProgram::setProjTrans (const glm::mat4& mat) {
	glUniformMatrix4fv (m_projUniform, 1, GL_FALSE, &mat[0][0]);
}

void GameShaderProgram::setViewTrans (const glm::mat4& mat) {
	glUniformMatrix4fv (m_viewUniform, 1, GL_FALSE, &mat[0][0]);
}

void GameShaderProgram::setViewPosTrans (glm::vec3 const& pos) {
	glUniform3fv (m_viewPosUniform, 1, &pos[0]);
}

void GameShaderProgram::setBFCulling (int const& trans) {
	glUniform1i (m_bfcullingUniform, trans);
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

		GLint p_it = glGetUniformLocation (m_shaderProg, ss.str ().c_str ());
		glUniform4fv (p_it, 4 * sizeof (float), &fpalette[0]);
	}
}



