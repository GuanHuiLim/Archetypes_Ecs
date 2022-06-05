#include "pch.h"
#include "CppUnitTest.h"
#include "../Archetype_Ecs/src/A_Ecs.h"
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace EcsTesting
{
	struct A { int x, y; };
	struct B { float x, y; };
	struct C { float x, y, z; };
	TEST_CLASS(ECS_Test)
	{
	public:
		/* Template test case
		
		TEST_METHOD(NewEntity)
		{
			Ecs::ECSWorld world;
			auto id = world.new_entity();
		}

		*/

		TEST_METHOD(NewEntity)
		{
			Ecs::ECSWorld world;
			auto id = world.new_entity();
		}

		TEST_METHOD(AddComponent)
		{
			Ecs::ECSWorld world;
			auto id = world.new_entity();
			world.add_component<A>(id);
			B b{ 1.f,5.f };
			world.add_component<B>(id,b);
			auto& test_B = world.get_component<B>(id);

			Assert::IsTrue(test_B.x == b.x && test_B.y == b.y);
		}
	};
}
