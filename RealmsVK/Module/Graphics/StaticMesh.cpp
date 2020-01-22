#include "StaticMesh.h"


const Voxel* rlms::StaticMesh::getData () const {
	return m_vxs.getData ();
}

const size_t rlms::StaticMesh::getVertexCount () const {
	return m_vxs.getSize ();
}

const std::vector<Voxel>& rlms::StaticMesh::getVoxels () {
	return m_vxs.getVoxels ();
}

void rlms::StaticMesh::import () {
	VoxFileParser::Load (m_vxs, getFile());
}

void rlms::StaticMesh::optimise () {
	m_vxs.optimise ();
}

void rlms::StaticMesh::load () {
}

void rlms::StaticMesh::reload () {}

void rlms::StaticMesh::bind () {}

void rlms::StaticMesh::draw () {}

/* REPLACE WITH :
	shader.use();
	mesh.bind();
	shader.bind();
	shader.setModelTrans(glm::value_ptr (trans));
	mesh.draw();
	shader.unbind();
	IMesh::Unbind();
*/

void rlms::StaticMesh::unload () {}
