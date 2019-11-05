#pragma once

#include <type_traits>
#include "tools.h"

XKEINNAMESPACE_START
	template<class _Ty = int>
	class Point
	{
		typedef Point<_Ty> _MyT;

		template<class _rTy>
		inline _MyT& operator=(const Point<_rTy>& other) _NOEXCEPT
		{
			x = other.x;
			y = other.y;
		}

		_Ty x;
		_Ty y;
	};

	template<class _Ty = int>
	class Rectangle
	{
		typedef Rectangle<_Ty> _MyT;

		template<class _rTy>
		inline _MyT& operator=(const Rectangle<_rTy>& other) _NOEXCEPT
		{
			location = other.location;
			size = other.size;
		}

#pragma warning(push)
#pragma warning(disable: 4201)
		union {
			Point<_Ty> location;
			struct {
				_Ty x;
				_Ty y;
			};
		};
		union {
			Point<_Ty> size;
			struct {
				_Ty width;
				_Ty height;
			};
		};
#pragma warning(pop)
	};

	template<class _Ty = void*, size_t _dimension = 1>
	class Array
	{
		typedef Array<_Ty, _dimension> _MyT;
		using iterator = _Ty*;

		class ArrayIterator
		{
			friend class _MyT;
		public:
			operator _Ty&() const
			{
				return (*_Ptr);
			}

			_Ty* operator&() const
			{
				return _Ptr;
			}

			ArrayIterator& operator[](int offset)
			{
				ArrayIterator* pArrayIterator = _EachLength == 1 ? this : this + 1;
				pArrayIterator->_Ptr = _Ptr + pArrayIterator->_EachLength * offset;
				return (*pArrayIterator);
			}
		private:
			_Ty* _Ptr;
			size_t _EachLength;
		};

	public:
		Array(const _MyT& other) _NOEXCEPT
		{
			memcpy_s(this, sizeof(_MyT), &other, sizeof(_MyT));
			_IAllocated = false;
		}

		Array(const size_t(&_dimensionRule)[_dimension], void* _ptr = nullptr) _NOEXCEPT_COND(_NOEXCEPT_OPER(std::is_nothrow_default_constructible_v<_Ty>))
		{
			size_t arraySize = 1;
			for (int idx = 0; idx < _dimension; idx++) {
				arraySize *= _DimensionRule[idx] = _dimensionRule[idx];
			}
			_Length = arraySize;
			_Ptr = (_IAllocated = _ptr == nullptr) ? new _Ty[arraySize] : (_Ty*)_ptr;
			for (size_t idx = 0; idx < _dimension; idx++) {
				arraySize /= _dimensionRule[idx];
				_ArrayIterator[idx]._Ptr = nullptr;
				_ArrayIterator[idx]._EachLength = arraySize;
			}
		}

		~Array() _NOEXCEPT_COND(_NOEXCEPT_OPER(std::is_nothrow_destructible_v<_Ty>))
		{
			_Length = 0;
			if (_IAllocated) {
				_IAllocated = false;
				delete[] _Ptr;
			}
		}

		ArrayIterator& operator[](int offset)
		{
			_ArrayIterator[0]._Ptr = _Ptr + _ArrayIterator[0]._EachLength * offset;
			return (_ArrayIterator[0]);
		}

		_Ty& operator*() const
		{
			return (*_Ptr);
		}

		iterator begin() const _NOEXCEPT
		{
			return _Ptr;
		}

		iterator end() const _NOEXCEPT
		{
			return _Ptr + _Length;
		}

	protected:
		bool _IAllocated;
		ArrayIterator _ArrayIterator[_dimension];
		_Ty* _Ptr;
		size_t _Length;
		size_t _DimensionRule[_dimension];
	};

	template<class _Ty = void*>
	class DynamicArray
	{
	protected:
		typedef DynamicArray<_Ty> _MyT;
		using iterator = Iterator<_Ty>;
		static constexpr size_t _firstCapacityIncreament = 0x10;
	public:
		static constexpr size_t _invaildPos = size_t(-1);
	protected:
		inline void move_range(_Ty* _first, _Ty* _end, _Ty* _dest) _NOEXCEPT_COND(_NOEXCEPT_OPER(std::is_nothrow_copy_constructible_v<_Ty> && std::is_nothrow_destructible_v<_Ty>))
		{
			if (_first >= _end || _dest == _first) return;
			if (_dest > _first) {
				_dest += _end - _first;
				for (_Ty* _cur = _end - 1; _cur >= _first; --_cur) {
					new (--_dest)_Ty(std::move(*_cur));
					_cur->~_Ty();
				}
			}
			else {
				for (_Ty* _cur = _first; _cur < _end; ++_cur) {
					new (_dest++)_Ty(std::move(*_cur));
					_cur->~_Ty();
				}
			}
		}

		inline void destruct_range(_Ty* _first, _Ty* _end) _NOEXCEPT_COND(_NOEXCEPT_OPER(std::is_nothrow_destructible_v<_Ty>))
		{
			for (_Ty* _cur = _first; _cur < _end; ++_cur) {
				_cur->~_Ty();
			}
		}

		inline void construct_range(_Ty* _first, _Ty* _end, _Ty* _dest) _NOEXCEPT_COND(_NOEXCEPT_OPER(std::is_nothrow_copy_constructible_v<_Ty>))
		{
			for (_Ty* _cur = _first; _cur < _end; ++_cur) {
				new (_dest++)_Ty(std::move(*_cur));
			}
		}

	public:
		DynamicArray() _NOEXCEPT : _PtrFirst(nullptr), _Ptr(nullptr), _Length(0), _Capacity(0), _CapacityIncreament(_firstCapacityIncreament)
		{
		}

		DynamicArray(size_t _capacity) _NOEXCEPT : _PtrFirst(nullptr), _Ptr(nullptr), _Capacity(0), _Length(0), _CapacityIncreament(_firstCapacityIncreament)
		{
			Alloc(_capacity);
		}

		DynamicArray(const _MyT& other) _NOEXCEPT_COND(_NOEXCEPT_OPER(std::is_nothrow_copy_constructible_v<_Ty>))
		{
			operator=(other);
		}

		DynamicArray(_MyT&& other) _NOEXCEPT
		{
			operator=(std::move(other));
		}
		
		void Insert(const _Ty& _val, size_t idx) _NOEXCEPT_COND(_NOEXCEPT_OPER(std::is_nothrow_copy_constructible_v<_Ty> && std::is_nothrow_destructible_v<_Ty>))
		{
			Insert(&_val, 1, idx);
		}

		void Insert(const _Ty* _first, size_t _length, size_t idx) _NOEXCEPT_COND(_NOEXCEPT_OPER(std::is_nothrow_copy_constructible_v<_Ty>))
		{
			size_t _newLength = _Length;

			if (idx > _newLength) {
				idx = _newLength;
			}
			_newLength += _length;

			if (_newLength <= _Capacity) {
				if (_PtrOffset() >= _length && idx < _Length / 2) {
					move_range(_Ptr, _Ptr + idx, _Ptr - _length);
					_Ptr -= _length;
				}
				else if (_PtrOffset() + _newLength > _Capacity) {
					move_range(_Ptr, _Ptr + idx, _PtrFirst);
					if (_PtrOffset() > 1) {
						move_range(_Ptr + idx, _Ptr + _Length, _PtrFirst + idx + _length);
					}
					_Ptr = _PtrFirst;
				}
				else {
					move_range(_Ptr + idx, _Ptr + _Length, _Ptr + idx + _length);
				}

				Copy(_first, idx, _length);
				_Length = _newLength;
			}
			else {
				const _Ty* _oldPtr = _Ptr;
				const bool _oldEmpty = _Capacity == 0;
				const size_t _oldLength = _Length;
				Alloc(_newLength);
				Copy(_oldPtr, 0, idx);
				Copy(_first, idx, _length);
				Copy(_oldPtr + idx, idx + _length, _oldLength - idx);
				_Length = _newLength;
				if (_oldEmpty == false) {
					destruct_range(const_cast<_Ty*>(_oldPtr), const_cast<_Ty*>(_oldPtr + _oldLength));
					::operator delete[](reinterpret_cast<void*>(const_cast<_Ty*>(_oldPtr)));
				}
			}
		}

		void Erase(size_t idx)
		{
			Erase(begin() + idx);
		}

		void Erase(iterator _first, size_t _length = 1)
		{
			size_t idx = _first - begin();
			if (_Length < idx + _length)
				throw std::out_of_range("xkein::DynamicArray::out of range!");
			destruct_range(_Ptr + idx, _Ptr + idx + _length);
			if (idx >= _Length / 2) {
				move_range(_Ptr + idx + 1, _Ptr + _Length, _Ptr + idx);
			}
			else {
				move_range(_Ptr, _Ptr + idx, _Ptr + 1);
				_Ptr++;
			}

			if (--_Length == 0) {
				_Ptr = _PtrFirst;
			}
		}

		void Erase(_Ty& _ref, size_t _num)
		{
			Erase(Find(_ref, _num));
		}

		void Remove(_Ty& _ref)
		{
			Erase(_ref, 1);
		}

		void Copy(const _Ty* _source, size_t _myPos, size_t _length) _NOEXCEPT_COND(_NOEXCEPT_OPER(std::is_nothrow_copy_constructible_v<_Ty> && std::is_nothrow_destructible_v<_Ty>))
		{
			if (_length == 0)
				return;

			if (_myPos > _Length)
				_myPos = _Length;

			if (_length + _myPos > _Capacity)
			{
				const _Ty* _oldPtr = _Ptr;
				const _Ty* _oldPtrFirst = _PtrFirst;
				const bool _oldEmpty = _Capacity == 0;
				const size_t _oldLength = _Length;
				Alloc(_length + _myPos);
				construct_range(const_cast<_Ty*>(_oldPtr), const_cast<_Ty*>(_oldPtr + _myPos), _Ptr);
				_Length = _length + _myPos;
				if (_oldEmpty == false) {
					destruct_range(const_cast<_Ty*>(_oldPtr), const_cast<_Ty*>(_oldPtr + _oldLength));
					::operator delete[](reinterpret_cast<void*>(const_cast<_Ty*>(_oldPtrFirst)));
				}
			}
			else if (_length + _myPos > _Length)
			{
				if (_length + _myPos + _PtrOffset() > _Capacity) {
					move_range(_Ptr, _Ptr + _myPos, _PtrFirst);
					destruct_range(_Ptr + _myPos, _Ptr + _Length);
					_Ptr = _PtrFirst;
				}
				_Length = _length + _myPos;
			}

			construct_range(const_cast<_Ty*>(_source), const_cast<_Ty*>(_source + _length), _Ptr + _myPos);
		}

		template<int size>
		void Copy(const _Ty(&_ptr)[size], size_t _myPos = 0, int _length = size) _NOEXCEPT_COND(_NOEXCEPT_OPER(std::is_nothrow_copy_constructible_v<_Ty> && std::is_nothrow_destructible_v<_Ty>))
		{
			Copy((const _Ty*)_ptr, _myPos, _length);
		}

		void Alloc(size_t _length) _NOEXCEPT
		{
			if (_length <= _Capacity)
				return;
			do {
				_Capacity += _CapacityIncreament;
				if (_CapacityIncreament < USN_PAGE_SIZE)
					_CapacityIncreament <<= 1;
			} while (_length > _Capacity);
			_PtrFirst = _Ptr = static_cast<_Ty*>(::operator new(sizeof(_Ty)*(_Capacity)));
			_Length = 0;
		}

		void PushBack(const _Ty& _val) _NOEXCEPT_COND(_NOEXCEPT_OPER(std::is_nothrow_copy_constructible_v<_Ty> && std::is_nothrow_destructible_v<_Ty>))
		{
			construct_back(_val);
		}

		void PushBackUnique(const _Ty& _val) _NOEXCEPT_COND(_NOEXCEPT_OPER(std::is_nothrow_copy_constructible_v<_Ty> && std::is_nothrow_destructible_v<_Ty>))
		{
			if (Find(_val, 1) == _invaildPos) {
				PushBack(_val);
			}
		}

		void InsertUnique(const _Ty& _val, size_t idx) _NOEXCEPT_COND(_NOEXCEPT_OPER(std::is_nothrow_copy_constructible_v<_Ty> && std::is_nothrow_destructible_v<_Ty>))
		{
			if (Find(_val, 1) == _invaildPos) {
				Insert(_val, idx);
			}
		}

		~DynamicArray() _NOEXCEPT_COND(_NOEXCEPT_OPER(std::is_nothrow_destructible_v<_Ty>))
		{
			Clear();
		}

		void Clear() _NOEXCEPT_COND(_NOEXCEPT_OPER(std::is_nothrow_destructible_v<_Ty>))
		{
			if (_Capacity) {
				destruct_range(_Ptr, _Ptr + _Length);
				::operator delete[](_PtrFirst);
				_Capacity = 0;
				_Length = 0;
				_CapacityIncreament = _firstCapacityIncreament;
				_PtrFirst = nullptr;
				_Ptr = nullptr;
			}
		}

		_MyT& operator=(const _MyT& other) _NOEXCEPT_COND(_NOEXCEPT_OPER(std::is_nothrow_copy_constructible_v<_Ty> && std::is_nothrow_destructible_v<_Ty>))
		{
			Copy(other._Ptr, 0, other._Length);
			_Capacity = other._Capacity;
			_Length = other._Length;
			_CapacityIncreament = other._CapacityIncreament;
			_PtrFirst = other._PtrFirst;
			_Ptr = other._Ptr;
			return (*this);
		}

		_MyT& operator=(_MyT&& other) _NOEXCEPT
		{
			_Capacity = other._Capacity;
			_Length = other._Length;
			_CapacityIncreament = other._CapacityIncreament;
			_PtrFirst = other._PtrFirst;
			_Ptr = other._Ptr;
			other._Capacity = 0;
			other._Length = 0;
			other._CapacityIncreament = _firstCapacityIncreament;
			other._PtrFirst = nullptr;
			other._Ptr = nullptr;
			return (*this);
		}

		_Ty& operator[](int offset) const
		{
			return (_Ptr[offset]);
		}

		_Ty& operator*() const
		{
			return (*_Ptr);
		}

		const iterator begin() const _NOEXCEPT
		{
			return _Ptr;
		}

		const iterator end() const _NOEXCEPT
		{
			return _Ptr + _Length;
		}

		iterator begin() _NOEXCEPT
		{
			return _Ptr;
		}

		iterator end() _NOEXCEPT
		{
			return _Ptr + _Length;
		}

		size_t length() const _NOEXCEPT
		{
			return _Length;
		}

		size_t capacity() const _NOEXCEPT
		{
			return _Capacity;
		}

		_Ty& back() const _NOEXCEPT
		{
			return (_Ptr[_Length - 1]);
		}

		_Ty& front() const _NOEXCEPT
		{
			return (_Ptr[0]);
		}

		template<class _Fn>
		_Fn foreach(_Fn _Func) _NOEXCEPT_COND(_NOEXCEPT_OPER(_Func))
		{
			for (_Ty& ref : *this) {
				if (!_Func(ref))
					break;
			}

			return _Func;
		}

		template<class _Fn>
		_Fn foreach(_Fn _Func) const _NOEXCEPT_COND(_NOEXCEPT_OPER(_Func))
		{
			for (const _Ty& ref : *this) {
				if (!_Func(ref))
					break;
			}

			return _Func;
		}

		_Ty& at(size_t index) const
		{
			if (index < _Length) {
				return _Ptr[index];
			}

			return throw std::out_of_range("xkein::DynamicArray::out of range!"), GetNullReference<_Ty>();
		}

		template<bool compareAddress = false>
		size_t Find(const _Ty& _ref, size_t _num, size_t startPos = 0) const _NOEXCEPT
		{
			const _Ty* _end = _Ptr + _Length;
			for (const _Ty* _cur = _Ptr + startPos; _cur < _end; ++_cur) {
				if ((compareAddress ? _cur == &_ref : *_cur == _ref) && --_num == 0)
					return _cur - _Ptr;
			}

			return _invaildPos;
		}

		template<bool compareAddress = false>
		size_t Find(_Ty& _ref, size_t _num, size_t startPos = 0) _NOEXCEPT
		{
			const _Ty* _end = _Ptr + _Length;
			for (_Ty* _cur = _Ptr + startPos; _cur < _end; ++_cur) {
				if ((compareAddress ? _cur == &_ref : *_cur == _ref) && --_num == 0)
					return _cur - _Ptr;
			}

			return _invaildPos;
		}

		size_t FindRange(const _Ty* _first, const _Ty* _last, size_t _num, size_t startPos = 0) const _NOEXCEPT
		{
			const int endOffset = _Length - (_last - _first);
			if (endOffset <= 0)
				return _invaildPos;
			const _Ty* _end = _Ptr + endOffset;
			for (const _Ty* _cur = _Ptr + startPos; _cur < _end; ++_cur) {
				if (*_cur == *_first && std::equal(_first, _last, _cur, std::equal_to<_Ty>()) && --_num == 0)
					return _cur - _Ptr;
			}

			return _invaildPos;
		}

		size_t FindRange(_Ty* _first, _Ty* _last, size_t _num, size_t startPos = 0) _NOEXCEPT
		{
			const int endOffset = _Length - (_last - _first);
			if (endOffset <= 0)
				return _invaildPos;
			const _Ty* _end = _Ptr + endOffset;
			for (_Ty* _cur = _Ptr + startPos; _cur < _end; ++_cur) {
				if (*_cur == *_first && std::equal(_first, _last, _cur, std::equal_to<_Ty>()) && --_num == 0)
					return _cur - _Ptr;
			}

			return _invaildPos;
		}

		template<class... _ArgTys>
		void construct_back(_ArgTys&&... _Args) _NOEXCEPT_COND(_NOEXCEPT_OPER(
			(std::is_nothrow_constructible_v<_Ty, typename std::add_lvalue_reference<_ArgTys>::type...>)
			&& std::is_nothrow_copy_constructible_v<_Ty> && std::is_nothrow_destructible_v<_Ty>))
		{
			if (_Length + 1 <= _Capacity)
			{
				if (_PtrOffset() + _Length + 1 > _Capacity) {
					move_range(_Ptr, _Ptr + _Length, _PtrFirst);
					_Ptr = _PtrFirst;
				}
				new (&_Ptr[_Length++])_Ty(std::forward<_ArgTys>(_Args)...);
			}
			else
			{
				const _Ty* _oldPtr = _Ptr;
				const bool _oldEmpty = _Capacity == 0;
				const size_t _oldLength = _Length;
				Alloc(_Length + 1);
				Copy(_oldPtr, 0, _oldLength);
				new (&_Ptr[_oldLength])_Ty(std::forward<_ArgTys>(_Args)...);
				_Length = _oldLength + 1;
				if (_oldEmpty == false) {
					destruct_range(const_cast<_Ty*>(_oldPtr), const_cast<_Ty*>(_oldPtr + _oldLength));
					::operator delete[](reinterpret_cast<void*>(const_cast<_Ty*>(_oldPtr)));
				}
			}
		}

		void PopBack() _NOEXCEPT_COND(_NOEXCEPT_OPER(std::is_nothrow_destructible_v<_Ty>))
		{
			Erase(_Length - 1);
		}

		bool empty() const _NOEXCEPT
		{
			return length() == 0;
		}

		inline size_t _PtrOffset() const _NOEXCEPT
		{
			return _Ptr - _PtrFirst;
		}

		void Resize(size_t length) _NOEXCEPT
		{
			Alloc(length);
			_Length = length;
		}

	protected:
		_Ty* _PtrFirst;
		_Ty* _Ptr;
		size_t _Length;
		size_t _Capacity;
		size_t _CapacityIncreament;
	};


	template<class _Ty = void*>
	class Stack
	{
		typedef Stack<_Ty> _MyT;
		typedef DynamicArray<_Ty> Container;
	public:
		Stack()
		{
		}

		Stack(const _MyT& other) _NOEXCEPT_COND(_NOEXCEPT_OPER(std::is_nothrow_copy_constructible_v<_Ty>))
		{
			_Container = other._Container;
		}

		void Push(const _Ty& _val) _NOEXCEPT_COND(_NOEXCEPT_OPER(std::is_nothrow_copy_constructible_v<_Ty>))
		{
			_Container.PushBack(_val);
		}

		void Pop() _NOEXCEPT_COND(_NOEXCEPT_OPER(std::is_nothrow_destructible_v<_Ty>))
		{
			_Container.PopBack();
		}

		bool empty() const _NOEXCEPT
		{
			return _Container.empty();
		}

		_Ty& top() const _NOEXCEPT
		{
			return _Container.back();
		}

		_Ty& operator[](int offset) const
		{
			return (_Container[_Container.length() - 1 - offset]);
		}

		size_t size() const _NOEXCEPT
		{
			return _Container.length();
		}

		const Container& _Get_container() const _NOEXCEPT
		{	// get reference to container
			return (_Container);
		}

	protected:
		Container _Container;
	};

	template<class _Ty = void*>
	class Queue
	{
		typedef Queue<_Ty> _MyT;
		typedef DynamicArray<_Ty> Container;
	public:
		Queue()
		{
		}

		Queue(const _MyT& other) _NOEXCEPT_COND(_NOEXCEPT_OPER(std::is_nothrow_copy_constructible_v<_Ty>))
		{
			_Container = other._Container;
		}

		void PushBack(const _Ty& _val) _NOEXCEPT_COND(_NOEXCEPT_OPER(std::is_nothrow_copy_constructible_v<_Ty>))
		{
			_Container.PushBack(_val);
		}

		void PopFront() _NOEXCEPT_COND(_NOEXCEPT_OPER(std::is_nothrow_destructible_v<_Ty>))
		{
			_Container.Erase(static_cast<size_t>(0));
		}

		bool empty() const _NOEXCEPT
		{
			return _Container.empty();
		}

		_Ty& back() const _NOEXCEPT
		{
			return _Container.back();
		}

		_Ty& front() const _NOEXCEPT
		{
			return _Container.front();
		}

		size_t size() const _NOEXCEPT
		{
			return _Container.length();
		}

		const Container& _Get_container() const _NOEXCEPT
		{	// get reference to container
			return (_Container);
		}

		_Ty& operator[](int offset) const
		{
			return (_Container[offset]);
		}

	protected:
		Container _Container;
	};

	template<class _Ty = void*>
	class Deque : Queue<_Ty>
	{
	public:
		void PushFront(const _Ty& _val) _NOEXCEPT_COND(_NOEXCEPT_OPER(std::is_nothrow_copy_constructible_v<_Ty>))
		{
			_Container.Insert(_val, 0);
		}

		void PopBack()  _NOEXCEPT_COND(_NOEXCEPT_OPER(std::is_nothrow_destructible_v<_Ty>))
		{
			_Container.PopBack();
		}
	};

XKEINNAMESPACE_END