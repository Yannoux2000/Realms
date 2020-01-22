#include "IShaderProgram.h"
#include <iostream>

IShaderProgram::IShaderProgram () : m_shaderProg() {
}

IShaderProgram::~IShaderProgram () {
}

void IShaderProgram::attach (unsigned int shader) {
}

void IShaderProgram::attach (Shader& shader) {
}

void IShaderProgram::link () {
}

void IShaderProgram::use () {
}

void IShaderProgram::detach (unsigned int shader) {
}

void IShaderProgram::detach (Shader& shader) {
}

const unsigned int& IShaderProgram::getGLenum () {
	return m_shaderProg;
};