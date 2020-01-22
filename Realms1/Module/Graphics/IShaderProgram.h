#pragma once

#include "OpenGL.h"
#include "Shader.h"
#include "GLError.h"

#include "glm/glm.hpp"

class IShaderProgram {
protected:
	GLuint m_shaderProg;
public:
	IShaderProgram ();
	~IShaderProgram ();

	const GLuint& getGLenum ();

	virtual void load () {
		link ();
	};

	virtual inline void bind () {}; //GL attribs
	virtual inline void unbind () {};

	virtual inline void draw (unsigned int const& texture) {};
	
	void attach (GLuint shader);
	void attach (Shader& shader);

	void link ();
	void use ();

	void detach (GLuint shader);
	void detach (Shader& shader);

	// utility uniform functions
	// ------------------------------------------------------------------------
	inline void setBool (const std::string& name, bool value) const {
		glUniform1i (glGetUniformLocation (m_shaderProg, name.c_str ()), (int)value);
	}
	// ------------------------------------------------------------------------
	inline void setInt (const std::string& name, int value) const {
		glUniform1i (glGetUniformLocation (m_shaderProg, name.c_str ()), value);
	}
	// ------------------------------------------------------------------------
	inline void setFloat (const std::string& name, float value) const {
		glUniform1f (glGetUniformLocation (m_shaderProg, name.c_str ()), value);
	}
	// ------------------------------------------------------------------------
	inline void setVec2 (const std::string& name, const glm::vec2& value) const {
		glUniform2fv (glGetUniformLocation (m_shaderProg, name.c_str ()), 1, &value[0]);
	}
	inline void setVec2 (const std::string& name, float x, float y) const {
		glUniform2f (glGetUniformLocation (m_shaderProg, name.c_str ()), x, y);
	}
	// ------------------------------------------------------------------------
	inline void setVec3 (const std::string& name, const glm::vec3& value) const {
		glUniform3fv (glGetUniformLocation (m_shaderProg, name.c_str ()), 1, &value[0]);
	}
	inline void setVec3 (const std::string& name, float x, float y, float z) const {
		glUniform3f (glGetUniformLocation (m_shaderProg, name.c_str ()), x, y, z);
	}
	// ------------------------------------------------------------------------
	inline void setVec4 (const std::string& name, const glm::vec4& value) const {
		glUniform4fv (glGetUniformLocation (m_shaderProg, name.c_str ()), 1, &value[0]);
	}
	inline void setVec4 (const std::string& name, float x, float y, float z, float w) {
		glUniform4f (glGetUniformLocation (m_shaderProg, name.c_str ()), x, y, z, w);
	}
	// ------------------------------------------------------------------------
	inline void setMat2 (const std::string& name, const glm::mat2& mat) const {
		glUniformMatrix2fv (glGetUniformLocation (m_shaderProg, name.c_str ()), 1, GL_FALSE, &mat[0][0]);
	}
	// ------------------------------------------------------------------------
	inline void setMat3 (const std::string& name, const glm::mat3& mat) const {
		glUniformMatrix3fv (glGetUniformLocation (m_shaderProg, name.c_str ()), 1, GL_FALSE, &mat[0][0]);
	}
	// ------------------------------------------------------------------------
	inline void setMat4 (const std::string& name, const glm::mat4& mat) const {
		glUniformMatrix4fv (glGetUniformLocation (m_shaderProg, name.c_str ()), 1, GL_FALSE, &mat[0][0]);
	}
};


