#pragma once
#include "ECS/IComponent.h"
#include "Vec3.h"

namespace rlms {
	struct TransformComponent : public IComponent {
		Vec3f pos;
		Vec3f rotation;
	};
}