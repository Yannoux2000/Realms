#pragma once

#include "OpenGL.h"
#include "Shader.h"
#include "GLError.h"

#include "glm/glm.hpp"

class IShaderProgram {
protected:
	unsigned int m_shaderProg;
public:
	IShaderProgram ();
	~IShaderProgram ();

	const unsigned int& getGLenum ();

	virtual void load () {
		link ();
	};

	virtual inline void bind () {}; //GL attribs
	virtual inline void unbind () {};

	virtual inline void draw (unsigned int const& texture) {};
	
	void attach (unsigned int shader);
	void attach (Shader& shader);

	void link ();
	void use ();

	void detach (unsigned int shader);
	void detach (Shader& shader);

	// utility uniform functions
	// ------------------------------------------------------------------------
	inline void setBool (const std::string& name, bool value) const {
	}
	// ------------------------------------------------------------------------
	inline void setInt (const std::string& name, int value) const {
	}
	// ------------------------------------------------------------------------
	inline void setFloat (const std::string& name, float value) const {
	}
	// ------------------------------------------------------------------------
	inline void setVec2 (const std::string& name, const glm::vec2& value) const {
	}
	inline void setVec2 (const std::string& name, float x, float y) const {
	}
	// ------------------------------------------------------------------------
	inline void setVec3 (const std::string& name, const glm::vec3& value) const {
	}
	inline void setVec3 (const std::string& name, float x, float y, float z) const {
	}
	// ------------------------------------------------------------------------
	inline void setVec4 (const std::string& name, const glm::vec4& value) const {
	}
	inline void setVec4 (const std::string& name, float x, float y, float z, float w) {
	}
	// ------------------------------------------------------------------------
	inline void setMat2 (const std::string& name, const glm::mat2& mat) const {
	}
	// ------------------------------------------------------------------------
	inline void setMat3 (const std::string& name, const glm::mat3& mat) const {
	}
	// ------------------------------------------------------------------------
	inline void setMat4 (const std::string& name, const glm::mat4& mat) const {
	}
};


