#pragma once

#include <array>

#include "OpenGL.h"
#include "GLError.h"
#include "IShaderProgram.h"

#include "glm/glm.hpp"

class GameShaderProgram : public IShaderProgram {
private:
	GLint m_posAttrib;
	GLint m_cullingAttrib;
	GLint m_colAttrib;

	GLint m_bfcullingUniform;
	GLint m_viewUniform;
	GLint m_modelUniform;
	GLint m_projUniform;

	GLint m_viewPosUniform;
public:
	GameShaderProgram ();

	void load () override;

	inline void bind () override {
		glEnableVertexAttribArray (m_posAttrib);
		glVertexAttribIPointer (m_posAttrib, 3, GL_INT, 5 * sizeof (GLint), 0);
		glEnableVertexAttribArray (m_cullingAttrib);
		glVertexAttribIPointer (m_cullingAttrib, 1, GL_UNSIGNED_BYTE, 5 * sizeof (GLint), (GLvoid*)(3 * sizeof (GLint)));

		glEnableVertexAttribArray (m_colAttrib);
		glVertexAttribIPointer (m_colAttrib, 1, GL_UNSIGNED_BYTE, 5 * sizeof (GLint), (GLvoid*)(4 * sizeof (GLint)));
	}

	inline void draw (unsigned int const& num) override {
		glDrawArrays (GL_POINTS, 0, static_cast<GLsizei>(num));
	}

	inline void unbind () override {
		glDisableVertexAttribArray (m_posAttrib);
		glDisableVertexAttribArray (m_cullingAttrib);
		glDisableVertexAttribArray (m_colAttrib);
	}

	void setModelTrans (glm::mat4 const& mat);
	void setProjTrans (glm::mat4 const& mat);
	void setViewTrans (glm::mat4 const& mat);
	void setViewPosTrans (glm::vec3 const& pos);
	void setBFCulling (int const& trans);
	void setPalette (std::array<int, 256> const& palette);
};

