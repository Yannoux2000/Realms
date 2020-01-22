#include "Shader.h"

#include <vector>

Shader::Shader () : _type (), _shader (), _errorLog() {}

Shader::Shader (int const& type, const GLchar* const& code) : _type (type), _errorLog("") {
	_shader = glCreateShader (type);

	GLint status;
	glShaderSource (_shader, 1, &code, nullptr);
	glCompileShader (_shader);

	glGetShaderiv (_shader, GL_COMPILE_STATUS, &status);

	if (status != GL_TRUE) {
		GLint maxLength = 0;
		glGetShaderiv (_shader, GL_INFO_LOG_LENGTH, &maxLength);

		std::vector<GLchar> buffer (maxLength);
		glGetShaderInfoLog (_shader, maxLength, NULL, &buffer[0]);
		_errorLog = std::string (buffer.begin (), buffer.end ());
	}
}

Shader::Shader (Shader const& shader) 
	: _shader(shader._shader),
	_type (shader._type),
	_errorLog (shader._errorLog)
{}

const GLenum& Shader::getType () {
	return _type;
}

const GLuint& Shader::getShader () {
	return _shader;
}

std::string Shader::getCompileLog () const {
	return _errorLog;
}

Shader::~Shader() {
	glDeleteShader (_shader);
}