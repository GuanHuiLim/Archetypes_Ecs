/************************************************************************************//*!
\file           SparseArray.h
\project        Ouroboros
\author         Lim Guan Hui, l.guanhui, 2000552 | code contribution (100%)
\par            email: l.guanhui\@digipen.edu
\date           October 6, 2021
\brief          A container that is implemented as a "sparse set" data structure.
                It functions by using three containers, for its operations.
                - the container of densely packed elements
                - the container of index the densely packed elements are related to
                - the container of index to the elements

Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*//*************************************************************************************/
#pragma once
#include <memory>
#include <vector>
namespace oo
{
	struct SparseContainerBase
	{
		template <typename T>
		using allocator_type = std::allocator<T>;
		//using allocator_type = std::allocator<T>;

		using index_type = uint32_t;
		constexpr static std::size_t MAX_SIZE = 5000u;
		using sparse_container = std::vector<index_type, allocator_type<index_type>>;
		using sparse_container_iterator = typename sparse_container::iterator;
		using index_container = std::array<index_type, MAX_SIZE>;

		sparse_container& GetSparseContainer() { return m_sparse; }
		sparse_container_iterator SparseBegin() { return m_sparse.begin(); }
		sparse_container_iterator SparseEnd() { return m_sparse.end(); }
		sparse_container::size_type SparseContainerSize() const { return m_sparse.size(); }
		virtual bool Contains(index_type index) const = 0;
	protected:
		sparse_container m_sparse;
	};


	template <typename T>
	class SparseContainer : public SparseContainerBase
	{
	private:
		/*using alloc_type = typename std::allocator_traits<Allocator>::template rebind_alloc<T>;
		using alloc_traits = std::allocator_traits<alloc_type>;
		using alloc_pointer = typename alloc_traits::pointer;
		using alloc_const_pointer = typename alloc_traits::const_pointer;

		using bucket_alloc_type = typename std::allocator_traits<Allocator>::template rebind_alloc<alloc_pointer>;
		using bucket_alloc_traits = std::allocator_traits<bucket_alloc_type>;
		using bucket_alloc_pointer = typename bucket_alloc_traits::pointer;*/

	public:
		using value_type = T;
		using dense_container = std::vector<value_type, typename SparseContainerBase::allocator_type<value_type>>;
		using iterator = typename dense_container::iterator;
		using size_type = typename SparseContainerBase::index_type;
		using sparse_container = typename SparseContainerBase::sparse_container;
		using sparse_container_iterator = typename SparseContainerBase::sparse_container_iterator;
		using index_container = typename SparseContainerBase::index_container;

		static constexpr index_type MAX_ID = std::numeric_limits<index_type>::max();
	protected:
		// the container of densely packed elements
		dense_container m_dense;
		// the container of index the densely packed elements are related to
		//sparse_container m_sparse;
		// the container of index to the elements
		index_container m_index;

		/*alloc_type allocator;
		bucket_alloc_type bucket_allocator;
		bucket_alloc_pointer sparse;
		alloc_pointer packed;
		std::size_t bucket;
		std::size_t count;
		std::size_t reserved;*/


	public:
		///------------------------------------
		/// Constructors
		///------------------------------------
		/// // creating a sparse set; space is not reserved, requires resize.
		SparseContainer()
		{
			m_dense.reserve(MAX_SIZE);
			m_sparse.reserve(MAX_SIZE);
			std::fill(m_index.begin(), m_index.end(), MAX_ID);
		}

		bool IsValid(const size_type index) const
		{
			return index < MAX_SIZE;
		}
		/*********************************************************************************//*!
		\brief returns true if "index" has an element related to it being stored, false otherwise
		\param index index to check
		\return true if there is no element associated with this index, false otherwise
		*//**********************************************************************************/
		bool IsAvailable(const size_type index) const
		{
			//if unused index return true
			if (IsValid(index) && m_index[index] >= m_dense.size())
				return true;
			//if the dense element located by this index does not belong to
			//this index return true, else return false
			return m_sparse[m_index[index]] != index;
		}
		/*********************************************************************************//*!
		\brief Checks if there is an element stored at "index"
		\param index index to check is there is an element
		\return true if there is an element stored, false otherwise
		*//**********************************************************************************/
		bool Contains(size_type index) const override
		{
			//if unused index return false
			if (IsValid(index) == false || m_index[index] >= m_dense.size())
				return false;

			return m_sparse[m_index[index]] == index;
		}

		virtual ~SparseContainer() = default;

		T* At(const size_type index)
		{
			if (IsAvailable(index) == true)
				return nullptr;
			return &(m_dense[m_index[index]]);
		}

		const T* At(const size_type index) const
		{
			if (IsAvailable(index) == true)
				return nullptr;
			return &(m_dense[m_index[index]]);
		}

		T& AtIndex(const size_type index)
		{
			return m_dense[index];
		}

		const T& AtIndex(const size_type index) const
		{
			return m_dense[index];
		}

		typename sparse_container::value_type AtIndexSparse(const size_type index) const
		{
			return m_sparse[index];
		}

		T* operator[](const size_type index)
		{
			return At(index);
		}

		const T* operator[](const size_type index) const
		{
			return At(index);
		}

		size_type Size() const
		{
			return static_cast<size_type>(m_dense.size());
		}

		dense_container& GetContainer()
		{
			return m_dense;
		}

		const dense_container& GetContainer() const
		{
			return m_dense;
		}

		T* insert(const size_type index, T& element)
		{
			if (IsAvailable(index) == false)
				return nullptr;
			m_dense.emplace_back(element);
			m_sparse.emplace_back(index);
			m_index[index] = static_cast<typename index_container::value_type>(m_dense.size() - 1);
			return At(index);
		}

		template <typename... args>
		T* emplace(const size_type index, args&&... arguementList)
		{
			if (IsAvailable(index) == false)
				return nullptr;
			m_dense.emplace_back(std::forward<args>(arguementList)...);
			m_sparse.emplace_back(index);
			m_index[index] = static_cast<typename index_container::value_type>(m_dense.size() - 1);

			return At(index);
		}

		virtual bool Remove(const size_type index)
		{
			if (IsAvailable(index))
				return false;

			auto index_to_remove = m_index[index];
			auto last_index = m_sparse.back();

			std::swap(m_dense[index_to_remove], m_dense.back());
			std::swap(m_sparse[index_to_remove], m_sparse.back());

			m_index[last_index] = index_to_remove;


			m_dense.pop_back();
			m_sparse.pop_back();
			return true;
		}

		void Clear()
		{
			m_dense.clear();
			m_sparse.clear();
			std::fill(m_index.begin(), m_index.end(), MAX_ID);
		}

		iterator begin() { return m_dense.begin(); }
		iterator end() { return m_dense.end(); }

		void Swap(size_type index1, size_type index2)
		{
			if (index1 >= m_dense.size() || index2 >= m_dense.size() || index1 == index2)
				return;
			std::swap(*(m_dense.begin() + index1), *(m_dense.begin() + index2));
			std::swap(*(m_sparse.begin() + index1), *(m_sparse.begin() + index2));
			std::swap(m_index[*(m_sparse.begin() + index1)], m_index[*(m_sparse.begin() + index2)]);
		}

		/*********************************************************************************//*!
        \brief    Shift elements from start_index all the way to end of container, to index
        indicated by insert_index. 
         
        \param    insert_index
        index at which elements shifted into, e.g if this is index 2, the shifted elements will be 
        inserted such that the first of the shifted elements will now be at index 2
        \param    start_index
        index of the first element to be shifted from, elements starting from this element and onwards,
        all the way to the end will be shifted
        */ /**********************************************************************************/
		void ShiftElementsFromBackToIndex(size_type insert_index, size_type start_index)
		{
			if (start_index >= m_dense.size())
			{
				LOG_ENGINE_WARN("SwapElementsAtBack start_index:{0} invalid", start_index);
				return;
			}
			dense_container d_temp;
			sparse_container s_temp;
			for (typename dense_container::iterator i = m_dense.begin() + start_index; i != m_dense.end(); ++i)
				d_temp.emplace_back((*i));
			for (typename sparse_container::iterator i = m_sparse.begin() + start_index; i != m_sparse.end(); ++i)
				s_temp.emplace_back((*i));

			std::size_t num = d_temp.size();
			while (num)
			{
				m_dense.pop_back();
				m_sparse.pop_back();
				--num;
			}

			typename dense_container::iterator dense_current = m_dense.begin() + insert_index;
			typename sparse_container::iterator sparse_current = m_sparse.begin() + insert_index;
			for (std::size_t i = 0; i < d_temp.size(); ++i)
			{
				dense_current = m_dense.emplace(dense_current, d_temp[i]);
				sparse_current = m_sparse.emplace(sparse_current, s_temp[i]);

				m_index[*sparse_current] = static_cast<typename index_container::value_type>(dense_current - m_dense.begin());
			}
		}

		/*********************************************************************************//*!
        \brief    Reorders the entire sparse container with the data provided by new_container 
        \param    new_container
        a container of type sparse_container which contains the new arrangement of elements
        */ /**********************************************************************************/
		void ReorderFromIndexArray(sparse_container& new_container)
		{
			dense_container new_dense;
			new_dense.reserve(m_dense.size());
			typename sparse_container::value_type index = 0;
			for (auto i : new_container)
			{
				new_dense.emplace_back(*((*this)[i]));
				m_index[i] = index;
				++index;
			}
			m_dense = std::move(new_dense);
			m_sparse = std::move(new_container);
		}

		dense_container& GetDenseContainer()
		{
			return m_dense;
		}

		const dense_container& GetDenseContainer() const
		{
			return m_dense;
		}

		size_type GetIndex(Entity e) const
		{
			if (IsAvailable(static_cast<size_type>(e)))
				return Size();
			return m_index[e];
		}

	private:
	};
}
