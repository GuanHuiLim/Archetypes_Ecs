#pragma once
#include "EcsUtils.h"
#include "Component.h"
#include "Archetype.h"
#include <algorithm>
namespace Ecs 
{
	//forward declarations
	DataChunk* Build_chunk(ComponentCombination* cmpList);
	inline int Insert_entity_in_chunk(DataChunk* chunk, EntityID EID, bool bInitializeConstructors = true);
	inline EntityID Erase_entity_in_chunk(DataChunk* chunk, uint16_t index);

	
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
	//ecs world
	***********************************/
	inline EntityID Allocate_entity(ECSWorld* world) {
		EntityID newID;
		//no dead entity IDs
		if (world->dead_entities == 0) {
			int index = world->entities.size();

			EnityToChunk entity_chunk_mapping;
			entity_chunk_mapping.chunk = nullptr;
			entity_chunk_mapping.chunkIndex = 0;
			entity_chunk_mapping.generation = 1;

			world->entities.push_back(entity_chunk_mapping);

			newID.generation = 1;
			newID.index = index;
		}
		else { //dead entity IDs available for reuse
			int index = world->deletedEntities.back();
			world->deletedEntities.pop_back();

			//increment generation count as we are recycling it
			world->entities[index].generation++; 
			//update our return value
			newID.generation = world->entities[index].generation;
			newID.index = index;
			//decrease number of dead entity IDs available
			world->dead_entities--;
		}

		world->live_entities++;
		return newID;
	}
	/***********************************
	//chunk
	***********************************/
	inline DataChunk* Find_free_chunk(Archetype* arch) {
		DataChunk* targetChunk = nullptr;
		if (arch->chunks.size() == 0) {
			targetChunk = Create_chunk_for_archetype(arch);
		}
		else {
			targetChunk = arch->chunks[arch->chunks.size() - 1];
			//chunk is full, create a new one
			if (targetChunk->header.last == arch->componentList->chunkCapacity) {
				targetChunk = Create_chunk_for_archetype(arch);
			}
		}
		return targetChunk;
	}

	/***********************************
	//entity
	***********************************/
	inline void Move_entity_to_archetype(Archetype* newarch, EntityID id, bool bInitializeConstructors = true) {

		//insert into new chunk
		DataChunk* oldChunk = newarch->ownerWorld->entities[id.index].chunk;
		DataChunk* newChunk = Find_free_chunk(newarch);

		int newindex = Insert_entity_in_chunk(newChunk, id, bInitializeConstructors);
		int oldindex = newarch->ownerWorld->entities[id.index].chunkIndex;

		int oldNcomps = oldChunk->header.componentList->components.size();
		int newNcomps = newChunk->header.componentList->components.size();

		auto& oldClist = oldChunk->header.componentList;
		auto& newClist = newChunk->header.componentList;

		//copy all data from old chunk into new chunk
		//bad iteration, fix later

		struct Merge {
			int msize;
			int idxOld;
			int idxNew;
		};
		int mergcount = 0;
		Merge mergarray[32];

		for (int i = 0; i < oldNcomps; i++) {
			const ComponentInfo* mtCp1 = oldClist->components[i].type;
			if (!mtCp1->is_empty()) {
				for (int j = 0; j < newNcomps; j++) {
					const ComponentInfo* mtCp2 = newClist->components[j].type;

					//pointers are stable
					if (mtCp2 == mtCp1) {
						mergarray[mergcount].idxNew = j;
						mergarray[mergcount].idxOld = i;
						mergarray[mergcount].msize = mtCp1->size;
						mergcount++;
					}
				}
			}
		}

		for (int i = 0; i < mergcount; i++) {
			//const Metatype* mtCp1 = mergarray[i].mtype;

			//pointer for old location in old chunk
			void* ptrOld = (void*)((byte*)oldChunk + oldClist->components[mergarray[i].idxOld].chunkOffset + (mergarray[i].msize * oldindex));

			//pointer for new location in new chunk
			void* ptrNew = (void*)((byte*)newChunk + newClist->components[mergarray[i].idxNew].chunkOffset + (mergarray[i].msize * newindex));

			//memcopy component data from old to new
			memcpy(ptrNew, ptrOld, mergarray[i].msize);
		}

		//delete entity from old chunk
		Erase_entity_in_chunk(oldChunk, oldindex);

		//assign entity chunk data
		newarch->ownerWorld->entities[id.index].chunk = newChunk;
		newarch->ownerWorld->entities[id.index].chunkIndex = newindex;
	}
	inline void Set_entity_archetype(Archetype* arch, EntityID id) {

		//if chunk is null, we are a empty entity
		if (arch->ownerWorld->entities[id.index].chunk == nullptr) {

			DataChunk* targetChunk = Find_free_chunk(arch);

			int index = Insert_entity_in_chunk(targetChunk, id);
			arch->ownerWorld->entities[id.index].chunk = targetChunk;
			arch->ownerWorld->entities[id.index].chunkIndex = index;
		}
		else {
			Move_entity_to_archetype(arch, id, false);
		}
	}
	inline EntityID Create_entity_with_archetype(Archetype* arch) {
		ECSWorld* world = arch->ownerWorld;

		EntityID newID = Allocate_entity(world);

		Set_entity_archetype(arch, newID);

		return newID;
	}
}
