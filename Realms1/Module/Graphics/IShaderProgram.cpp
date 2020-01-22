#include "IShaderProgram.h"
#include <iostream>

IShaderProgram::IShaderProgram () : m_shaderProg() {
	m_shaderProg = glCreateProgram ();
}

IShaderProgram::~IShaderProgram () {
	glDeleteProgram (m_shaderProg);
}

void IShaderProgram::attach (GLuint shader) {
	glAttachShader (m_shaderProg, shader);
}

void IShaderProgram::attach (Shader& shader) {
	glAttachShader (m_shaderProg, shader.getShader ());
}

void IShaderProgram::link () {
	glLinkProgram (m_shaderProg);
}

void IShaderProgram::use () {
	glUseProgram (m_shaderProg);
}

void IShaderProgram::detach (GLuint shader) {
	glDetachShader (m_shaderProg, shader);
}

void IShaderProgram::detach (Shader& shader) {
	glDetachShader (m_shaderProg, shader.getShader ());
}

const GLuint& IShaderProgram::getGLenum () {
	return m_shaderProg;
};