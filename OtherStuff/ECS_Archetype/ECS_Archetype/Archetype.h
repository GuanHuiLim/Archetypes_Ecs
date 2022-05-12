#pragma once
#include "SparseSet.h"

#include <vector>
#include <stdexcept>

namespace ecs
{
template <typename T>
class Archetype
{
	using Chunk = SparseSet<T, Entity, CHUNK_SIZE>;

	std::vector<Chunk> m_chunks;
public:

	//number of chunks this archetype is using
	std::size_t NumChunks() const { return m_chunks.size(); }
	std::size_t Size() const
	{
		std::size_t totalSize{ 0u };
		for (auto& i : m_chunks)
			totalSize += i.Size();
		return totalSize;
	}

	template <typename... args>
	bool Emplace(Entity const entity, args&&... arguementList) const
	{
		for (auto& i : m_chunks)
		{
			if (i.Full())
				continue;

			return i.Emplace(entity, std::forward<args>(arguementList)...);
		}
	}

	bool Erase(Entity const entity)
	{
		for (auto& i : m_chunks)
		{
			if (i.Contains(entity) == false)
				continue;
			return i.Erase(entity);
		}
		//could not find or does not contain element
		return false;
	}

	T& At(Entity const entity)
	{
		for (auto& i : m_chunks)
		{
			if (i.Contains(entity) == false)
				continue;
			return i.At(entity);
		}
	}

	T const& At(Entity const entity) const
	{
		for (auto& i : m_chunks)
		{
			if (i.Contains(entity) == false)
				continue;
			return i.At(entity);
		}
	}

	T& operator[](Entity const entity)
	{
		if (Contains(entity) == false)
			throw std::runtime_error("no corresponding element for this entity!!");
		return At(key);
	}

	bool Contains(Entity const entity) const noexcept
	{
		for (auto& i : m_chunks)
		{
			if (i.Contains(entity))
				return true;
		}
		return false;
	}
};

} //namespace ecs