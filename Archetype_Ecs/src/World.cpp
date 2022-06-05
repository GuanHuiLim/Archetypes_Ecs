#pragma once
#include "World.h"
#include "EcsCommon.h"
namespace Ecs 
{
	inline ECSWorld::ECSWorld()
	{
		Archetype* nullArch = new Archetype();

		nullArch->full_chunks = 0;
		nullArch->componentList = Build_component_list(nullptr, 0);
		nullArch->componentHash = 0;
		nullArch->ownerWorld = this;

		archetypes.push_back(nullArch);

		archetypeSignatures.push_back(0);

		archetype_signature_map[0].push_back(nullArch);

		//we want archs to allways have 1 chunk at least, create initial
		Create_chunk_for_archetype(nullArch);
	}
}
