#include "pch.h"
#include "CppUnitTest.h"
#include "../Archetype_Ecs/src/A_Ecs.h"

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#include <array>
#include <cmath>
#include <limits>
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace EcsTesting
{
	constexpr auto EPSILON = std::numeric_limits<float>::epsilon();
	struct A { 
		int x{ -1 }, y{-2}; 
		friend  bool operator==(A const& lhs, A const& rhs)
		{
			return lhs.x == rhs.x && lhs.y == rhs.y;
		}
	
	};
	struct B {
		float x{ -1.f }, y{-2.f};
		friend  bool operator==(B const& lhs, B const& rhs)
		{
			return abs(lhs.x - rhs.x) < EPSILON 
				&& abs(lhs.y - rhs.y) < EPSILON;
		}
	};
	struct C { float x, y, z; };
	TEST_CLASS(ECS_Test)
	{
	public:
		ECS_Test() { _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF); }

		
		/* Template test case
		
		TEST_METHOD(NewEntity)
		{
			Ecs::ECSWorld world;
			auto id = world.new_entity();
		}

		*/

		TEST_METHOD(NewEntity)
		{
			constexpr int ARR_SIZE = 100000;
			bool result{ true };
			Ecs::ECSWorld world;
			auto id = world.new_entity();
			std::array<decltype(id), ARR_SIZE> ids_100;
			for (size_t i = 0; i < ARR_SIZE; i++)
			{
				ids_100[i] = world.new_entity<A,B>();
			}
			for (size_t i = 0; i < ARR_SIZE; i++)
			{
				A defaultA{};
				B defaultB{};
				auto& a = world.get_component<A>(ids_100[i]);
				auto& b = world.get_component<B>(ids_100[i]);

				if (a != defaultA 
				 || b != defaultB)
					result = false;
			}
			world.set_singleton<C>();
			Assert::IsTrue(result);
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

		TEST_METHOD(For_Each)
		{
			Ecs::ECSWorld world;
			constexpr int ARR_SIZE = 1000;
			bool result{ true };

			std::array<Ecs::EntityID, ARR_SIZE> ids;
			for (size_t i = 0; i < ARR_SIZE; i++)
			{
				ids[i] = world.new_entity<A, B>();
			}

			A defaultA{}; defaultA.x++; defaultA.y++;
			B defaultB{}; defaultB.x -= 10.f; defaultB.y -= 10.f;
			world.for_each([](A&a, B& b) {
				a.x++; a.y++;
				b.x -= 10.f; b.y -= 10.f;
				});

			for (size_t i = 0; i < ARR_SIZE; i++)
			{
				auto& a = world.get_component<A>(ids[i]);
				auto& b = world.get_component<B>(ids[i]);

				if (a != defaultA
				 || b != defaultB)
					result = false;
			}

			Assert::IsTrue(result);
		}

		TEST_METHOD(For_Each_Query)
		{
			Ecs::ECSWorld world;
			constexpr int ARR_SIZE = 1000;
			bool result{ true };

			Ecs::Query query;
			query.with<A, B>().exclude<C>().build();

			//same as for each
			std::array<Ecs::EntityID, ARR_SIZE> ids;
			for (size_t i = 0; i < ARR_SIZE; i++)
			{
				ids[i] = world.new_entity<A, B>();
			}

			A defaultA{}; defaultA.x++; defaultA.y++;
			B defaultB{}; defaultB.x -= 10.f; defaultB.y -= 10.f;
			world.for_each(query,
				[](A& a, B& b) {
				a.x++; a.y++;
				b.x -= 10.f; b.y -= 10.f;
				});

			for (size_t i = 0; i < ARR_SIZE; i++)
			{
				auto& a = world.get_component<A>(ids[i]);
				auto& b = world.get_component<B>(ids[i]);

				if (a != defaultA
					|| b != defaultB)
					result = false;
			}

			Assert::IsTrue(result);
		}
	};
}
