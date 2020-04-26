#pragma once

#include <vector>
#include <string>

#include "IDrawable.h"
#include "glm/glm.hpp"

namespace rlms {

	enum MeshType {
		None = 0,
		Static,
		Animated,
		MultiAnimated
	};

	class IMesh {
	private:
		std::string m_filename;
		MeshType type;
	public:
		IMesh (std::string filename) : m_filename (filename), type (MeshType::None) {};
		IMesh (std::string filename, MeshType t_type) : m_filename (filename), type (t_type) {};
		virtual ~IMesh () = default;

		const std::string getFile () const {
			return m_filename;
		}

		//define models voxel resolution/size
		virtual void setDims (unsigned char const& dim_x, unsigned char const& dim_y, unsigned char const& dim_z) {};

		//simplify and remove non visibles voxels
		virtual void optimise () {};

		bool isStatic () {
			return type == Static;
		}

		bool isSingleAnimated () {
			return type == Animated;
		}

		bool isMultiAnimated () {
			return type == MultiAnimated;
		}
	};
}
