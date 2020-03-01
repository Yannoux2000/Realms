#include "pch.h"

#include "Module/ECS/Entity.h"
#include "Module/ECS/Entity.cpp"

#include "Module/ECS/TransformComponent.h"

using namespace rlms;

TEST (TestEntity, Contructor) {
	ASSERT_NO_THROW (
		Entity e (1);
	);
}

TEST (TestEntity, Destructor) {
	Entity* e = new Entity (1);
	ASSERT_NO_THROW (
		delete e;
	);
}

TEST (TestEntity, addComponent) {
	Entity e (1);
	IComponent c;
	TransformComponent t;

	{
		SCOPED_TRACE ("IComponent");
		ASSERT_NO_THROW (
			e.add (&c);
		);
	}

	{
		SCOPED_TRACE ("TransformComponent");
		ASSERT_NO_THROW (
			e.add (&c);
		);
	}
}

TEST (TestEntity, hasComponent) {
	Entity e (1);
	IComponent t;
	bool ret = true;

	{
		SCOPED_TRACE ("Before Add");
		ASSERT_NO_THROW (
			ret = e.has<IComponent> ();
		);
		ASSERT_EQ (ret, false);
	}

	ret = false;
	e.add (&t);

	{
		SCOPED_TRACE ("Nominal");
		ASSERT_NO_THROW (
			ret = e.has<IComponent> ();
		);
		ASSERT_EQ (ret, true);
	}

	ret = true;
	e.rem<IComponent> ();

	{
		SCOPED_TRACE ("After Remove");
		ASSERT_NO_THROW (
			ret = e.has<IComponent> ();
		);
		ASSERT_EQ (ret, false);
	}
}

TEST (TestEntity, getComponent) {
	Entity e (1);
	IComponent *c, t;

	{
		SCOPED_TRACE ("Before Add");
		ASSERT_NO_THROW (
			c = e.get<IComponent> ();
		);
		ASSERT_EQ (c, nullptr);
	}

	e.add (&t);

	{
		SCOPED_TRACE ("Nominal");
		ASSERT_NO_THROW (
			c = e.get<IComponent> ();
		);
		ASSERT_EQ (c, &t);
	}

	e.rem<IComponent> ();

	{
		SCOPED_TRACE ("After Remove");
		ASSERT_NO_THROW (
			c = e.get<IComponent> ();
		);
		ASSERT_EQ (c, nullptr);
	}
}

TEST (TestEntity, remComponent) {
	Entity e (1);
	IComponent t;

	{
		SCOPED_TRACE ("Before Add");
		ASSERT_NO_THROW (
			e.rem<IComponent> ();
		);
	}

	e.add (&t);
	{
		SCOPED_TRACE ("Nominal");
		ASSERT_NO_THROW (
			e.rem<IComponent> ();
		);
	}

	{
		SCOPED_TRACE ("After Remove");
		ASSERT_NO_THROW (
			e.rem<IComponent> ();
		);
	}
}

TEST (TestEntity, id) {
	int id = 45;
	{
		SCOPED_TRACE ("Nominal");
		Entity e1 (id);
		ASSERT_EQ (e1.id (), id);

		Entity e2 (id + 5);
		ASSERT_NE (e2.id (), id);
	}
}

TEST (TestEntity, Destructor2) {
	Entity* e = new Entity (1);
	TransformComponent* t = new TransformComponent ();
	t->position = Vec3<double> (5, 3, 1);
	e->add (t);

	{
		SCOPED_TRACE ("NonDestructionComponents");
		ASSERT_NO_THROW (
			delete e;
		);

		ASSERT_NE (t, nullptr);
		ASSERT_NO_THROW (
			ASSERT_EQ (t->position.getX (), 5);
			ASSERT_EQ (t->position.getY (), 3);
			ASSERT_EQ (t->position.getZ (), 1);
		);
	}
}

TEST (TestEntity, getComponents) {
	int id = 45;

	Entity e (id);

	IComponent c;
	TransformComponent t;
	std::vector<IComponent*> v;
	bool ret;

	{
		SCOPED_TRACE ("Empty");
		v = e.getComponents ();

		ASSERT_EQ (v.size (), 0);
		ret = false;
		for (auto it = v.begin (); it != v.end (); it++) {
			if (*it == &c) {
				ret = true;
			}
		}
		ASSERT_FALSE (ret);
	}

	e.add (&c);
	e.add (&t);

	{
		SCOPED_TRACE ("Nominal");
		ASSERT_EQ (e.has<IComponent> (), true);
		ASSERT_EQ (e.has<TransformComponent> (), true);
		ASSERT_EQ (e.get<TransformComponent> (), &t);
		ASSERT_EQ (e.get<IComponent> (), &c);

		v = e.getComponents ();

		ASSERT_EQ (v.size (), 2);
		ret = false;
		for (auto it = v.begin (); it != v.end (); it++) {
			if (*it == &c) {
				ret = true;
			}
		}
		ASSERT_EQ (ret, true);
	}
}

TEST (TestEntity, nominalUseCase) {
	int id = 45;

	Entity e (id);
	ASSERT_EQ (e.id (), id);

	IComponent ca, cb;
	TransformComponent t;

	ASSERT_EQ (e.has<IComponent> (), false);
	ASSERT_EQ (e.has<TransformComponent> (), false);
	ASSERT_EQ (e.get<IComponent> (), nullptr);
	ASSERT_EQ (e.get<TransformComponent> (), nullptr);

	e.add (&ca);

	ASSERT_EQ (e.has<IComponent> (), true);
	ASSERT_EQ (e.has<TransformComponent> (), false);
	ASSERT_NE (e.get<IComponent> (), nullptr);
	ASSERT_EQ (e.get<IComponent> (), &ca);
	ASSERT_EQ (e.get<TransformComponent> (), nullptr);

	e.add (&cb);

	ASSERT_EQ (e.has<IComponent> (), true);
	ASSERT_EQ (e.has<TransformComponent> (), false);
	ASSERT_NE (e.get<IComponent> (), nullptr);
	ASSERT_EQ (e.get<IComponent> (), &cb);
	ASSERT_EQ (e.get<TransformComponent> (), nullptr);

	e.add (&t);

	ASSERT_EQ (e.has<IComponent> (), true);
	ASSERT_EQ (e.has<TransformComponent> (), true);
	ASSERT_NE (e.get<TransformComponent> (), nullptr);
	ASSERT_EQ (e.get<TransformComponent> (), &t);

}