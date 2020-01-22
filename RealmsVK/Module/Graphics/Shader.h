#pragma once

#include "OpenGL.h"
#include "../../Base/Logging/ILogged.h"

using namespace rlms;

class Shader : public ILogged {
private:
	unsigned int _type;
	unsigned int _shader;
	std::string _errorLog;

public:
	Shader ();
	Shader (int const& type, const char* const& code);
	Shader (Shader const& shader);

	const unsigned int& getType ();
	const unsigned int& getShader ();
	std::string getCompileLog () const;

	~Shader ();
};
