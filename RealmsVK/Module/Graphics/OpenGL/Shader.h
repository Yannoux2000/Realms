#pragma once

#include "OpenGL.h"
#include <string>

class Shader {
private:
	unsigned int _type;
	unsigned int _shader;

public:
	Shader ();
	Shader (int const& type, const char* const& code);
	Shader (Shader const& shader);

	const unsigned int& getType ();
	const unsigned int& getShader ();
	std::string getCompileLog () const;

	~Shader ();
};
