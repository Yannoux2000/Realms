#include "IRenderer.h"

using namespace rlms;

rlms::IRenderer::IRenderer () : ILogged (), m_shader(nullptr), vertex_count (0) {}

rlms::IRenderer::~IRenderer () {
	if(m_shader != nullptr) delete m_shader;
}

IShaderProgram* rlms::IRenderer::getIShader () {
	return m_shader;
}

void rlms::IRenderer::start (std::shared_ptr<Logger> funnel) {
	startLogger (funnel);
	logger->tag (LogTags::None) << "Initializing !" << '\n';
	if (load ()) {
		logger->tag (LogTags::None) << "Initialized correctly !" << '\n';
	} else {
		logger->tag (LogTags::Error) << "Initialization Failed !" << '\n';
	}
}

void rlms::IRenderer::use () {
	m_shader->use ();
}

void rlms::IRenderer::bind (IDrawable drawable) {
	drawable.bind ();
	m_shader->bind ();
	vertex_count = drawable.getVertexCount ();
}

void rlms::IRenderer::bind (IDrawable* drawable) {
	drawable->bind ();
	m_shader->bind ();
	vertex_count = drawable->getVertexCount ();
}

void rlms::IRenderer::draw (bool const single) {
	m_shader->draw (vertex_count);
	if (single) vertex_count = 0;
}

void rlms::IRenderer::unbind () {
	m_shader->unbind ();
}

