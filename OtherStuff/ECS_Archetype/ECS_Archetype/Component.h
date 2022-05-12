#pragma once

#include <vector>
#include <cstddef>
namespace ecs::component
{
	namespace type
	{
		using Uid = uint32_t;
		using BitID = uint32_t;
		using Size = uint32_t;

		enum class Category
		{
			PLAIN_DATA,
			SHARED_DATA,
		};

		using ConstructFunctionPointer	= void(*)(std::byte*) noexcept;
		using DestructFunctionPointer	= void(*)(std::byte*) noexcept;
		using CopyFunctionPointer		= void(*)(std::byte* dst, const std::byte* src) noexcept;
		using MoveFunctionPointer		= void(*)(std::byte* dst, const std::byte* src) noexcept;
	}

	struct info
	{
		type::Uid unique_id;
		type::BitID bit_id;
		type::Size size;

		type::Category category;

		type::ConstructFunctionPointer constructFP;
		type::DestructFunctionPointer destructFP;
		type::CopyFunctionPointer copyFP;
		type::MoveFunctionPointer moveFP;

		const char* const name;
	};


}
