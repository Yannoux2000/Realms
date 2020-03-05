#pragma once
#include "IComponent.h"

#include <string>
#include "../../Base/Math/Vec3.h"

namespace rlms {
	struct TransformComponent : public IComponent {
		Vec3<double> position;
	};

	class TransformComponentPrototype : public IComponentPrototype {

	};
}