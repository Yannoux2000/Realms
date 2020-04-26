#include "StaticMesh.h"

using namespace rlms;

const Voxel* StaticMesh::getData () const {
	return m_vxs.getData ();
}
//
//const size_t StaticMesh::getVertexCount () const {
//	return m_vxs.getSize ();
//}

const std::vector<Voxel>& StaticMesh::getVoxels () {
	return m_vxs.getVoxels ();
}

//void StaticMesh::import () {
//	VoxFileParser::Load (m_vxs, getFile());
//}

void StaticMesh::optimise () {
	m_vxs.optimise ();
}

//void StaticMesh::load () {
//}
//
//void StaticMesh::reload () {}
//
//void StaticMesh::bind () {}
//
//void StaticMesh::draw () {}
//
//void StaticMesh::unload () {}
