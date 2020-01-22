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
	try {
		m_shader = new GameShaderProgram ();

		openGL_Error_Poll ();
		m_shader->attach (vertexShader);
		openGL_Error_Poll ();
		m_shader->attach (geometryShader);
		openGL_Error_Poll ();
		m_shader->attach (fragmentShader);
		openGL_Error_Poll ();
		m_shader->load ();
		openGL_Error_Poll ();

		logger->tag (LogTags::Info) << "Shaders load successful.\n";
	} catch (GLException glError) {
		logger->tag (LogTags::Error) << glError.what () << "\n";

		if (!glIsProgram (m_shader->getGLenum ())) logger->tag (LogTags::None) << "program is invalid.\n";
		logger->tag (LogTags::Debug) << "Shader id :" << m_shader->getGLenum() << ":\n";

		if (!glIsShader (vertexShader.getShader ())) logger->tag (LogTags::None) << "vertex shader is invalid.\n";
		logger->tag (LogTags::Debug) << "Vertex shader id : " << vertexShader.getShader () << ", " << vertexShader.getType() << ":\n" << vertexShader.getCompileLog ();

		if (!glIsShader (geometryShader.getShader ())) logger->tag (LogTags::None) << "geometry shader is invalid.\n";
		logger->tag (LogTags::Debug) << "Geometry shader id : " << geometryShader.getShader () << ", " << geometryShader.getType () << ":\n" << geometryShader.getCompileLog ();

		if (!glIsShader (fragmentShader.getShader ())) logger->tag (LogTags::None) << "fragment shader is invalid.\n";
		logger->tag (LogTags::Debug) << "Fragment shader id : " << fragmentShader.getShader () << ", " << fragmentShader.getType () << ":\n" << fragmentShader.getCompileLog ();

		return false;
	}
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
