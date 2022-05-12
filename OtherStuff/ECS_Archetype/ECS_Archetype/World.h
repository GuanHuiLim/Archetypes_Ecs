#pragma once
#include "ECS_Utils.h"
#include "Component_Manager.h"
#include "Archetype_Manager.h"

namespace ecs
{
	class World
	{
	public:
		World();
		~World();

		Entity CreateEntity();

		template<typename T>
		T& AddComponent(T const& component);


	private:
		std::unique_ptr<ComponentManager> m_componentMgr;
		std::unique_ptr<Archetype_Manager> m_archetypeMgr;

	};

	World::World()
	{

	}

	World::~World()
	{
	}
}