#include "ECS_Archetype.hpp"

namespace ecs
{
	static std::uint32_t componentID;

	std::uint32_t ecs::GetNextComponentID()
	{
		return componentID++;
	}
}