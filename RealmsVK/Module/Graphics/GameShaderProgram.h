#pragma once

#include <array>

#include "OpenGL.h"
#include "GLError.h"
#include "IShaderProgram.h"

#include "glm/glm.hpp"

class GameShaderProgram : public IShaderProgram {
private:

public:
	GameShaderProgram ();

	void load () override;

	inline void bind () override {
	}

	inline void draw (unsigned int const& num) override {
	}

	inline void unbind () override {
	}

	void setModelTrans (glm::mat4 const& mat);
	void setProjTrans (glm::mat4 const& mat);
	void setViewTrans (glm::mat4 const& mat);
	void setViewPosTrans (glm::vec3 const& pos);
	void setBFCulling (int const& trans);
	void setPalette (std::array<int, 256> const& palette);
};

