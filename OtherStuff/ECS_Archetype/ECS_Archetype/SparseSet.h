#pragma once
#include "ECS_Utils.h"

#include <memory>
#include <vector>
#include <bitset>
#include <array>
#include <cassert>
#include <stdexcept>

namespace ecs
{

template <typename Element, typename KeyType, std::size_t MAX_SIZE = 100u>
class SparseSet
{
	using value_type = Element;
	using key_type = KeyType;
	constexpr static std::size_t SIZE = MAX_SIZE;

	template <typename T>
	using allocator = std::allocator<T>;
	using indexVector = std::vector<key_type, allocator<key_type>>;
	using elementVector = std::vector<value_type, allocator<value_type>>;
	using sparseVector = std::array<value_type, MAX_SIZE>;
	static constexpr key_type INVALID = std::numeric_limits<key_type>::max();

	indexVector m_indexes{};
	elementVector m_elements{};
	sparseVector m_sparseKeys{};

	std::size_t m_size{ 0 };
public:

	SparseSet()
	{
		m_indexes.reserve(SIZE);
		m_elements.reserve(SIZE);
		std::fill(m_sparseKeys.begin(), m_sparseKeys.end(), INVALID);		
	}

	~SparseSet() = default;

	///------------------------------------
	/// Modifiers
	///------------------------------------
	//void resize(size_type size); // reserve the space for elements in the interval [0,size-1]

	//void swap(generic_sparse_set& s); // swap the contents with another sparse set

	// insert an element; returns true if a new element is inserted;
	// returns false if the same element is already in the set.
	bool Insert(key_type const key, value_type const& value)
	{
		if (ContainsKey(key) == true)
			return false;//throw std::runtime_error("element already exists");
		if (m_elements.size() < m_size)
			throw std::runtime_error("element container invalid size");
		if (Full())
			throw std::runtime_error("cannot insert more than this!!");


		if (m_elements.size() > m_size)
		{
			m_elements[m_size] = value;
			m_indexes[m_size]  = key;
			m_sparseKeys[key] = static_cast<key_type>(m_size - 1u);
		}
		else
		{
			m_elements.emplace_back(value);
			m_indexes.emplace_back(key);
			m_sparseKeys[key] = static_cast<key_type>(m_indexes.size() - 1u);
		}

		++m_size;

		return true;
	}

	// insert an element by constructing it in place with arguements given; 
	// returns true if a new element is inserted;
	// returns false if the same element is already in the set.
	template <typename... args>
	bool Emplace(key_type const key, args&&... arguementList)
	{
		if (ContainsKey(key) == true)
			return false;//throw std::runtime_error("element already exists");
		if (m_elements.size() < m_size)
			throw std::runtime_error("element container invalid size");
		if (Full())
			throw std::runtime_error("cannot insert more than this!!");

		if (m_elements.size() > m_size)
		{
			m_elements[m_size] = value_type{ arguementList };
			m_indexes[m_size] = key;
			m_sparseKeys[key] = static_cast<key_type>(m_size - 1u);
		}
		else
		{
			m_elements.emplace_back(arguementList);
			m_indexes.emplace_back(key);
			m_sparseKeys[key] = static_cast<key_type>(m_indexes.size() - 1u);
		}

		++m_size;

		return true;
	}

	value_type& At(key_type const key)
	{
		if (ContainsKey(key) == false)
			throw std::runtime_error("element does not exist");
		return m_elements[m_sparseKeys[key]];
	}

	value_type const& At(key_type const key) const
	{
		if (ContainsKey(key) == false)
			throw std::runtime_error("element does not exist");
		return m_elements[m_sparseKeys[key]];
	}

	value_type& operator[](key_type const key)
	{
		if (ContainsKey(key) == false)
			Emplace(key);
		return At(key);
	}
							   
	// delete an element by key
	bool Erase(key_type const key)
	{
		if (ContainsKey(key) == false)
			return false;
		if (Empty())
			throw std::runtime_error("container empty!!");

		Swap_And_Dont_Pop(key);
		--m_size;

		return true;
	}

	//void erase(const iterator& it); // optional method; deletes an element by iterator   

	// deletes all the elemnts
	void Clear()
	{
		m_indexes.clear();
		m_elements.clear();
		std::fill(m_sparseKeys.begin(), m_sparseKeys.end(), INVALID);
		m_size = 0u;
	}

	//number of items in packed array
	std::size_t Size() const noexcept { return m_size; }
	//capacity of packed array
	std::size_t Capacity() const noexcept { return m_elements.capacity(); }
	//true if no items in packed array
	bool Empty() const noexcept { return m_size == 0u; }
	//true if contains max number of possible items
	bool Full() const noexcept
	{
		return m_size == (SIZE - 1u);
	}
	//true if the key has a corresponding element
	bool Contains(key_type const key) const noexcept
	{
		return ContainsKey(key);
	}


private:
	bool ContainsKey(key_type const key) const
	{
		if (m_sparseKeys[key] < m_size)
			return m_indexes[m_sparseKeys[key]] == key;
		else
			return false;
	}

	void Swap_And_Dont_Pop(key_type const key)
	{
		auto index_to_remove = m_sparseKeys[key];
		auto last_valid_index = m_indexes[m_size-1u];

		std::swap(m_elements[index_to_remove], m_elements[m_size - 1u]);
		std::swap(m_indexes[index_to_remove], m_indexes[m_size - 1u]);
		//m_elements.pop_back();
		//m_indexes.pop_back();


		m_sparseKeys[last_valid_index] = index_to_remove;

	}

	void Swap_And_Pop(key_type const key)
	{
		auto index_to_remove = m_sparseKeys[key];
		auto last_valid_index = m_indexes[m_size - 1u];

		std::swap(m_elements[index_to_remove], m_elements.back());
		std::swap(m_indexes[index_to_remove], m_indexes.back());
		m_elements.pop_back();
		m_indexes.pop_back();


		m_sparseKeys[last_valid_index] = index_to_remove;

	}

};


}//namespace ecs