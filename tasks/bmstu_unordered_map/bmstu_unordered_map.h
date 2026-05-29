#pragma once
#include <cstring>
#include <list>
#include <utility>
#include <vector>
#include "fast_streebog.h"

namespace bmstu
{
template <typename item>
struct equal_to
{
	bool operator()(const item& lhs, const item& rhs) const
	{
		return lhs == rhs;
	}
};

template <typename K>
struct hash
{
	size_t operator()(const K& key) const
	{
		uint8_t digest[32];
		streebog_hash_256(reinterpret_cast<const uint8_t*>(&key), sizeof(K),
						  digest);
		std::size_t result = 0;
		std::memcpy(&result, digest, sizeof(std::size_t));
		return result;
	}
};

template <>
struct hash<const char*>
{
	size_t operator()(const char* str) const
	{
		uint8_t digest[32];
		size_t string_size = strlen(str);
		streebog_hash_256(reinterpret_cast<const uint8_t*>(str), string_size,
						  digest);
		std::size_t result = 0;
		std::memcpy(&result, digest, sizeof(std::size_t));
		return result;
	}
};

template <>
struct hash<size_t>
{
	size_t operator()(const size_t& key) const { return key; }
};

template <>
struct hash<int>
{
	size_t operator()(const int& key) const { return static_cast<size_t>(key); }
};

template <>
struct hash<char>
{
	size_t operator()(const char& key) const
	{
		return static_cast<size_t>(key);
	}
};

template <>
struct hash<double>
{
	size_t operator()(const double& key) const
	{
		size_t result;
		std::memcpy(&result, &key, sizeof(double));
		return result;
	}
};

template <typename K,
		  typename V,
		  typename Hash = hash<K>,
		  typename KeyEqual = equal_to<K>>
class unordered_map
{
   public:
	using size_type = size_t;
	using key_type = K;
	using mapped_type = V;
	// Пара ключ-значение, const для невозможности изменения ключа
	using value_type = std::pair<const K, V>;

   private:
	// число дефолтных бакетов
	static constexpr size_type DEFAULT_BUCKET_COUNT = 7;
	// фактор максимальной загрузки
	static constexpr double MAX_LOAD_FACTOR = 0.75;

	size_t bucket_for(const key_type& key) const
	{
		return hash_(key) % buckets_.size();
	}

	using bucket_type = std::list<value_type>;

	void rehash(size_t new_bucket_count)
	{
		std::vector<bucket_type> next(new_bucket_count);
		for (auto& bkt : buckets_)
		{
			for (auto& pair : bkt)
			{
				size_type idx = hash_(pair.first) % new_bucket_count;
				next[idx].push_back(std::move(pair));
			}
		}
		buckets_ = std::move(next);
	}

	std::vector<bucket_type> buckets_;
	size_type size_;
	Hash hash_;
	KeyEqual equal_;

   public:
	explicit unordered_map(size_type bucket_count = DEFAULT_BUCKET_COUNT)
		: buckets_(bucket_count), size_(0)
	{
	}

	// не реализуется по правилу нуля (отдается компилятору)
	// конструктор копирования
	unordered_map(const unordered_map&) = default;
	// конструктор перемещения
	unordered_map(unordered_map&&) = default;
	// оператор копирующего присваивания
	unordered_map& operator=(const unordered_map&) = default;
	// оператор перемещающего присваивания
	unordered_map& operator=(unordered_map&&) = default;
	// деструктор
	~unordered_map() = default;

	class iterator
	{
	   public:
		using iterator_category = std::forward_iterator_tag;
		using value_type = unordered_map::value_type;
		using pointer = value_type*;
		using reference = value_type&;
		using difference_type = std::ptrdiff_t;

		iterator() = default;

		// Итератор должен знать о buckets_ родительского мапа, чтобы корректно
		// переходить к следующему бакету
		iterator(std::vector<bucket_type>* buckets_ptr,
				 size_type bucket_index,
				 typename bucket_type::iterator list_it)
			: buckets_ptr_(buckets_ptr),
			  bucket_index_(bucket_index),
			  list_it_(list_it) {};

		iterator(const iterator& other) = default;
		iterator(iterator&& other) noexcept = default;

		reference operator*() const { return *list_it_; }

		pointer operator->() const { return &(*list_it_); }

		friend pointer to_address(const iterator& it) noexcept
		{
			return &(*it.list_it_);
		}

		iterator& operator=(const iterator& other) = default;

		iterator& operator=(iterator&& other) noexcept = default;

#pragma region Operators
		iterator& operator++()
		{
			++list_it_;
			while (bucket_index_ < buckets_ptr_->size() &&
				   list_it_ == (*buckets_ptr_)[bucket_index_].end())
			{
				++bucket_index_;
				if (bucket_index_ < buckets_ptr_->size())
					list_it_ = (*buckets_ptr_)[bucket_index_].begin();
			}
			return *this;
		}

		// реализуем потом для bidirectional
		iterator& operator--() = delete;

		iterator operator++(int)
		{
			iterator tmp = *this;
			++(*this);
			return tmp;
		}

		iterator operator--(int) = delete;

		// Оператор приведения к bool
		explicit operator bool() const { return buckets_ptr_ != nullptr; }

		bool operator==(const iterator& o) const
		{
			bool at_end =
				!buckets_ptr_ || bucket_index_ >= buckets_ptr_->size();
			bool o_at_end =
				!o.buckets_ptr_ || o.bucket_index_ >= o.buckets_ptr_->size();

			if (at_end && o_at_end)
			{
				return true;
			}
			if (at_end != o_at_end)
			{
				return false;
			}
			return buckets_ptr_ == o.buckets_ptr_ &&
				   bucket_index_ == o.bucket_index_ && list_it_ == o.list_it_;
		}

		bool operator!=(const iterator& o) const { return !(*this == o); }

		iterator& operator=(std::nullptr_t) noexcept
		{
			buckets_ptr_ = nullptr;
			return *this;
		}

#pragma endregion
	   private:
		std::vector<bucket_type>* buckets_ptr_ = nullptr;
		size_type bucket_index_ = 0;
		typename bucket_type::iterator list_it_;
	};

#pragma region HASHPOLICY

	double load_factor() const
	{
		return static_cast<double>(size_) /
			   static_cast<double>(buckets_.size());
	}
	size_type bucket_count() const { return buckets_.size(); }

	iterator begin()
	{
		for (size_t i = 0; i < buckets_.size(); ++i)
		{
			if (!buckets_[i].empty())
			{
				return iterator(&buckets_, i, buckets_[i].begin());
			}
		}
		return end();
	}

	iterator end()
	{
		return iterator(&buckets_, buckets_.size(),
						typename bucket_type::iterator());
	}

	iterator find(const K& key)
	{
		size_type idx = bucket_for(key);
		for (auto it = buckets_[idx].begin(); it != buckets_[idx].end(); ++it)
		{
			if (equal_(it->first, key))
			{
				return iterator(&buckets_, idx, it);
			}
		}
		return end();
	}

	iterator insert(const value_type& value)
	{
		auto it = find(value.first);
		if (it != end())
		{
			it->second = value.second;	// обновляем ключ
			return it;
		}

		if (load_factor() > MAX_LOAD_FACTOR)
		{
			rehash(bucket_count() * 2);
		}
		size_type idx = bucket_for(value.first);
		buckets_[idx].push_front(value);
		++size_;
		return iterator(&buckets_, idx, buckets_[idx].begin());
	}

	V& operator[](const K& key)
	{
		auto it = find(key);
		if (it != end())
		{
			return it->second;
		}
		else
		{
			if (load_factor() > MAX_LOAD_FACTOR)
			{
				rehash(bucket_count() * 2);
			}
			size_type idx = bucket_for(key);
			buckets_[idx].emplace_back(key, V());
			++size_;
			return buckets_[idx].back().second;
		}
	}

	bool erase(const K& key)
	{
		size_type idx = bucket_for(key);
		auto& bucket = buckets_[idx];
		for (auto it = bucket.begin(); it != bucket.end(); ++it)
		{
			if (equal_(it->first, key))
			{
				bucket.erase(it);
				--size_;
				return true;
			}
		}
		return false;
	}

#pragma endregion
};

}  // namespace bmstu