#include "IComponent.h"

using namespace rlms;

IComponentPrototype* IComponent::c_proto;

void IComponentPrototype::Destroy (Allocator* const& alloc, IComponent* const& c) {
	c->~IComponent ();
	alloc->deallocate (c);
}
