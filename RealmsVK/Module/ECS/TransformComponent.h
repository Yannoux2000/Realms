#pragma once
#include "IComponent.h"

#include <string>
#include <regex>

#include "../../Base/Math/Vec3.h"

namespace rlms {
	struct TransformComponent : public IComponent {
		Vec3<double> position;
		TransformComponent (ENTITY_ID const& entity_id, COMPONENT_ID const& component_id, COMPONENT_TYPE_ID const& type_id) : position(), IComponent(entity_id, component_id, type_id) {}
	};

	class TransformComponentPrototype : public IComponentPrototype {
	public:
		TransformComponentPrototype () : IComponentPrototype ("Transform", sizeof (TransformComponent), alignof(TransformComponent)) {}
		~TransformComponentPrototype() {}

		void Populate (IComponent* const& c) override {
			auto tc = (TransformComponent*) c;
			tc->position = Vec3<double> (0,0,0);
		}

		void* GetAttrib (IComponent* const& c, std::string& member) override {
			if (IBase::is<TransformComponent> (c)) {
				if (member == "pos" || member == "position") {
					return &(IBase::to<TransformComponent> (c)->position);
				}
			}

			return nullptr;
		}
	};
}