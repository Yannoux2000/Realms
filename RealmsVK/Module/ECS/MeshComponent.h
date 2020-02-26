#pragma once
#include "IComponent.h"
#include "../Graphics/MeshRegister.h"


namespace rlms{
	struct MeshComponent : public IComponent {
		IMESH_TYPE_ID mesh_id;
	};
}
