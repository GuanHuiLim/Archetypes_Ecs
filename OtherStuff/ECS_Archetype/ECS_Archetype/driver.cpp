// ECS_Archetype.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "ECS_Archetype.hpp"
#include <iostream>

int main()
{
    std::cout << "Hello World!\n";

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
}
