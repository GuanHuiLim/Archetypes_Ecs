#pragma once
#include "EcsUtils.h"
#include "Component.h"
#include "Archetype.h"
#include <algorithm>
namespace Ecs 
{
	//forward declarations
	DataChunk* build_chunk(ComponentCombination* cmpList);


	
	//retrieves a component's ComponentInfo
	template<typename T>
	static const ComponentInfo* Get_ComponentInfo() {
		static const ComponentInfo* mt = []() {
			constexpr size_t name_hash = ComponentInfo::build_hash<T>().name_hash;

			auto type = componentInfo_map.find(name_hash);
			if (type == componentInfo_map.end()) {
				constexpr ComponentInfo newtype = ComponentInfo::build<T>();
				componentInfo_map[name_hash] = newtype;
			}
			return &componentInfo_map[name_hash];
		}();
		return mt;
	}

	inline ComponentCombination* Build_component_list(const ComponentInfo** types, size_t count) {
		ComponentCombination* list = new ComponentCombination();

		//calculate total size of all components
		int component_size = sizeof(EntityID);
		for (size_t i = 0; i < count; i++) {
			component_size += types[i]->size;
		}
		//total size of allocated memory
		size_t availableStorage = sizeof(DataChunk::storage);
		//2 less than the real count to account for sizes and give some slack
		size_t itemCount = (availableStorage / component_size) - 2;

		uint32_t offsets = sizeof(DataChunkHeader);
		offsets += sizeof(EntityID) * itemCount;

		for (size_t i = 0; i < count; i++) {
			const ComponentInfo* type = types[i];

			if (type->align != 0) {
				//align properly
				size_t remainder = offsets % type->align;
				size_t oset = type->align - remainder;
				offsets += oset;
			}

			list->components.push_back({ type,type->hash,offsets });

			if (type->align != 0) {
				offsets += type->size * (itemCount);
			}

		}

		//implement proper size handling later
		assert(offsets <= BLOCK_MEMORY_16K);

		list->chunkCapacity = itemCount;

		return list;
	}
	//given array of component types, generates unique signature 
	//from combination of component types
	inline size_t Build_signature(const ComponentInfo** types, size_t count) {
		size_t and_hash = 0;
		//for (auto m : types)
		for (int i = 0; i < count; i++)
		{
			//consider if the fancy hash is needed, there is a big slowdown
			//size_t keyhash = ash_64_fnv1a(&types[i]->name_hash, sizeof(size_t));
			//size_t keyhash = types[i]->name_hash;

			and_hash |= types[i]->hash.matcher_hash;
			//and_hash |=(uint64_t)0x1L << (uint64_t)((types[i]->name_hash) % 63L);
		}
		return and_hash;
	}
	//add a new chunk to an archtype
	inline DataChunk* Create_chunk_for_archetype(Archetype* arch) {
		DataChunk* chunk = Build_chunk(arch->componentList);

		chunk->header.archetype = arch;
		arch->chunks.emplace_back(chunk);
		return chunk;
	}

	//comparision function for sorting components in the correct order
	inline bool Compare_ComponentInfo(const ComponentInfo* A, const ComponentInfo* B) {
		//return A->name_hash < B->name_hash;
		return A < B;
	}
	//given an array of components, sorts components in order
	inline void Sort_ComponentInfo(const ComponentInfo** types, size_t count) {
		std::sort(types, types + count, [](const ComponentInfo* A, const ComponentInfo* B) {
			return Compare_ComponentInfo(A, B);
			});
	}
	//retrieves or creates an archetype
	inline Archetype* Find_or_create_archetype(ECSWorld* world, const ComponentInfo** types, size_t count) {
		const ComponentInfo* temporalComponentInfoArray[MAX_COMPONENTS];
		assert(count < MAX_COMPONENTS);

		const ComponentInfo** typelist;

		if (false) {//!is_sorted(types, count)) {
			for (int i = 0; i < count; i++) {
				temporalComponentInfoArray[i] = types[i];

			}
			Sort_ComponentInfo(temporalComponentInfoArray, count);
			typelist = temporalComponentInfoArray;
		}
		else {
			typelist = types;
		}

		const uint64_t matcher = Build_signature(typelist, count);

		//try to find the archetype in the hashmap
		auto iter = world->archetype_signature_map.find(matcher);
		if (iter != world->archetype_signature_map.end()) {

			//iterate the vector of archetypes
			auto& archvec = iter->second;//world->archetype_signature_map[matcher];
			for (int i = 0; i < archvec.size(); i++) {

				auto componentList = archvec[i]->componentList;
				int ccount = componentList->components.size();
				// if requested number of components matches archetype's
				// number of components
				if (ccount == count) { 
					for (int j = 0; j < ccount; j++) {
						//check if components matches the typelist
						if (componentList->components[j].type != typelist[j])
						{
							//mismatch, inmediately continue
							goto contA;
						}
					}

					//everything matched. Found. Return inmediately
					return archvec[i];
				}

			contA:;
			}
		}

		//not found, create a new one
		Archetype* newArch = new Archetype();

		newArch->full_chunks = 0;
		newArch->componentList = Build_component_list(typelist, count);
		newArch->componentHash = matcher;
		newArch->ownerWorld = world;
		world->archetypes.push_back(newArch);
		world->archetypeSignatures.push_back(matcher);
		world->archetype_signature_map[matcher].push_back(newArch);

		//we want archs to allways have 1 chunk at least, create initial
		Create_chunk_for_archetype(newArch);
		return newArch;
	}
	//allocates new memory for a chunk
	inline DataChunk* Build_chunk(ComponentCombination* cmpList) {

		DataChunk* chunk = new DataChunk();
		chunk->header.last = 0;
		chunk->header.componentList = cmpList;

		return chunk;
	}


	/***********************************
	//entity
	***********************************/
	inline void Set_entity_archetype(Archetype* arch, EntityID id) {

		//if chunk is null, we are a empty entity
		if (arch->ownerWorld->entities[id.index].chunk == nullptr) {

			DataChunk* targetChunk = find_free_chunk(arch);

			int index = insert_entity_in_chunk(targetChunk, id);
			arch->ownerWorld->entities[id.index].chunk = targetChunk;
			arch->ownerWorld->entities[id.index].chunkIndex = index;
		}
		else {
			move_entity_to_archetype(arch, id, false);
		}
	}
	inline EntityID Create_entity_with_archetype(Archetype* arch) {
		ECSWorld* world = arch->ownerWorld;

		EntityID newID = allocate_entity(world);

		set_entity_archetype(arch, newID);

		return newID;
	}
}
