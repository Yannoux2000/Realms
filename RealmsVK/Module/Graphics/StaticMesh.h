#pragma once
#include "../../Utility/FileIO/Voxelite.h"
#include "../../Utility/FileIO/VoxFileParser.h"
#include "IMesh.h"

namespace rlms {
	class StaticMesh : public IMesh {
	private:
		Voxelite m_vxs;
	protected:
		const Voxel* getData () const;

		const std::vector<Voxel>& getVoxels ();

	public:
		StaticMesh () : IMesh ("") {};
		StaticMesh (std::string filename) : IMesh (filename) {};

		void optimise () override;

		~StaticMesh () {};
	};
}
