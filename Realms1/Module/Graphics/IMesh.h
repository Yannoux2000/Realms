#pragma once

#include <vector>
#include <string>

#include "GameShaderProgram.h"
#include "IDrawable.h"
#include "glm/glm.hpp"

namespace rlms {
	class IMesh : public IDrawable {
	private:
		std::string m_filename;

	public:
		IMesh (std::string filename) : m_filename (filename) {};
		virtual ~IMesh () = default;

		const std::string getFile () const {
			return m_filename;
		}

		virtual size_t getSize () {
			return sizeof (IMesh);
		}

		//define models voxel resolution/size
		virtual void setDims (unsigned char const& dim_x, unsigned char const& dim_y, unsigned char const& dim_z) {};

		//import data from file.
		virtual void import () {};

		//simplify and remove non visibles voxels
		virtual void optimise () {};

		//old function
		virtual void draw (GameShaderProgram& shader, glm::mat4& trans) {};
		
		//either the mesh have animations or not
		virtual bool animated () { return false; }
	};
}
