#pragma once
#include "OpenGL.h"

#include "../../Utility/FileIO/Voxelite.h"
#include "../../Utility/FileIO/VoxFileParser.h"
#include "IMesh.h"
#include "GameShaderProgram.h"

#include "glm/glm.hpp"
#include <glm/gtc/type_ptr.hpp>

namespace rlms {
	class StaticMesh : public IMesh {
	private:
		Voxelite m_vxs;

		GLuint _vao;
		GLuint _vbo;
	protected:
		const Voxel* getData () const;
		const size_t getVertexCount ()const override;

		const std::vector<Voxel>& getVoxels ();

	public:
		StaticMesh () : IMesh (""), _vao (), _vbo () {};
		StaticMesh (std::string filename) : IMesh (filename), _vao (), _vbo () {};

		void import () override;
		void optimise () override;

		void load () override;
		void reload () override;

		void bind () override;
		void draw () override;
		void unload () override;

		~StaticMesh () {};
	};
}
