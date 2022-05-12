#pragma once
#include <cstdint>
#include <cstddef>
#include <limits>


namespace ecs
{

constexpr static std::uint64_t INVALID_ENTITY = std::numeric_limits<std::uint64_t>::max();
static constexpr std::size_t CHUNK_SIZE = 100;

union Entity final 
{
	std::uint64_t       value{ INVALID_ENTITY };

	union State final
	{
		std::uint32_t value;
		struct
		{
			std::uint32_t generation : 31;
			bool active : 1;
		};

		constexpr bool operator == (const State& rhs) const noexcept { return value == rhs.value; }
	};

	struct
	{
		std::uint32_t   index;		
		State			state;
	};


	template<typename T>

};

union ComponentId final
{
	std::uint64_t value{ INVALID_ENTITY };

	struct
	{
		std::uint32_t   id;
		std::uint32_t   unused : 31;
		bool			active : 1;
	};
};

std::uint32_t GetNextComponentID();

}// namespace ecs
