#pragma once

#include <cstdint>
#include <cstddef>
#include <typeinfo>
#include <unordered_map>
namespace Ecs
{
	constexpr size_t BLOCK_MEMORY_16K = 16384;
	constexpr size_t BLOCK_MEMORY_8K = 8192;

	struct TypeHash;
	struct ComponentInfo;
	union  EntityID;
	struct DataChunkHeader;
	struct DataChunk;
	struct ChunkComponentList;
	struct Archetype;
	struct EnityToChunk;
	struct Query;
	struct ECSWorld;

	inline constexpr uint64_t hash_64_fnv1a(const char* key, const uint64_t len) {

		uint64_t hash = 14695981039346656037ull;
		uint64_t prime = 1099511628211ull;
		for (int i = 0; i < len; ++i) 
		{
			uint8_t value = key[i];
			hash = hash ^ value;
			hash *= prime;
		}
		return hash;
	}

	inline constexpr uint64_t hash_fnv1a(const char* key) {
		uint64_t hash = 14695981039346656037ull;
		uint64_t prime = 1099511628211ull;
		int i = 0;
		while (key[i]) 
		{
			uint8_t value = key[i++];
			hash = hash ^ value;
			hash *= prime;
		}
		return hash;
	}

	union EntityID 
	{
		uint64_t value;
		struct
		{
			uint32_t index;
			uint32_t generation;
		};
	};

}
