#include "Shader.h"

#include <vector>

Shader::Shader () : _type (), _shader () {}

Shader::Shader (int const& type, const char* const& code) : _type (type) {
}

Shader::Shader (Shader const& shader) 
	: _shader(shader._shader),
	_type (shader._type)
{}

const unsigned int& Shader::getType () {
	return _type;
}

const unsigned int& Shader::getShader () {
	return _shader;
}

std::string Shader::getCompileLog () const {
	return "";
}

Shader::~Shader() {
}