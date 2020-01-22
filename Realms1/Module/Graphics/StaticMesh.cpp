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
	// Create VAO
	glGenVertexArrays (1, &_vao);
	glBindVertexArray (_vao);

	// Create VBO with point coordinates
	glGenBuffers (1, &_vbo);

	auto voxels = getVoxels ();
	std::vector<GLint> points;
	for (auto it = voxels.begin (); it != voxels.end (); it++) {
		points.push_back (it->x);
		points.push_back (it->y);
		points.push_back (it->z);
		points.push_back (it->culling);
		points.push_back (it->color);
	}

	glBindBuffer (GL_ARRAY_BUFFER, _vbo);
	glBufferData (GL_ARRAY_BUFFER, points.size () * sizeof (GLint), &points.front (), GL_STATIC_DRAW);
}

void rlms::StaticMesh::reload () {
	glBindVertexArray (_vao);

	auto voxels = getVoxels ();
	std::vector<GLint> points;
	for (auto it = voxels.begin (); it != voxels.end (); it++) {
		points.push_back (it->x);
		points.push_back (it->y);
		points.push_back (it->z);
		points.push_back (it->culling);
		points.push_back (it->color);
	}

	glBindBuffer (GL_ARRAY_BUFFER, _vbo);
	glBufferData (GL_ARRAY_BUFFER, points.size () * sizeof (GLint), &points.front (), GL_STATIC_DRAW);
}

void rlms::StaticMesh::bind () {
	glBindVertexArray (_vao);
	glBindBuffer (GL_ARRAY_BUFFER, _vbo);
}

void rlms::StaticMesh::draw () {
	glDrawArrays (GL_POINTS, 0, static_cast<GLsizei>(getVertexCount ()));
}

/* REPLACE WITH :
	shader.use();
	mesh.bind();
	shader.bind();
	shader.setModelTrans(glm::value_ptr (trans));
	mesh.draw();
	shader.unbind();
	IMesh::Unbind();
*/

void rlms::StaticMesh::unload () {
	glDeleteBuffers (1, &_vbo);
	glDeleteVertexArrays (1, &_vao);
}
