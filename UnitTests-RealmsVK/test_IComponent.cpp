#include "pch.h"

#include "Module/ECS/IComponent.h"

using namespace rlms;

TEST (TestComponent, Contructor) {
	IComponentPrototype proto;
	ASSERT_EQ (proto.getTypeId (), (COMPONENT_TYPE_ID)0);
}