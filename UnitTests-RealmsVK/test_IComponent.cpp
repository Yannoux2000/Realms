#include "pch.h"

#include "Module/ECS/IComponent.h"

#include "Module/ECS/TransformComponent.h"

using namespace rlms;

TEST (TestComponent, Contructor) {
	ASSERT_NO_THROW (
		IComponent c (1, 1);
	);
}