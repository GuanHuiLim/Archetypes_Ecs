#pragma once
#include "EcsUtils.h"
#include "EcsCommon.h"
#include "Archetype.h"

#include <vector>
#include <unordered_map>
namespace Ecs
{

	struct ECSWorld {
		std::vector<EnityToChunk> entities;
		std::vector<EntityID::index_type> deletedEntities;

		std::unordered_map<uint64_t, std::vector<Archetype*>> archetype_signature_map{};
		std::unordered_map<uint64_t, Archetype*> archetype_map{};
		std::vector<Archetype*> archetypes;
		//unique archetype hashes
		std::vector<size_t> archetypeHashes;
		//bytemask hash for checking
		std::vector<size_t> archetypeSignatures;

		std::unordered_map<uint64_t, void*> singleton_map{};

		int live_entities{ 0 };
		int dead_entities{ 0 };
		inline ECSWorld();

		~ECSWorld()
		{
			for (Archetype* arch : archetypes)
			{
				for (DataChunk* chunk : arch->chunks) {
					delete chunk;
				}
				delete arch;
			}
		};
		//needs push_back(DataChunk*) to work, returns number
		template<typename Container>
		int gather_chunks(Query& query, Container& container);

		template<typename Func>
		void for_each(Query& query, Func&& function);

		template<typename Func>
		void for_each(Func&& function);

		template<typename C>
		void add_component(EntityID id, C& comp);
		template<typename C>
		void add_component(EntityID id);

		template<typename C>
		void remove_component(EntityID id);

		template<typename C>
		bool has_component(EntityID id);

		template<typename C>
		C& get_component(EntityID id);

		template<typename C>
		C* set_singleton();
		template<typename C>
		C* set_singleton(C&& singleton);

		template<typename C>
		C* get_singleton();

		template<typename ... Comps>
		inline EntityID new_entity()
		{
			Archetype* archetype = nullptr;
			//empty component list will use the hardcoded null archetype
			if constexpr (sizeof...(Comps) != 0) {
				static const ComponentInfo* types[] = { Get_ComponentInfo<Comps>()... };
				constexpr size_t num = (sizeof(types) / sizeof(*types));

				Sort_ComponentInfo(types, num);
				arch = Find_or_create_archetype(this, types, num);
			}
			else {
				arch = get_empty_archetype();
			}

			return adv::create_entity_with_archetype(arch);
		}

		inline void destroy(EntityID eid);

		Archetype* get_empty_archetype() { return archetypes[0]; };
	};
}