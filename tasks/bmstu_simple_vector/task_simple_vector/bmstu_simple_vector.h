#include <ostream>
#include <stdexcept>
#include <utility>
#include "array_ptr.h"

namespace bmstu
{
template <typename T>
class simple_vector
{
   public:
	class iterator
	{
	   public:
		using iterator_category = std::contiguous_iterator_tag;
		using value_type = T;
		using pointer = T*;
		using reference = T&;
		using difference_type = std::ptrdiff_t;

		// Конструктор по умолчанию
		iterator() = default;

		// Конструктор копирования
		iterator(const iterator& other) = default;

		// Конструктор для работы с nullptr
		iterator(std::nullptr_t) noexcept : ptr_(nullptr) {}

		// Конструктор перемещения
		iterator(iterator&& other) noexcept = default;

		// Приведение типа
		explicit iterator(pointer ptr) : ptr_(ptr) {}

		// Оператор разыменования
		reference operator*() const { return *ptr_; }

		// Оператор доступа к членам
		pointer operator->() const { return ptr_; }

		// Совместимость со старым кодом
		friend pointer to_address(const iterator& it) noexcept
		{
			return it.ptr_;
		}

		// Оператор копирующего присваивания
		iterator& operator=(const iterator& other) = default;

		// Оператор перемещающего присваивания
		iterator& operator=(iterator&& other) = default;

#pragma region Operators
		iterator& operator++()
		{
			++ptr_;
			return *this;
		}

		iterator& operator--()
		{
			--ptr_;
			return *this;
		}

		iterator operator++(int)
		{
			iterator temp = *this;
			++ptr_;
			return temp;
		}

		iterator operator--(int)
		{
			iterator temp = *this;
			--ptr_;
			return temp;
		}

		// Оператор приведения к bool
		explicit operator bool() const { return ptr_ != nullptr; }

		// Операторы сравнения
		auto operator<=>(const iterator& other) const = default;
		friend bool operator==(const iterator& lhs, const iterator& rhs)
		{
			return lhs.ptr_ == rhs.ptr_;
		}

		friend bool operator==(const iterator& lhs, std::nullptr_t)
		{
			return lhs.ptr_ == nullptr;
		}

		iterator& operator=(std::nullptr_t) noexcept
		{
			ptr_ = nullptr;
			return *this;
		}

		friend bool operator==(std::nullptr_t, const iterator& rhs)
		{
			return rhs.ptr_ == nullptr;
		}

		friend bool operator!=(const iterator& lhs, const iterator& rhs)
		{
			return lhs.ptr_ != rhs.ptr_;
		}

		iterator operator+(const difference_type& n) const noexcept
		{
			return iterator(ptr_ + n);
		}

		iterator operator+=(const difference_type& n) noexcept
		{
			ptr_ += n;
			return *this;
		}

		iterator operator-(const difference_type& n) const noexcept
		{
			return iterator(ptr_ - n);
		}

		iterator operator-=(const difference_type& n) noexcept
		{
			ptr_ -= n;
			return *this;
		}

		// Расстояние между двумя итераторами
		friend difference_type operator-(const iterator& end,
										 const iterator& begin) noexcept
		{
			return end.ptr_ - begin.ptr_;
		}

#pragma endregion
	   private:
		pointer ptr_ = nullptr;
	};

	using const_iterator = iterator;

	// Конструктор по умолчанию
	simple_vector() noexcept = default;

	// Деструктор
	~simple_vector() = default;

	// Конструктор через initializer list
	simple_vector(std::initializer_list<T> init) noexcept
		: size_(init.size()), capacity_(init.size())
	{
		if (size_ > 0)
		{
			data_ = array_ptr<T>(size_);
			size_t i = 0;

			for (auto it = init.begin(); it != init.end(); ++it)
				data_.get()[i++] = *it;
		}
	}

	// Конструктор копирования
	simple_vector(const simple_vector& other)
		: size_(other.size_), capacity_(other.capacity_)
	{
		if (size_ > 0)
		{
			data_ = array_ptr<T>(size_);
			for (size_t i = 0; i < size_; ++i)
				data_[i] = other.data_[i];
		}
	}
	// Конструктор перемещения
	// this изначально пуст, меняемся с other
	simple_vector(simple_vector&& other) noexcept { swap(other); }

	// Оператор копирующего присваивания
	simple_vector& operator=(const simple_vector& other)
	{
		if (this != &other)
		{
			// временная копия other
			simple_vector temp(other);
			swap(temp);
		}
		return *this;
	}

	// Оператор перемещающего присваивания
	simple_vector& operator=(simple_vector&& dying) noexcept
	{
		if (this != &dying)
		{
			data_ = std::move(dying.data_);

			size_ = dying.size_;
			capacity_ = dying.capacity_;

			dying.size_ = 0;
			dying.capacity_ = 0;
		}
		return *this;
	}

	// Конструктор по размеру
	simple_vector(size_t size, const T& value = T{})
		: size_(size), capacity_(size)
	{
		if (size_ > 0)
		{
			data_ = array_ptr<T>(size_);

			for (size_t i = 0; i < size_; ++i)
				data_.get()[i] = value;
		}
	}

	// Указатель на первый элемент
	iterator begin() noexcept { return iterator(data_.get()); }

	// Указатель на последний элемент
	iterator end() noexcept { return iterator(data_.get() + size_); }

	// Указатель на первый элемент (const)
	const_iterator begin() const noexcept { return iterator(data_.get()); }

	// Указатель на последний элемент (const)
	const_iterator end() const noexcept
	{
		return iterator(data_.get() + size_);
	}

	// Обращение по индексу
	typename iterator::reference operator[](size_t index) noexcept
	{
		return data_[index];
	}

	// Обращение по индексу (const)
	const typename const_iterator::reference operator[](
		size_t index) const noexcept
	{
		return data_.get()[index];
	}

	typename iterator::reference at(size_t index)
	{
		if (index >= size_)
			throw std::out_of_range("Index out of range");
		return data_[index];
	}

	typename const_iterator::reference at(size_t index) const
	{
		if (index >= size_)
			throw std::out_of_range("Index out of range");
		return data_[index];
	}

	size_t size() const noexcept { return size_; }

	size_t capacity() const noexcept { return capacity_; }

	// Меняет вектора между собой
	void swap(simple_vector& other) noexcept
	{
		data_.swap(other.data_);
		std::swap(size_, other.size_);
		std::swap(capacity_, other.capacity_);
	}

	// Глобальная версия swap для совместимости
	friend void swap(simple_vector& lhs, simple_vector& rhs) noexcept
	{
		lhs.swap(rhs);
	}

	// Управление вместимостью
	void reserve(size_t new_cap)
	{
		if (new_cap <= capacity_)
			return;

		array_ptr<T> new_data(new_cap);

		for (size_t i = 0; i < size_; ++i)
		{
			new_data[i] = std::move(data_[i]);
		}

		data_.swap(new_data);
		capacity_ = new_cap;
	}

	// Изменение размера
	void resize(size_t new_size)
	{
		if (new_size > capacity_)
		{
			size_t new_cap = std::max(new_size, capacity_ * 2);
			reserve(new_cap);
		}

		if (new_size > size_)
		{
			for (size_t i = size_; i < new_size; ++i)
				data_[i] = T{};
		}
		size_ = new_size;
	}

	// insert для перемещения
	iterator insert(const_iterator where, T&& value)
	{
		size_t index = where - begin();
		T temp = std::move(value);

		if (size_ == capacity_)
		{
			size_t new_cap = capacity_ == 0 ? 1 : capacity_ * 2;
			reserve(new_cap);
		}

		for (size_t i = size_; i > index; --i)
			data_[i] = std::move(data_[i - 1]);

		data_[index] = std::move(temp);
		++size_;

		return begin() + index;
	}

	// insert для копирования
	iterator insert(const_iterator where, const T& value)
	{
		size_t index = where - begin();

		T temp = value;

		if (size_ == capacity_)
		{
			size_t new_cap = capacity_ == 0 ? 1 : capacity_ * 2;
			reserve(new_cap);
		}

		for (size_t i = size_; i > index; --i)
			data_[i] = std::move(data_[i - 1]);

		data_[index] = std::move(temp);
		++size_;

		return begin() + index;
	}

	// Добавление элемента с перемещением
	void push_back(T&& value)
	{
		insert(end(), std::move(value));

		// if (size_ == capacity_)
		// {
		// 	T temp = std::move(value);
		// 	size_t new_cap = capacity_ == 0 ? 1 : capacity_ * 2;
		// 	reserve(new_cap);
		// 	data_[size_] = std::move(temp);
		// }
		// else
		// {
		// 	data_[size_] = std::move(value);
		// }
		// ++size_;
	}

	void clear() noexcept { size_ = 0; }

	// Добавление элемента с копированием
	void push_back(const T& value)
	{
		insert(end(), value);
		// if (size_ == capacity_)
		// {
		// 	T temp = value;
		// 	size_t new_cap = capacity_ == 0 ? 1 : capacity_ * 2;
		// 	reserve(new_cap);
		// 	data_[size_] = std::move(temp);
		// }
		// else
		// {
		// 	data_[size_] = value;
		// }
		// ++size_;
	}

	bool empty() const noexcept { return size_ == 0; }

	// Удаление элемента с конца массива
	void pop_back()
	{
		if (size_ > 0)
			--size_;
	}

	friend bool operator==(const simple_vector& lhs, const simple_vector& rhs)
	{
		if (lhs.size() != rhs.size())
			return false;

		for (size_t i = 0; i < lhs.size(); ++i)
		{
			if (lhs[i] != rhs[i])
				return false;
		}

		return true;
	}

	friend bool operator!=(const simple_vector& lhs, const simple_vector& rhs)
	{
		if (lhs.size() != rhs.size())
			return true;

		for (size_t i = 0; i < lhs.size(); ++i)
		{
			if (lhs[i] != rhs[i])
				return true;
		}
		return false;
	}

	// Оператор сравнения
	friend auto operator<=>(const simple_vector& lhs, const simple_vector& rhs)
	{
		if (alphabet_compare(lhs, rhs))
			return -1;

		if (alphabet_compare(rhs, lhs))
			return 1;

		return 0;
	}

	// Оператор потока вывода
	friend std::ostream& operator<<(std::ostream& os, const simple_vector& vec)
	{
		os << "[";
		for (size_t i = 0; i < vec.size(); ++i)
		{
			if (i > 0)
			{
				os << ", ";
			}
			os << vec[i];
		}
		os << "]";
		return os;
	}
	// Удаление элемента внутри массива
	iterator erase(iterator where)
	{
		size_t index = where - begin();
		for (size_t i = index; i < size_ - 1; ++i)
			data_[i] = data_[i + 1];

		--size_;
		return begin() + index;
	}

   private:
	static bool alphabet_compare(const simple_vector<T>& lhs,
								 const simple_vector<T>& rhs)
	{
		size_t min_size = (lhs.size() < rhs.size()) ? lhs.size() : rhs.size();

		for (size_t i = 0; i < min_size; ++i)
		{
			// возвращаем true, если левый меньше правого
			if (lhs[i] < rhs[i])
				return true;

			// возвращаем false, если правый меньше левого
			if (rhs[i] < lhs[i])
				return false;
		}

		// меньший тот, что короче
		return lhs.size() < rhs.size();
	}
	array_ptr<T> data_;
	size_t size_ = 0;
	size_t capacity_ = 0;
};
}  // namespace bmstu
