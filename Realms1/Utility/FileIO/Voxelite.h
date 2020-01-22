#pragma once
#include <vector>
#include "../../Base/Math/IVoxel.h"

namespace rlms {
	/*
	----------------------
	| Voxel  |
	----------------------
	| 1 byte | x
	| 1 byte | y
	| 1 byte | z
	| 1 byte | cull
	| 1 byte | colorIndex
	*/

	struct Voxel : IVoxel {
		Voxel () : IVoxel (0), x (0), y (0), z (0), color (0) {}

		char x;
		char y;
		char z;
		unsigned char color;
	};

	class Voxelite {
	private:
		//dimentions
		unsigned char m_dim_x;
		unsigned char m_dim_y;
		unsigned char m_dim_z;

		std::vector<Voxel> m_voxelsArray;
	public:

		using iterator = std::vector<Voxel>::iterator;

		Voxelite () : m_voxelsArray (), m_dim_x (0), m_dim_y (0), m_dim_z (0) {}
		virtual ~Voxelite () = default;

		void addVoxel (char const& x, char const& y, char const& z, unsigned char const& color);
		void setDims (unsigned char const& dim_x, unsigned char const& dim_y, unsigned char const& dim_z);
		const Voxel* getData () const;
		const size_t getSize () const;
		const std::vector<Voxel>& getVoxels ();
		const void optimise ();
	};
}
