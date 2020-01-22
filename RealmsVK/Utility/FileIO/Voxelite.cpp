#include "Voxelite.h"
#include "../../Base/Math/VoxelMath.h"

using namespace rlms;

void rlms::Voxelite::addVoxel (char const& x, char const& y, char const& z, unsigned char const& color) {
	Voxel v;

	v.x = x;
	v.y = y;
	v.z = z;
	v.color = color;

	m_voxelsArray.push_back (v);
}

void rlms::Voxelite::setDims (unsigned char const& dim_x, unsigned char const& dim_y, unsigned char const& dim_z) {
	m_dim_x = dim_x;
	m_dim_y = dim_y;
	m_dim_z = dim_z;
}

const Voxel* rlms::Voxelite::getData () const {
	return &m_voxelsArray.front ();
}

const size_t rlms::Voxelite::getSize () const {
	return m_voxelsArray.size ();
}

const std::vector<Voxel>& rlms::Voxelite::getVoxels () {
	return m_voxelsArray;
}

const void rlms::Voxelite::optimise () {
	std::vector<std::vector<std::vector<IVoxel>>> chunk;

	char offset_x = m_dim_x / 2, offset_y = m_dim_y / 2, offset_z = m_dim_z / 2;

	chunk = std::vector<std::vector<std::vector<IVoxel>>> (m_dim_x, std::vector<std::vector<IVoxel>> (m_dim_y, std::vector<IVoxel>(m_dim_z, IVoxel())));
	for (int i = 0; i < m_voxelsArray.size (); i++) {
		Voxel& v = m_voxelsArray[i];
		IVoxel::Mask (chunk[v.x + offset_x][v.y + offset_y][v.z], IVoxel::Hidden);
	}
	for (auto it = m_voxelsArray.begin (); it != m_voxelsArray.end (); it++) {
	}
	VoxelMath::OcclusionCulling (chunk);

	std::vector<Voxel> oldVoxelsArray = m_voxelsArray;
	m_voxelsArray.clear ();

	for (int i = 0; i < oldVoxelsArray.size(); i++) {
		Voxel& v = oldVoxelsArray[i];
		v.culling = chunk[v.x + offset_x][v.y + offset_y][v.z].culling;
		if (IVoxel::HasAny (v, IVoxel::Faces)) {
			m_voxelsArray.push_back (v);
		}
	}
}
