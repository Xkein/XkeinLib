#pragma once

#include <type_traits>
#include "tools.h"

XKEINNAMESPACE_START
	template<class _tPtr, bool _auto_alloc = false>
	class SmartPtr
	{
		static_assert(std::is_pointer_v<_tPtr>
			, "xkein::SmartPtr::invaild type, it isn't a pointer type.");

		//typedef typename std::remove_reference<decltype(*((_tPtr)nullptr))>::type _Type;
		typedef typename std::remove_pointer_t<_tPtr> _Type;
		typedef SmartPtr<_tPtr, _auto_alloc> _MyT;
		typedef Iterator<_Type> iterator;

		static constexpr int _length_of_ptr = -1;

		static int arrayIncrement;

	public:
		SmartPtr(_MyT& other) _NOEXCEPT : _IAllocated(false), _HasNext(false), next(nullptr)
		{
			_Ptr = other._Ptr;
			_Length = other._Length;
			_Ptr_offset = other._Ptr_offset;
		}

		SmartPtr(_tPtr& value) _NOEXCEPT : _IAllocated(false), _HasNext(false), _Length(_length_of_ptr), _Ptr_offset(0), next(nullptr)
		{
			_Ptr = std::move(value);
		}

		// warning! if the value is "new _Type", you should delete it by yourself.
		// recommend that use "Smart<_tPtr, true> ptr;" or "Smart<_tPtr> ptr(...);" to instead of such way
		SmartPtr(_tPtr&& value) _NOEXCEPT : _IAllocated(false), _HasNext(false), _Length(_length_of_ptr), _Ptr_offset(0), next(nullptr)
		{
			_Ptr = value;
		}

		template<int size>
		SmartPtr(_Type(&value)[size]) _NOEXCEPT : _IAllocated(false), _HasNext(false), _Ptr_offset(0), next(nullptr)
		{
			_Ptr = value;
			_Length = size;
		}

		SmartPtr() _NOEXCEPT_COND(_NOEXCEPT_OPER(!_auto_alloc || std::is_nothrow_default_constructible_v<_Type>))
			: _Length(_length_of_ptr), _Ptr_offset(0), _HasNext(false), next(nullptr)
		{
			_Ptr = _auto_alloc ? new _Type : nullptr;
			_IAllocated = _auto_alloc;
		}

		template<class... _Types>
		SmartPtr(_Types&&... _Args) _NOEXCEPT_COND(_NOEXCEPT_OPER((std::is_nothrow_constructible_v<_Type
			, typename std::add_lvalue_reference<_Types>::type...>)))
			: _Length(_length_of_ptr), _Ptr_offset(0), _IAllocated(true), _HasNext(false), next(nullptr)
		{
			_Ptr = new _Type(std::forward<_Types>(_Args)...);
		}

		virtual ~SmartPtr() _NOEXCEPT_COND(_NOEXCEPT_OPER(std::is_nothrow_destructible_v<_Type>))
		{
			Clear();
		}

		// remember to set *this = nullptr before the _Ptr is lost
		// for example, (*this = nullptr) = right
		_tPtr& operator=(_tPtr right) _NOEXCEPT_COND(_NOEXCEPT_OPER(std::is_nothrow_destructible_v<_Type>))
		{
			RollBack();

			if (_IAllocated) {
				if (right == nullptr) {
					IsArray() ? delete[] _Ptr : delete _Ptr;
				}
				_IAllocated = false;
			}

			_Length = _length_of_ptr;
			_Ptr = right;

			return (_Ptr);
		}

		_Type& operator*() const
		{
			return (*_Ptr);
		}

		_tPtr operator->() const _NOEXCEPT
		{
			return *this;
		}

		_Type& operator[](int offset) const
		{
			return (_Ptr[offset]);
		}

		_tPtr operator+(SmartPtr& right) const _NOEXCEPT
		{
			return _Ptr + right;
		}

		_tPtr operator-(SmartPtr& right) const _NOEXCEPT
		{
			return _Ptr - right;
		}

		_tPtr& operator+=(int offset) _NOEXCEPT
		{
			_Ptr_offset += offset;
			return (_Ptr += offset);
		}

		_tPtr& operator-=(int offset) _NOEXCEPT
		{
			_Ptr_offset -= offset;
			return (_Ptr -= offset);
		}

		_tPtr operator+(int offset) const _NOEXCEPT
		{
			return _Ptr + offset;
		}

		_tPtr operator-(int offset) const _NOEXCEPT
		{
			return _Ptr - offset;
		}

		_tPtr operator++() _NOEXCEPT
		{
			_Ptr_offset++;
			return ++_Ptr;
		}

		_tPtr operator++(int) _NOEXCEPT
		{
			_Ptr_offset++;
			return _Ptr++;
		}

		_tPtr operator--() _NOEXCEPT
		{
			_Ptr_offset--;
			return --_Ptr;
		}

		_tPtr operator--(int) _NOEXCEPT
		{
			_Ptr_offset--;
			return _Ptr--;
		}

		operator _tPtr() const _NOEXCEPT
		{
			return _Ptr;
		}

		const _tPtr get() const _NOEXCEPT
		{
			return *this;
		}

		int GetArrayLength() const
		{
			if (IsArray() && (_Ptr_offset < 0 || _Ptr_offset >= _Length)) {
				_MyT* pThis = const_cast<_MyT*>(this);
				pThis->RollBack();
				throw std::out_of_range("xkein::SmartPtr::out of range!");
			}
			return IsArray() ? _Length - _Ptr_offset : _length_of_ptr;
		}

		bool IsArray() const _NOEXCEPT
		{
			return _Length != _length_of_ptr;
		}

		// set array length if it isn't an array, or throw error
		void SetArrayLength(int length)
		{
			if (IsArray() == false) {
				_Length = length;
			}
			else {
				throw std::logic_error("xkein::SmartPtr::invalid operation");
			}
		}

		void SetArray(_tPtr arr, int length) _NOEXCEPT
		{
			this->operator=(arr);
			SetArrayLength(length);
		}

		// alloc with default constructing or without
		_tPtr& AllocArray(int length) _NOEXCEPT_COND(_NOEXCEPT_OPER(std::is_nothrow_default_constructible_v<_Type>))
		{
			_tPtr _Tmp = nullptr;
			if (length < 0) {
				_Tmp = reinterpret_cast<_tPtr>(::operator new[](sizeof(_Type) * -length));
				SetArray(_Tmp, -length);
			}
			else {
				_Tmp = new _Type[length];
				SetArray(_Tmp, length);
			}
			_IAllocated = true;
			return (_Ptr);
		}

		// alloc with explicit constructing
		template<class... _Types>
		_tPtr& AllocArray(int length, _Types&&... _Args) _NOEXCEPT_COND(_NOEXCEPT_OPER(
			(std::is_nothrow_constructible_v<_Type, typename std::add_lvalue_reference<_Types>::type...>)))
		{
			_tPtr _Tmp = AllocArray(-length);
			for (int idx = 0; idx < length; idx++) {
				new (&_Tmp[idx])_Type(std::forward<_Types>(_Args)...);
			}
			return (_Ptr);
		}

		iterator begin() const _NOEXCEPT
		{
			return (_Ptr);
		}

		iterator end() const
		{
			iterator _Tmp = begin();
			const int length = GetArrayLength();
			if (0 < length) {
				_Tmp += length;
			}
			else if (length == _length_of_ptr) {
				_Tmp++;
			}
			return (_Tmp);
		}

		void RollBack() _NOEXCEPT
		{ // roll _Ptr back
			_Ptr -= _Ptr_offset;
			_Ptr_offset = 0;
		}

		_tPtr& ClearAndSet(_tPtr value) _NOEXCEPT
		{
			return (Clear() = value);
		}

		_tPtr& Clear() _NOEXCEPT_COND(_NOEXCEPT_OPER(std::is_nothrow_destructible_v<_Type>))
		{
			return (this->operator=(nullptr));
		}

		_MyT* Link(_MyT* object) _NOEXCEPT
		{
			return _HasNext = object != nullptr, next = object;
		}

		template<class _Ty>
		_Ty& Link(_Ty object) _NOEXCEPT
		{
			return (_HasNext = false, linked = object);
		}

		template<class _Fn>
		_Fn foreach(_Fn _Func) _NOEXCEPT_COND(_NOEXCEPT_OPER(_Func))
		{
			_MyT* pNext = this;
			do {
				for (_Type& ref : *pNext) {
					if (!_Func(ref))
						break;
				}
			} while (pNext = pNext->Next());
			return _Func;
		}

		template<class _Fn>
		_Fn foreach(_Fn _Func) const _NOEXCEPT_COND(_NOEXCEPT_OPER(_Func))
		{
			const _MyT* pNext = this;
			do {
				for (const _Type& ref : *pNext) {
					if (!_Func(ref))
						break;
				}
			} while (pNext = pNext->Next());
			return _Func;
		}

		_MyT* Next() const _NOEXCEPT
		{
			return _HasNext ? next : nullptr;
		}

		template<class _Ty = void*>
		_Ty Linked() const _NOEXCEPT
		{
			static_assert(std::is_reference_v<_Ty>
				, "xkein::SmartPtr::invaild type, it couldn't be a reference.");
			return _HasNext ? nullptr : static_cast<_Ty>(linked);
		}


#pragma warning(push)
#pragma warning(disable: 4702)
		_Type& at(int index) const
		{
			const _MyT* pNext = this;
			do {
				int length = pNext->GetArrayLength();
				if (length == _length_of_ptr) {
					length = 1;
				}
				if (index < length) {
					return (pNext->_Ptr[index]);
				}
				index -= length;
			} while ((pNext = pNext->Next()) != nullptr);

			return throw std::out_of_range("xkein::SmartPtr::out of range!"), GetNullReference<_Type>();
		}
#pragma warning(pop)

		int Find(const _tPtr ptr) const _NOEXCEPT
		{
			int index = 0;
			foreach([&](const _Type& ref) _NOEXCEPT{ return index <= 0 && ptr != &ref ? (--index, true) : (index = -index, false); });
			return index;
		}

		int Find(const _Type& ref) const _NOEXCEPT
		{
			int index = 0;
			foreach([&](const _Type& _ref) _NOEXCEPT{ return index <= 0 && _ref != ref ? (--index, true) : (index = -index, false); });
			return index;
		}

		template<int size>
		_tPtr& operator=(_Type(&value)[size]) _NOEXCEPT
		{
			SetArray(value, size);
			return (_Ptr);
		}

		void Copy(const _tPtr ptr, int length = _length_of_ptr) _NOEXCEPT_COND(_NOEXCEPT_OPER((std::is_nothrow_constructible_v<_Type
			, typename std::add_lvalue_reference<_Type>::type>)))
		{
			if (length == _length_of_ptr) {
				length = ptrlen(ptr);
			}

			if (GetArrayLength() < length) {
				AllocArray(-length);
			}
			for (int idx = 0; idx < length; idx++) {
				new (&_Ptr[idx])_Type(ptr[idx]);
			}
		}

		template<int size>
		void Copy(const _Type(&ptr)[size], int length = size) _NOEXCEPT_COND(_NOEXCEPT_OPER((std::is_nothrow_constructible_v<_Type
			, typename std::add_lvalue_reference<_Type>::type>)))
		{
			Copy((const _tPtr)ptr, length);
		}

		// SmartPtr services for ptr, not vector
		// so i will support it no more
		// ---------------------------------------

		int backIndex() const
		{
			return ptrlen(_Ptr) - 1;
		}

		_Type& back() const
		{
			return (_Ptr[backIndex()]);
		}

		void push_back(const _Type& value)
		{
			int oldLength = GetArrayLength();
			int backIndex = this->backIndex();
			if (oldLength <= backIndex + 1) {
				_tPtr tmp = _Ptr;
				int allocLength = oldLength + arrayIncrement;
				if (allocLength > USN_PAGE_SIZE && allocLength < USN_PAGE_SIZE * 2) {
					allocLength = USN_PAGE_SIZE * 2;
					arrayIncrement = USN_PAGE_SIZE;
				}
				AllocArray(-(allocLength));
				if (arrayIncrement < USN_PAGE_SIZE)
					arrayIncrement <<= 1;
				Copy(tmp, oldLength);
				new (&_Ptr[backIndex + 1])_Type(value);
				oldLength != _length_of_ptr ? delete[] tmp : delete tmp;
			}
			else {
				new (&_Ptr[backIndex + 1])_Type(value);
			}
		}
		// ---------------------------------------

	protected:
		bool _IAllocated;
		bool _HasNext;
		_tPtr _Ptr;
		int _Length;
		int _Ptr_offset;
		union {
			_MyT* next;
			void* linked;
		};
	};

	template<class _tPtr, bool _auto_alloc>
	int SmartPtr<_tPtr, _auto_alloc>::arrayIncrement = 0x10;

	// for normal situation, origin function ptr is recommended.
#define _GET_SMARTFUNCTIONPTR_(CALL_OPT, XCV_OPT, XREF_OPT, NOEXCEPT_OPT) \
	template<class _Ret, bool _release, class... _Types> \
	class SmartPtr<_Ret(CALL_OPT*)(_Types...) NOEXCEPT_OPT, _release> { \
		typedef _Ret(CALL_OPT*_tPtr)(_Types...) NOEXCEPT_OPT; \
		typedef SmartPtr<_tPtr, _release> _MyT; \
		typedef typename std::remove_pointer<_tPtr>::type _Type; \
	protected: \
		virtual void _Release() _NOEXCEPT { if constexpr(_release){ delete[] Convert<BYTE*>(_Ptr); } } \
	public: \
		typedef FunctionInformation<_tPtr> func; \
		SmartPtr(_MyT& other) _NOEXCEPT { _Ptr = other._Ptr; } \
		SmartPtr(_tPtr& value) _NOEXCEPT { _Ptr = std::move(value); } \
		SmartPtr() _NOEXCEPT : _Ptr(nullptr) { } \
		SmartPtr(_Type value) _NOEXCEPT { _Ptr = value; } \
		virtual ~SmartPtr() _NOEXCEPT {  this->operator=(nullptr); } \
		virtual _tPtr& operator=(_tPtr right) _NOEXCEPT { return (_Ptr = (right ? right : (_Release(), right))); } \
		virtual const _tPtr* operator&() const _NOEXCEPT { return &_Ptr; } \
		_MyT* operator&() _NOEXCEPT { return this; } \
		virtual _Type& operator*() const { return *_Ptr; } \
		virtual operator _tPtr() const _NOEXCEPT { return _Ptr; } \
		virtual const _tPtr get() const _NOEXCEPT { return *this; } \
		virtual _Ret operator()(_Types&&... _Args) const NOEXCEPT_OPT STATEMENTS(_NOEXCEPT_COND(_NOEXCEPT_OPER(_Ptr(std::forward<_Types>(_Args)...)))) \
			{ return _Ptr(std::forward<_Types>(_Args)...); } \
	protected: \
		_tPtr _Ptr; \
	};

#define _GET_SMARTCLASSFUNCTIONPTR_(CALL_OPT, CV_OPT, REF_OPT, NOEXCEPT_OPT) \
	template<class _Ret, class _Class, bool _release, class... _Types> \
	class SmartPtr<_Ret(CALL_OPT _Class::*)(_Types...) CV_OPT REF_OPT NOEXCEPT_OPT, _release> { \
		typedef _Ret(CALL_OPT _Class::*_tPtr)(_Types...) CV_OPT REF_OPT NOEXCEPT_OPT; \
		typedef SmartPtr<_tPtr, _release> _MyT; \
		typedef typename std::remove_pointer<_tPtr>::type _Type; \
	protected: \
		virtual void _Release() _NOEXCEPT { if constexpr(_release){ delete[] Convert<BYTE*>(_Ptr); } } \
	public: \
		typedef FunctionInformation<_tPtr> func; \
		SmartPtr(_MyT& other) _NOEXCEPT : _Ptr( other._Ptr) { } \
		SmartPtr(_tPtr& value) _NOEXCEPT : _Ptr(value) { } \
		SmartPtr() _NOEXCEPT : _Ptr(nullptr) { } \
		SmartPtr(_Type value) _NOEXCEPT : _Ptr(value) { } \
		virtual ~SmartPtr() _NOEXCEPT {  this->operator=(nullptr); } \
		virtual _tPtr& operator=(_tPtr right) _NOEXCEPT { return (_Ptr = (right ? right : (_Release(), right))); } \
		virtual const _tPtr* operator&() const _NOEXCEPT { return &_Ptr; } \
		_MyT* operator&() _NOEXCEPT { return this; } \
		virtual operator _tPtr() const _NOEXCEPT { return _Ptr; } \
		virtual const _tPtr get() const _NOEXCEPT { return *this; } \
		virtual _Ret operator()(_Class* pThis, _Types&&... _Args) const NOEXCEPT_OPT \
			{ return (((func::invoke_class_reference)*pThis).*_Ptr)(std::forward<_Types>(_Args)...); } \
	protected: \
		_tPtr _Ptr; \
	};

#pragma warning(push)
#pragma warning(disable: 4003)
#pragma warning(disable: 4002)
	_MEMBER_CALL(_GET_SMARTFUNCTIONPTR_, , ,)
	_MEMBER_CALL(_GET_SMARTFUNCTIONPTR_, , , _NOEXCEPT)
	_MEMBER_CALL_CV_REF_NOEXCEPT(_GET_SMARTCLASSFUNCTIONPTR_)
#pragma warning(pop)
#undef _GET_SMARTFUNCTIONPTR_

		template<class _Ty1,
		class _Ty2>
		bool operator==(const SmartPtr<_Ty1>& _Left, const SmartPtr<_Ty2>& _Right) _NOEXCEPT
	{	// test if SmartPtr == SmartPtr
		return (_Left.get() == _Right.get());
	}

	template<class _Ty1,
		class _Ty2>
		bool operator!=(const SmartPtr<_Ty1>& _Left, const SmartPtr<_Ty2>& _Right) _NOEXCEPT
	{	// test if SmartPtr != SmartPtr
		return (!(_Left == _Right));
	}

	template<class _Ty1,
		class _Ty2>
		bool operator<(const SmartPtr<_Ty1>& _Left, const SmartPtr<_Ty2>& _Right) _NOEXCEPT
	{	// test if SmartPtr < SmartPtr
		return (std::less<decltype(std::_Always_false<_Ty1>::value
			? _Left.get() : _Right.get())>()(
				_Left.get(), _Right.get()));
	}

	template<class _Ty1,
		class _Ty2>
		bool operator>=(const SmartPtr<_Ty1>& _Left, const SmartPtr<_Ty2>& _Right) _NOEXCEPT
	{	// SmartPtr >= SmartPtr
		return (!(_Left < _Right));
	}

	template<class _Ty1,
		class _Ty2>
		bool operator>(const SmartPtr<_Ty1>& _Left, const SmartPtr<_Ty2>& _Right) _NOEXCEPT
	{	// test if SmartPtr > SmartPtr
		return (_Right < _Left);
	}

	template<class _Ty1,
		class _Ty2>
		bool operator<=(const SmartPtr<_Ty1>& _Left, const SmartPtr<_Ty2>& _Right) _NOEXCEPT
	{	// test if SmartPtr <= SmartPtr
		return (!(_Right < _Left));
	}

	template<class _Ty>
	bool operator==(const SmartPtr<_Ty>& _Left, nullptr_t) _NOEXCEPT
	{	// test if SmartPtr == nullptr
		return (_Left.get() == nullptr_t{});
	}

	template<class _Ty>
	bool operator==(nullptr_t, const SmartPtr<_Ty>& _Right) _NOEXCEPT
	{	// test if nullptr == SmartPtr
		return (nullptr_t{} == _Right.get());
	}

	template<class _Ty>
	bool operator!=(const SmartPtr<_Ty>& _Left, nullptr_t) _NOEXCEPT
	{	// test if SmartPtr != nullptr
		return (!(_Left == nullptr_t{}));
	}

	template<class _Ty>
	bool operator!=(nullptr_t, const SmartPtr<_Ty>& _Right) _NOEXCEPT
	{	// test if nullptr != SmartPtr
		return (!(nullptr_t{} == _Right));
	}

	template<class _Ty>
	bool operator<(const SmartPtr<_Ty>& _Left, nullptr_t) _NOEXCEPT
	{	// test if SmartPtr < nullptr
		return (std::less<_Ty *>()(_Left.get(), nullptr_t{}));
	}

	template<class _Ty>
	bool operator<(nullptr_t, const SmartPtr<_Ty>& _Right) _NOEXCEPT
	{	// test if nullptr < SmartPtr
		return (std::less<_Ty *>()(nullptr_t{}, _Right.get()));
	}

	template<class _Ty>
	bool operator>=(const SmartPtr<_Ty>& _Left, nullptr_t) _NOEXCEPT
	{	// test if SmartPtr >= nullptr
		return (!(_Left < nullptr_t{}));
	}

	template<class _Ty>
	bool operator>=(nullptr_t, const SmartPtr<_Ty>& _Right) _NOEXCEPT
	{	// test if nullptr >= SmartPtr
		return (!(nullptr_t{} < _Right));
	}

	template<class _Ty>
	bool operator>(const SmartPtr<_Ty>& _Left, nullptr_t) _NOEXCEPT
	{	// test if SmartPtr > nullptr
		return (nullptr_t{} < _Left);
	}

	template<class _Ty>
	bool operator>(nullptr_t, const SmartPtr<_Ty>& _Right) _NOEXCEPT
	{	// test if nullptr > SmartPtr
		return (_Right < nullptr_t{});
	}

	template<class _Ty>
	bool operator<=(const SmartPtr<_Ty>& _Left, nullptr_t) _NOEXCEPT
	{	// test if SmartPtr <= nullptr
		return (!(nullptr_t{} < _Left));
	}

	template<class _Ty>
	bool operator<=(nullptr_t, const SmartPtr<_Ty>& _Right) _NOEXCEPT
	{	// test if nullptr <= SmartPtr
		return (!(_Right < nullptr_t{}));
	}


XKEINNAMESPACE_END
