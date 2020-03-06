#include "TransformComponent.h"

void* rlms::TransformComponentPrototype::Get (IComponent* const c, std::string&& member) {

	if(IBase::is<TransformComponent>(c)){
		if (std::regex_match (member, std::regex ("[(vec)(vector)]"))) {
			return &(IBase::to<TransformComponent>(c)->position);
		}
	}

	return _Get (c, member);
}
