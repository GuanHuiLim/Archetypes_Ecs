﻿#pragma once
#include "EcsUtils.h"

#include <typeinfo>
namespace Ecs
{
	extern std::unordered_map<uint64_t, ComponentInfo> componentInfo_map; //metatype_cache

	struct TypeHash
	{
		std::size_t name_hash{ 0 };
		std::size_t matcher_hash{ 0 };

		bool operator==(const TypeHash& other)const {
			return name_hash == other.name_hash;
		}
		template<typename T>
		static constexpr const char* name_detail() {
			return __FUNCSIG__;
		}

		template<typename T>
		static constexpr size_t hash() {

			static_assert(!std::is_reference_v<T>, "dont send references to hash");
			static_assert(!std::is_const_v<T>, "dont send const to hash");
			return hash_fnv1a(name_detail<T>());
		}
	};
	//struct containing information about a unique component type
	struct ComponentInfo
	{
		using ConstructorFn = void(void*);
		using DestructorFn = void(void*);

		TypeHash hash;

		const char* name{ "none" }; //name 
		ConstructorFn* constructor;
		DestructorFn* destructor;
		uint16_t size{ 0 };
		uint16_t align{ 0 };

		bool is_empty() const { return align == 0u; };

		template<typename T>
		static constexpr ComponentInfo build_hash() {

			using base_type = std::remove_const_t<std::remove_reference_t<T>>;

			TypeHash hash;
			hash.name_hash = TypeHash::hash<base_type>();
			hash.matcher_hash |= (uint64_t)0x1L << (uint64_t)((hash.name_hash) % 63L);
			return hash;
		};
		template<typename T>
		static constexpr ComponentInfo build() {

			ComponentInfo info{};
			info.hash = build_hash<T>();
			info.name = typeid(T).name();

			if constexpr (std::is_empty_v<T>)
			{
				info.align = 0;
				info.size = 0;
			}
			else {
				info.align = alignof(T);
				info.size = sizeof(T);
			}

			info.constructor = [](void* p)
			{
				new(p) T{};
			};
			info.destructor = [](void* p)
			{
				((T*)p)->~T();
			};

			return info;
		};
	};

	//linked list header
	struct DataChunkHeader {
		struct ChunkComponentList* componentList; //pointer to the signature for this block
		struct Archetype* archetype{ nullptr };
		struct DataChunk* prev{ nullptr };
		struct DataChunk* next{ nullptr };
		int16_t last{ 0 }; //max index that has an entity
	};
	struct alignas(32)DataChunk 
	{
		std::byte storage[BLOCK_MEMORY_16K - sizeof(DataChunkHeader)];
		DataChunkHeader header;
	};
	static_assert(sizeof(DataChunk) == BLOCK_MEMORY_16K, "chunk size isnt 16kb");

	//set of unique combination of components
	struct ComponentCombination {
		struct ComponentIdentifier {
			const ComponentInfo* type;
			TypeHash hash;
			uint32_t chunkOffset;
		};
		int16_t chunkCapacity;
		std::vector<ComponentIdentifier> components;
	};

	//an array for storing components
	template<typename T>
	struct ComponentArray 
	{
		T* data{ nullptr };
		DataChunk* chunkOwner{ nullptr };

		ComponentArray() = default;
		ComponentArray(void* pointer, DataChunk* owner) {
			data = (T*)pointer;
			chunkOwner = owner;
		}

		const T& operator[](size_t index) const {
			return data[index];
		}
		T& operator[](size_t index) {
			return data[index];
		}
		bool valid()const {
			return data != nullptr;
		}
		T* begin() {
			return data;
		}
		T* end() {
			return data + chunkOwner->header.last;
		}
		int16_t size() {
			return chunkOwner->header.last;
		}
	};
}
