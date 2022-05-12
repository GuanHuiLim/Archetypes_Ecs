#pragma once
#include "EcsUtils.h"

namespace Ecs
{
	struct Archetype {
		ChunkComponentList* componentList;
		struct ECSWorld* ownerWorld;
		size_t componentHash;
		int full_chunks;
		//full chunks allways on the start of the array
		std::vector<DataChunk*> chunks;
	};

	struct EnityToChunk
	{
		DataChunk* chunk;
		uint32_t generation;
		uint16_t chunkIndex;

		bool operator==(const EnityToChunk& other) const {
			return chunk == other.chunk && generation == other.generation && chunkIndex == other.chunkIndex;
		}

		bool operator!=(const EnityToChunk& other) const {
			return !(other == *this);
		}
	};
}