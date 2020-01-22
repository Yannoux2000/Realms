#include "Shader.h"

#include <vector>

Shader::Shader () : _type (), _shader (), _errorLog() {}

Shader::Shader (int const& type, const char* const& code) : _type (type), _errorLog("") {
}

Shader::Shader (Shader const& shader) 
	: _shader(shader._shader),
	_type (shader._type),
	_errorLog (shader._errorLog)
{}

const unsigned int& Shader::getType () {
	return _type;
}

const unsigned int& Shader::getShader () {
	return _shader;
}

std::string Shader::getCompileLog () const {
	return _errorLog;
}

Shader::~Shader() {
}