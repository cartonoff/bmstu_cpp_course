#pragma once

#include <exception>
#include <iostream>

namespace bmstu
{
template <typename T>
class simple_basic_string;

typedef simple_basic_string<char> string;
typedef simple_basic_string<wchar_t> wstring;
typedef simple_basic_string<char16_t> u16string;
typedef simple_basic_string<char32_t> u32string;

template <typename T>
class simple_basic_string
{
   public:
	/// Конструктор по умолчанию
	simple_basic_string() : ptr_(new T[1]{0}), size_(0) {}

	// Конструктор с размером
	simple_basic_string(size_t size) : ptr_(new T[size + 1]), size_(size)
	{
		for (size_t i = 0; i < size_; i++)
		{
			ptr_[i] = ' ';
		}

		ptr_[size_] = T(0);
	}

	// Конструктор через Initializer list
	simple_basic_string(std::initializer_list<T> il)
		: ptr_(new T[il.size() + 1]), size_(il.size())
	{
		size_t i = 0;
		for (const T& item : il)
		{
			ptr_[i++] = item;
		}
		ptr_[size_] = T(0);
	}

	/// Конструктор с параметром си-с
	simple_basic_string(const T* c_str)
	{
		size_ = strlen_(c_str);
		ptr_ = new T[size_ + 1];
		for (size_t i = 0; i < size_; i++)
		{
			ptr_[i] = c_str[i];
		}
		ptr_[size_] = T(0);
	}

	/// Конструктор копирования
	simple_basic_string(const simple_basic_string& other) : size_(other.size_)
	{
		ptr_ = new T[size_ + 1];
		for (size_t i = 0; i < size_; i++)
		{
			ptr_[i] = other.ptr_[i];
		}
		ptr_[size_] = T(0);
	}

	/// Перемещающий конструктор
	simple_basic_string(simple_basic_string&& dying)
		: ptr_(dying.ptr_), size_(dying.size_)
	{
		dying.ptr_ = new T[1]{T(0)};
		dying.size_ = 0;
	}

	/// Деструктор
	~simple_basic_string() { clean_(); }

	/// Геттер на си-строку
	const T* c_str() const { return ptr_; }

	size_t size() const { return size_; }

	/// Оператор перемещающего присваивания
	simple_basic_string& operator=(simple_basic_string&& other)
	{
		if (this != &other)
		{
			clean_();
			ptr_ = other.ptr_;
			size_ = other.size_;
			other.ptr_ = new T[1]{T(0)};
			other.size_ = 0;
		}

		return *this;
	}

	/// Оператор копирующего присваивания си  строки
	simple_basic_string& operator=(const T* c_str)
	{
		if (ptr_ == c_str)
			return *this;

		size_t new_size = strlen_(c_str);
		T* new_ptr = new T[new_size + 1];

		for (size_t i = 0; i < new_size; i++)
		{
			new_ptr[i] = c_str[i];
		}
		new_ptr[new_size] = T(0);
		clean_();
		ptr_ = new_ptr;
		size_ = new_size;
		return *this;
	}

	/// Оператор копирующего присваивания
	simple_basic_string& operator=(const simple_basic_string& other)
	{
		if (this != &other)
		{
			T* new_ptr = new T[other.size_ + 1];
			for (size_t i = 0; i < other.size_; i++)
			{
				new_ptr[i] = other.ptr_[i];
			}

			new_ptr[other.size_] = T(0);

			clean_();
			ptr_ = new_ptr;
			size_ = other.size_;
		}

		return *this;
	}

	friend simple_basic_string<T> operator+(const simple_basic_string<T>& left,
											const simple_basic_string<T>& right)
	{
		simple_basic_string<T> result(left.size_ + right.size_);

		size_t k = 0;
		for (size_t i = 0; i < left.size_; i++)
		{
			result.ptr_[k++] = left.ptr_[i];
		}
		for (size_t i = 0; i < right.size_; i++)
		{
			result.ptr_[k++] = right.ptr_[i];
		}

		result.ptr_[result.size_] = T(0);

		return result;
	}

	template <typename S>
	friend S& operator<<(S& os, const simple_basic_string& obj)
	{
		for (size_t i = 0; i < obj.size_; i++)
		{
			os << obj.ptr_[i];
		}

		return os;
	}

	template <typename S>
	friend S& operator>>(S& is, simple_basic_string& obj)
	{
		obj.clean_();
		obj.ptr_ = new T[1]{T(0)};

		T ch;
		while (is.get(ch))
		{
			obj += ch;
		}

		return is;
	}

	simple_basic_string& operator+=(const simple_basic_string& other)
	{
		size_t new_size = size_ + other.size_;
		T* new_ptr = new T[new_size + 1];

		for (size_t i = 0; i < size_; ++i)
		{
			new_ptr[i] = ptr_[i];
		}

		for (size_t i = 0; i < other.size_; ++i)
		{
			new_ptr[i] = other.ptr_[i];
		}

		new_ptr[new_size] = T(0);

		clean_();
		ptr_ = new_ptr;
		size_ = new_size;

		return *this;
	}

	simple_basic_string& operator+=(T symbol)
	{
		size_t new_size = size_ + 1;
		T* new_ptr = new T[new_size + 1];

		for (size_t i = 0; i < size_; i++)
		{
			new_ptr[i] = ptr_[i];
		}
		new_ptr[size_] = symbol;
		new_ptr[new_size] = T(0);

		ptr_ = new_ptr;
		size_ = new_size;

		return *this;
	}

	T& operator[](size_t index) noexcept { return *(ptr_ + index); }

	T& at(size_t index) { throw std::out_of_range("Wrong index"); }

	T* data() { return ptr_; }

   private:
	static size_t strlen_(const T* str)
	{
		size_t len = 0;
		while (str != nullptr && str[len] != T(0))
		{
			++len;
		}
		return len;
	}

	void clean_()
	{
		delete[] ptr_;
		ptr_ = nullptr;
		size_ = 0;
	}

	T* ptr_ = nullptr;
	size_t size_;
};
}  // namespace bmstu