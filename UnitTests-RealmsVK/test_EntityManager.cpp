#include "pch.h"
#include "pch_loggers.h"

#include "Base/Allocators/MasqueradeAllocator.h"
#include "Module/ECS/EntityManager.h"
#include "Module/ECS/EntityManager.cpp"

class TestEntityManager : public ::testing::Test {
protected:
	static constexpr size_t size_mem = 4096;
	static void* mem_ptr;

	virtual void SetUp () {
		mem_ptr = malloc (size_mem);
		rlms::MasqueradeAllocator alloc (size_mem, mem_ptr);
		rlms::EntityManager::Initialize (&alloc, size_mem);
	}

	virtual void TearDown () {
		rlms::EntityManager::Terminate ();
		free (mem_ptr);
	}
};

void* TestEntityManager::mem_ptr;

TEST_F (TestEntityManager, proxy_test) {
	ASSERT_TRUE (true);
}

TEST_F (TestEntityManager, isValid) {

	ASSERT_EQ (rlms::EntityManager::n_errors, 0);
	{
		SCOPED_TRACE ("Null ID");
		ASSERT_FALSE (rlms::EntityManager::isValid (rlms::Entity::NULL_ID));
	}

	{
		SCOPED_TRACE ("Nominal_1");
		ASSERT_TRUE (rlms::EntityManager::isValid (1));
	}

	{
		SCOPED_TRACE ("Nominal_Max");
		ASSERT_TRUE (rlms::EntityManager::isValid (65525));
	}
	ASSERT_EQ (rlms::EntityManager::n_errors, 0);
}

//static const ENTITY_ID Create ();
TEST_F (TestEntityManager, CreateEntity) {
	rlms::ENTITY_ID e_id1 = rlms::Entity::NULL_ID;
	rlms::ENTITY_ID e_id2 = rlms::Entity::NULL_ID;

	{
		SCOPED_TRACE ("Nominal_1");
		ASSERT_NO_THROW (
			e_id1 = rlms::EntityManager::Create ();
		);

		ASSERT_NE (e_id1, rlms::Entity::NULL_ID);
		ASSERT_TRUE (rlms::EntityManager::isValid (e_id1));
		ASSERT_EQ (rlms::EntityManager::n_errors, 0);
	}

	{
		SCOPED_TRACE ("Nominal_2");
		ASSERT_NO_THROW (
			e_id2 = rlms::EntityManager::Create ();
		);

		ASSERT_NE (e_id2, rlms::Entity::NULL_ID);
		ASSERT_NE (e_id2, e_id1);
		ASSERT_TRUE (rlms::EntityManager::isValid (e_id2));
		ASSERT_EQ (rlms::EntityManager::n_errors, 0);
	}
}

////////////////////////////////////////
// CASE FOR MEMORY OVERFLOW
////////////////////////////////////////

//static const ENTITY_ID Create (ENTITY_ID id);
TEST_F (TestEntityManager, CreateEntityWithID) {
	rlms::ENTITY_ID e_id_asked = 40;
	rlms::ENTITY_ID e_id_created = rlms::Entity::NULL_ID;

	{
		SCOPED_TRACE ("Nominal");
		ASSERT_TRUE (rlms::EntityManager::isValid (e_id_asked));

		ASSERT_NO_THROW (
			e_id_created = rlms::EntityManager::Create (e_id_asked);
		);

		ASSERT_NE (e_id_created, rlms::Entity::NULL_ID);
		ASSERT_EQ (e_id_created, e_id_asked);
		ASSERT_TRUE (rlms::EntityManager::isValid (e_id_created));
		ASSERT_EQ (rlms::EntityManager::n_errors, 0);
	}

	{
		SCOPED_TRACE ("Override");
		ASSERT_NO_THROW (
			e_id_created = rlms::EntityManager::Create (e_id_asked);
		);

		ASSERT_EQ (e_id_created, rlms::Entity::NULL_ID);
		ASSERT_NE (e_id_created, e_id_asked);
		ASSERT_FALSE (rlms::EntityManager::isValid (e_id_created));
		ASSERT_EQ (rlms::EntityManager::n_errors, 1);
	}

	{
		SCOPED_TRACE ("Invalid ID");
		ASSERT_NO_THROW (
			e_id_created = rlms::EntityManager::Create (rlms::Entity::NULL_ID);
		);

		ASSERT_EQ (e_id_created, rlms::Entity::NULL_ID);
		ASSERT_NE (e_id_created, e_id_asked);
		ASSERT_FALSE (rlms::EntityManager::isValid (e_id_created));
		ASSERT_EQ (rlms::EntityManager::n_errors, 2);

	}
}

//static bool Has (ENTITY_ID id);
TEST_F (TestEntityManager, HasEntity) {
	rlms::ENTITY_ID e_id1 = 600;
	bool e = true;

	{
		SCOPED_TRACE ("PreConstruction");
		ASSERT_NO_THROW (
			e = rlms::EntityManager::Has (e_id1)
		);
		ASSERT_FALSE (e);
		ASSERT_EQ (rlms::EntityManager::n_errors, 0);
	}

	e_id1 = rlms::EntityManager::Create (e_id1);

	{
		SCOPED_TRACE ("Nominal");
		ASSERT_NO_THROW (
			e = rlms::EntityManager::Has (e_id1)
		);
		ASSERT_TRUE (e);
		ASSERT_EQ (rlms::EntityManager::n_errors, 0);
	}

	{
		SCOPED_TRACE ("Invalid ID");
		ASSERT_NO_THROW (
			e = rlms::EntityManager::Has (rlms::Entity::NULL_ID);
		);
		ASSERT_FALSE (e);
		ASSERT_EQ (rlms::EntityManager::n_errors, 1);
	}
}

//static Entity* Get (ENTITY_ID id);
TEST_F (TestEntityManager, GetEntity) {
	rlms::ENTITY_ID e_id1 = 40;
	rlms::Entity* e = nullptr;

	{
		SCOPED_TRACE ("PreConstruction");
		ASSERT_EQ (rlms::EntityManager::n_errors, 0);
		ASSERT_NO_THROW (
			e = rlms::EntityManager::Get (e_id1)
		);
		ASSERT_EQ (e, nullptr);
		ASSERT_EQ (rlms::EntityManager::n_errors, 1);
	}

	e_id1 = rlms::EntityManager::Create (e_id1);

	{
		SCOPED_TRACE ("Nominal");
		ASSERT_NO_THROW (
			e = rlms::EntityManager::Get (e_id1)
		);

		ASSERT_NE (e, nullptr);
		ASSERT_NO_THROW (
			ASSERT_EQ (e->id (), e_id1);
		);
		ASSERT_EQ (rlms::EntityManager::n_errors, 1);
	}

	rlms::EntityManager::Destroy (e_id1);

	{
		SCOPED_TRACE ("PostDestruction");
		ASSERT_NO_THROW (
			e = rlms::EntityManager::Get (e_id1);
		);
		ASSERT_EQ (e, nullptr);
		ASSERT_EQ (rlms::EntityManager::n_errors, 2);
	}

	{
		SCOPED_TRACE ("Invalid ID");
		ASSERT_NO_THROW (
			e = rlms::EntityManager::Get (rlms::Entity::NULL_ID);
		);
		ASSERT_EQ (rlms::EntityManager::n_errors, 3);
	}
}

//static void Destroy (ENTITY_ID id);
TEST_F (TestEntityManager, DestroyEntity) {
	rlms::ENTITY_ID e_id1 = 40;
	rlms::Entity* e = nullptr;

	{
		SCOPED_TRACE ("PreConstruction");
		ASSERT_EQ (rlms::EntityManager::n_errors, 0);
		ASSERT_NO_THROW (
			rlms::EntityManager::Destroy (e_id1);
		);
		ASSERT_EQ (rlms::EntityManager::n_errors, 1);
	}

	e_id1 = rlms::EntityManager::Create (e_id1);
	e = rlms::EntityManager::Get (e_id1);

	{
		SCOPED_TRACE ("Nominal");
		ASSERT_NE (e, nullptr);
		ASSERT_NO_THROW (
			ASSERT_EQ (e->id (), e_id1);
		);

		ASSERT_NO_THROW (
			rlms::EntityManager::Destroy (e_id1);
		);
	}

	{
		SCOPED_TRACE ("PostDestruction");
		ASSERT_NO_THROW (
			rlms::EntityManager::Destroy (e_id1);
		);
		ASSERT_EQ (rlms::EntityManager::n_errors, 2);
	}

	{
		SCOPED_TRACE ("Invalid ID");
		ASSERT_NO_THROW (
			rlms::EntityManager::Destroy (rlms::Entity::NULL_ID);
		);
		ASSERT_EQ (rlms::EntityManager::n_errors, 3);
	}
}

