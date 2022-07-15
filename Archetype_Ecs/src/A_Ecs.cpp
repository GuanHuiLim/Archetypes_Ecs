#include "A_Ecs.h"

namespace Ecs
{
	std::unordered_map<uint64_t, ComponentInfo> componentInfo_map{};

	EntityID ECSWorld::new_entity(std::vector<uint64_t> const& component_hashes)
	{
		Archetype* arch = nullptr;
		//empty component list will use the hardcoded null archetype
		if (component_hashes.empty() == false) {
			std::vector<const ComponentInfo*> componentInfos;
			componentInfos.reserve(component_hashes.size());
			for (auto c : component_hashes)
				componentInfos.emplace_back(&componentInfo_map[c]);

			const ComponentInfo** types = &componentInfos[0];
			size_t num = component_hashes.size();

			internal::sort_ComponentInfos(types, num);
			arch = internal::find_or_create_archetype(this, types, num);
		}
		else {
			arch = get_empty_archetype();
		}

		return internal::create_entity_with_archetype(arch);
	}

	EntityID ECSWorld::duplicate_entity(EntityID id)
	{
		assert(internal::is_entity_valid(this,id));
		Archetype* arch = internal::get_entity_archetype(this, id);



		return internal::duplicate_entity_with_archetype(arch, id);
	}


	std::vector<uint64_t> const ECSWorld::componentHashes(EntityID id)
	{
		//if invalid id return nothing
		if (internal::is_entity_valid(this, id) == false) return {};

		std::vector<uint64_t> hashes;
		Archetype* arch = entities[id.index].chunk->header.archetype;
		for (auto& c : arch->componentList->components)
			hashes.emplace_back(c.type->hash.name_hash);

		return hashes;
	}
}

//#include "World.cpp"
//#include "EcsCommon.cpp"
