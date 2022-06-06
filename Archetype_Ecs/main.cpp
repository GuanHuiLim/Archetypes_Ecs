// Archetype_Ecs.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "src/A_Ecs.h"

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#include <iostream>
int main()
{
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    //std::cout << "Hello World!\n";
    {
        Ecs::ECSWorld world;
        auto a = world.new_entity();
    }
    //_CrtDumpMemoryLeaks();
}