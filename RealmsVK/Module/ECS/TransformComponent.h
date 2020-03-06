#pragma once
#include "IComponent.h"

#include <string>
#include <regex>

#include "../../Base/Math/Vec3.h"


namespace rlms {
	struct TransformComponent : public IComponent {
		Vec3<double> position;
	};

	class TransformComponentPrototype : public IComponentPrototype {
	public:
		void* Get (IComponent* const c, std::string&& member) override;
	};
}