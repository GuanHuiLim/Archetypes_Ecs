#include "pch.h"
#include "CppUnitTest.h"
#include "ECS_Archetype.hpp"

#include <iostream>
#include <random>
#include <time.h>       /* time */
using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace ecs;
std::mt19937 rand_generator;
namespace ArchetypeTest
{
	TEST_CLASS(ArchetypeTest)
	{
		struct Position {
			float x, y, z;
		};
		struct GameobjectID {
			uint32_t id;
		};
		struct TestComponent {
			float data_float;
			int data_int;
		};



	public:

		TEST_METHOD(AddComponents)
		{
			rand_generator.seed(time(NULL));
			std::uniform_int_distribution<> randomizer(1, 1000);
			SparseSet<int, uint32_t, 100> sparseVector;
			bool result = false;

			std::vector<int> inputs;
			std::size_t count = 100;
			//insert
			for (size_t i = 0; i < count; i++)
			{
				inputs.emplace_back(randomizer(rand_generator));
				sparseVector.Insert(i, count);
			}
			//erase
			for (size_t i = 0; i < count; i++)
			{
				sparseVector.Erase(i);
			}
			if (sparseVector.Size() == 0)
				result = true;

			Assert::IsTrue(result);
		}
	};

	TEST_CLASS(SparseSetTest)
	{
	public:
		
		TEST_METHOD(Insertion)
		{
			SparseSet<int, uint32_t, 100> sparseVector;
			bool result = false;

			sparseVector.Insert(0, 10);
			sparseVector.Insert(1, 20);
			sparseVector.Insert(2, 30);

			if (sparseVector.At(0) == 10 &&
				sparseVector.At(1) == 20 &&
				sparseVector.At(2) == 30)
				result = true;
			auto temp0 = sparseVector.At(0);
			auto temp1 = sparseVector.At(1);
			auto temp2 = sparseVector.At(2);

			std::cout << temp0 << "  " << temp1 << "  " << temp2 << std::endl;
			Assert::IsTrue(result); 
		}

		TEST_METHOD(RepeatedInsertion)
		{
			SparseSet<int, uint32_t, 100> sparseVector;
			bool result = false;

			sparseVector.Insert(0, 10);
			sparseVector.Insert(1, 20);
			sparseVector.Insert(2, 30);

			auto temp0 = sparseVector.Insert(0, 10);
			auto temp1 = sparseVector.Insert(1, 20);
			auto temp2 = sparseVector.Insert(2, 30);

			if (temp0 == false &&
				temp1 == false &&
				temp2 == false)
				result = true;

			std::cout << temp0 << "  " << temp1 << "  " << temp2 << std::endl;
			Assert::IsTrue(result);
		}

		TEST_METHOD(Erase)
		{
			rand_generator.seed(time(NULL));
			std::uniform_int_distribution<> randomizer(1, 1000);
			SparseSet<int, uint32_t, 100> sparseVector;
			bool result = false;

			std::vector<int> inputs;
			std::size_t count = 100;
			//insert
			for (size_t i = 0; i < count; i++)
			{
				inputs.emplace_back(randomizer(rand_generator));
				sparseVector.Insert(i, count);
			}
			//erase
			for (size_t i = 0; i < count; i++)
			{
				sparseVector.Erase(i);
			}
			if (sparseVector.Size() == 0)
				result = true;

			Assert::IsTrue(result);
		}


	};
}
