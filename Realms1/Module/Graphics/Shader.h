#pragma once

#include "OpenGL.h"
#include "../../Base/Logging/ILogged.h"

using namespace rlms;

class Shader : public ILogged {
private:
	GLenum _type;
	GLuint _shader;
	std::string _errorLog;

public:
	Shader ();
	Shader (int const& type, const GLchar* const& code);
	Shader (Shader const& shader);

	const GLenum& getType ();
	const GLuint& getShader ();
	std::string getCompileLog () const;

	~Shader ();
};
