#pragma once
#include "IRenderer.h"
#include "Camera.h"
#include "GameShaderProgram.h"

#include "glm/glm.hpp"

namespace rlms {
	class GameRenderer : public IRenderer {
	private:
		Shader vertexShader;
		Shader geometryShader;
		Shader fragmentShader;

		std::string getLogName () override {
			return "GameRenderer";
		};

	public:
		GameShaderProgram* getShader () {
			return (GameShaderProgram*)(m_shader);
		}

		GameRenderer ();
		~GameRenderer ();

		void setVexShader (Shader && vexShdr);
		void setGeoShader (Shader && geoShdr);
		void setFrgShader (Shader && frgShdr);

		bool load () override;

		void setPalette (std::array<int, 256> & palette);
		void setCamera (Camera* const& cam);
		void setModelTrans (glm::mat4 const& mat);
	};
}
