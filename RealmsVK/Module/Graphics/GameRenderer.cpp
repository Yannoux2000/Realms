#include "GameRenderer.h"

rlms::GameRenderer::GameRenderer () : IRenderer() {}

rlms::GameRenderer::~GameRenderer () {}

void GameRenderer::setVexShader (Shader && vexShdr) {
	vertexShader = vexShdr;
}

void GameRenderer::setGeoShader (Shader && geoShdr) {
	geometryShader = geoShdr;
}

void GameRenderer::setFrgShader (Shader && frgShdr) {
	fragmentShader = frgShdr;
}

bool GameRenderer::load () {
	return true;
}

void rlms::GameRenderer::setPalette (std::array<int, 256> & palette) {
	getShader()->setPalette (palette);
}

void rlms::GameRenderer::setCamera (Camera* const& cam) {
	getShader ()->setViewPosTrans (cam->position);
	getShader ()->setViewTrans (cam->getMainView());
	getShader ()->setProjTrans (cam->getPerspective ());
	getShader ()->setBFCulling (cam->getBFCullingflags());
}

void rlms::GameRenderer::setModelTrans (glm::mat4 const& mat) {
	getShader ()->setModelTrans (mat);
}
