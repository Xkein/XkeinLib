#pragma once

#define XKEINNAMESPACE_START \
namespace xkein				 \
{
#define XKEINNAMESPACE_END	 \
}

XKEINNAMESPACE_START
#if!defined(_NOEXCEPT_COND) && !defined(_NOEXCEPT_OPER)
#if _HAS_EXCEPTIONS
#define _NOEXCEPT_COND(...)	noexcept(__VA_ARGS__)
#define _NOEXCEPT_OPER(...)	noexcept(__VA_ARGS__)
#else
#define _NOEXCEPT_COND(...)	throw ()
#define _NOEXCEPT_OPER(...)	true
#endif
#endif


#if !defined(_NOEXCEPT)
#if _HAS_EXCEPTIONS
#define _NOEXCEPT			noexcept
#else
#define _NOEXCEPT			throw ()
#endif
#endif


#define ABS(a) (((a) > 0) ? (a) : (b))

#define EMPTY

#define STATEMENTS(...) EMPTY

#define ABSTRACT EMPTY

#ifndef USING_SIMPLE_NAME_TYPENAME
#define USING_SIMPLE_NAME_TYPENAME _Ty
#endif

#ifndef UNREFERENCED_PARAMETER
#define UNREFERENCED_PARAMETER(P) (P)
#endif

#define USING_SIMPLE_NAME(name) \
	using name = xkein::name<USING_SIMPLE_NAME_TYPENAME>

	// evaluate the length of an array inexactly;
	template<class _Ty>
	size_t ptrlen(_Ty* ptr) _NOEXCEPT
	{
		for (size_t length = 0;; ++length) {
			BYTE* p = reinterpret_cast<BYTE*>(ptr + length);
			for (size_t idx = 0; idx < sizeof(_Ty); idx++) {
				switch (p[idx])
				{
				case NULL:
				case 0xCC:
				case 0xCD:
				case 0xFD:
				case 0xCF:
				case 0xFF:
					if (idx == sizeof(_Ty) - 1)
						return length;
					continue;
					//[[fallthrough]]//http://en.cppreference.com/w/cpp/language/attributes
				}
				break;
			}
		}
	}
	/*
	template<size_t idx, class _Ty>
	_Ty GetNthValueIsNotHelper(size_t n, _Ty _isnot) {
		return _isnot;
	}
	template<size_t idx, class _Ty, class _lTy, class... _lTys>
	_Ty GetNthValueIsNotHelper(size_t n, _Ty _isnot, _lTy head, _lTys... tails) {
		if (_isnot != head)
			if (++n == idx)
				return head;
		return GetNthValueIsNotHelper<idx>(n, _isnot, tails...);
	};

	STATEMENTS("http://en.cppreference.com/w/cpp/language/parameter_pack")
	template<size_t idx, class _Ty, class... _Tys>
	_Ty GetNthValueIsNot(_Ty _isnot, _Tys... _values)
	{
		return GetNthValueIsNotHelper<idx>(0, _isnot, _values...);
	}
	
	struct GetNthFunctionValueIsNotHelperSeparator {};

	template<size_t idx, class _Ty, class... _Tys>
	_Ty GetNthFunctionValueIsNotHelper(size_t n, _Ty _isnot, _Tys... _values, GetNthFunctionValueIsNotHelperSeparator) {
		return _isnot;
	}
	template<size_t idx, class _Ty, class... _Tys, class _Func,class... _Funcs>
	_Ty GetNthFunctionValueIsNotHelper(size_t n, _Ty _isnot, _Tys... _values, GetNthFunctionValueIsNotHelperSeparator, _Func head, _Funcs... tails) {
		_Ty ret = head(_values...);
		if (_isnot != ret)
			if (++n == idx)
				return ret;
		return GetNthFunctionValueIsNotHelper<idx>(n, _isnot, _values..., GetNthFunctionValueIsNotHelperSeparator(), tails...);
	};

	STATEMENTS("http://en.cppreference.com/w/cpp/language/parameter_pack")
		template<size_t idx, class _Ty, class... _Tys, class... _Funcs>
	_Ty GetNthFunctionValueIsNot(_Ty _isnot, _Tys... _values, GetNthFunctionValueIsNotHelperSeparator, _Funcs... _funcs)
	{
		return GetNthFunctionValueIsNotHelper<idx>(0, _isnot, _values..., GetNthFunctionValueIsNotHelperSeparator(), _funcs...);
	}

#define GETPROCADDRESS(lpModuleName, lpProcName, func) \
	func = (decltype(func))GetProcAddress(GetNthFunctionValueIsNot<1>(NULL, lpModuleName, GetNthFunctionValueIsNotHelperSeparator(), GetModuleHandle, LoadLibrary), lpProcName)
	*/

	/*
	template<class _Ty>
	class ConstexprReference
	{
		static_assert(std::is_object_v<_Ty> || std::is_function_v<_Ty>,
			"xkein::Reference<T> requires T to be an object type "
			"or a function type.");

		using type = _Ty;

	public:
		constexpr ConstexprReference(_Ty& _Val) _NOEXCEPT
			: _Ptr(_STD addressof(_Val))
		{	// construct
		}

		constexpr operator _Ty&() const _NOEXCEPT
		{	// return reference
			return (*_Ptr);
		}

		constexpr _NODISCARD _Ty& get() const _NOEXCEPT
		{	// return reference
			return (*_Ptr);
		}

		template<class... _Types>
		auto operator()(_Types&&... _Args) const
			-> decltype(_STD invoke(get(), _STD forward<_Types>(_Args)...))
		{	// invoke object/function
			return (_STD invoke(get(), _STD forward<_Types>(_Args)...));
		}

		constexpr ConstexprReference(_Ty&&) = delete;

	private:
		_Ty* _Ptr;
	};
	*/

	/** convert an object to any type. to be faster, the object should be a reference
	 * for example:
	 ** int aa = 5;
	 ** // convert aa to int&, now i itself but not the value of i is equal to 5 
	 ** int& i = Convert<int&>(aa);
	 ** // it is ok. (int&)a is converted to int& and aa2 is equal to 5
	 ** int aa2 = Convert<int&,int&>(aa);
	 ** // error, *(0x5) is invalid
	 ** int aa3 = Convert<int&>(aa);
	 ** // it is ok. &a is eaual to int&(&) and aa4 is equal to 5
	 ** int aa4 = Convert<int&>(&aa);
	**/
	template<class _Ty1 = void*, class _Ty2>
	inline _Ty1 Convert(_Ty2 object) _NOEXCEPT
	{ // i have no way to make it into constexpr version, but it is equal to constexpr since optimizing
		/*
		if constexpr(std::is_reference_v<_Ty1> && !std::is_reference_v<_Ty2>)
			return static_cast<_Ty1>(**reinterpret_cast<std::remove_reference_t<_Ty1>**>(&object))
		else
			return static_cast<_Ty1>(*reinterpret_cast<std::remove_reference_t<_Ty1>*>(&object));
		*/
		union {
			std::add_pointer_t<std::remove_reference_t<_Ty2>> object;
			std::add_pointer_t<std::add_pointer_t<std::remove_reference_t<_Ty2>>> pObject;
			std::add_pointer_t<std::remove_reference_t<_Ty1>> result;
		} tmp { nullptr };
		tmp.object = &object;
		if constexpr(std::is_reference_v<_Ty1> && !std::is_reference_v<_Ty2>)
			tmp.object = *tmp.pObject;
		return static_cast<_Ty1>(*tmp.result);
		
		/*
		union ConvertHelper
		{
			ConstexprReference<std::remove_reference_t<_Ty2>> object;
			ConstexprReference<std::remove_reference_t<_Ty1>> result;
		} tmp{ object };
		ConvertHelper& tmp2{ tmp };
		if constexpr(std::is_reference_v<_Ty1> && !std::is_reference_v<_Ty2>)
			return static_cast<_Ty1>(**reinterpret_cast<std::remove_reference_t<_Ty1>**>(&tmp2.result.get()));
		return static_cast<_Ty1>(tmp2.result);
		*/
	}

	// return a NULL reference that couldn't be accessed.
	template<class _Ty>
	inline _Ty& GetNullReference() _NOEXCEPT
	{
		return Convert<_Ty&>(nullptr);
	}

	template<class _Ty>
	class Iterator
	{
		static_assert(!std::is_reference_v<_Ty>
			, "xkein::Iterator::invaild type, it could not be a reference type.");
	protected:
		_Ty* _Ptr;
	public:
		Iterator() _NOEXCEPT : _Ptr(nullptr)
		{ }

		Iterator(_Ty* _Ptr) _NOEXCEPT : _Ptr(_Ptr)
		{ }

		_Ty* operator->() const
		{
			return _Ptr;
		}

		_Ty& operator*() const
		{
			return (*_Ptr);
		}

		_Ty& operator[](int offset) const
		{
			return (_Ptr[offset]);
		}

		_Ty*& operator+=(int offset) _NOEXCEPT
		{
			return (_Ptr += offset);
		}

		_Ty*& operator-=(int offset) _NOEXCEPT
		{
			return (_Ptr -= offset);
		}

		_Ty* operator+(int offset) const _NOEXCEPT
		{
			return _Ptr + offset;
		}

		_Ty* operator-(int offset) const _NOEXCEPT
		{
			return _Ptr - offset;
		}

		_Ty* operator++() _NOEXCEPT
		{
			return ++_Ptr;
		}

		_Ty* operator++(int) _NOEXCEPT
		{
			return _Ptr++;
		}

		_Ty* operator--() _NOEXCEPT
		{
			return --_Ptr;
		}

		_Ty* operator--(int) _NOEXCEPT
		{
			return _Ptr--;
		}

		operator _Ty*() const _NOEXCEPT
		{
			return _Ptr;
		}

		_Ty*& operator=(_Ty* right) _NOEXCEPT
		{
			return (_Ptr = right);
		}
	};

	template<class... _Args>
	constexpr bool is_all_true(_Args... args) _NOEXCEPT { return (... && args); }

	template<class... _Args>
	constexpr bool is_have_true(_Args... args) _NOEXCEPT { return (... || args); }

	template<class... _Args>
	constexpr bool is_have_false(_Args... args) _NOEXCEPT { return (... || !args); }

	template<class... _Args>
	constexpr bool is_all_false(_Args... args) _NOEXCEPT { return !is_have_true(args...); }


	template<class... Types>
	struct TypesCounter {
		using type = void;
		enum { ret = 0 };
	};
	template<class head, class... tails>
	struct TypesCounter<head, tails...>
	{
		using type = head;
		typedef TypesCounter<tails...> _Tails;
		enum { ret = 1 + _Tails::ret };
	};

	template<class _Ty, size_t... idxes>
	void* DynamicGetImplAddress(_Ty& obj, size_t idx, std::index_sequence<idxes...>) _NOEXCEPT
	{
		void* ret = nullptr;
		std::initializer_list{ (idx == idxes && (ret = Convert<void*>(&std::get<idxes>(obj))), idxes)... };
		return ret;
	}

	template<class _Ret = void*, class _Ty>
	std::add_lvalue_reference_t<_Ret> DynamicGetTupleImple(_Ty& obj, size_t idx) _NOEXCEPT
	{
		return Convert<std::add_lvalue_reference_t<_Ret>>(DynamicGetImplAddress(obj, idx, std::make_index_sequence<std::tuple_size_v<_Ty>>()));
	}

	template<class _Ty>
	constexpr size_t GetTupleSize(_Ty& obj) _NOEXCEPT
	{
		return std::tuple_size_v<_Ty>;
	}

	/* not so good
	template<size_t>
	void* GetTupleItem(size_t index, std::tuple<> tuple, size_t cur = size_t(0))
	{
		throw std::out_of_range("xkein::GetTupleItem::out of range!");
	};
	template<size_t size, class head, class... tails>
	void* GetTupleItem(size_t index, std::tuple<head, tails...>& tuple, size_t cur = size_t(0)) {

		if (index == cur) {
			return Convert<void*>(&std::get<0>(tuple));
		}
		else if (size > index) {
			return GetTupleItem<size, tails...>(index, tuple._Get_rest(), cur + 1);
		}
		else {
			throw std::out_of_range("xkein::GetTupleItem::out of range!");
		}
	}
	*/


	char* itoa_(int val, char* buffer, int radix)
	{
		char tmp[100]{};
		char* iter = buffer;

		if (val < 0)
			*iter++ = '-';

		int _val = val;
		char* tmpIter = tmp;

		while (_val) {
			char mod = (char)(_val % radix);
			*tmpIter++ = mod >= 10 ? mod - 10 + 'a' : mod + '0';
			_val /= radix;
		}
		while (tmpIter > tmp)
		{
			*iter++ = *--tmpIter;
		}

		*iter = '\0';
		return buffer;
	}


	template<class _Ty>
	struct DimensionCounter {
		using ElementType = _Ty;
		enum { ret = 0 };
	};

	template<class _Ty, size_t dimensions>
	struct DimensionCounter<_Ty[dimensions]>
	{
		typedef DimensionCounter<_Ty> NextCounter;
		using ElementType = typename NextCounter::ElementType;
		enum { ret = 1 + NextCounter::ret };
	};

	template<class _Ty>
	struct DimensionCounter<_Ty[]>
	{
		typedef DimensionCounter<_Ty> NextCounter;
		using ElementType = typename NextCounter::ElementType;
		enum { ret = 1 + NextCounter::ret };
	};

	template<class _Ty>
	struct DimensionCounter<_Ty*>
	{
		typedef DimensionCounter<_Ty> NextCounter;
		using ElementType = typename NextCounter::ElementType;
		enum { ret = 1 + NextCounter::ret };
	};

	template<class _Ty>
	struct DimensionCounter<_Ty* const>
	{
		typedef DimensionCounter<_Ty> NextCounter;
		using ElementType = typename NextCounter::ElementType;
		enum { ret = 1 + NextCounter::ret };
	};

	template<class _Ty>
	struct DimensionCounter<_Ty* volatile>
	{
		typedef DimensionCounter<_Ty> NextCounter;
		using ElementType = typename NextCounter::ElementType;
		enum { ret = 1 + NextCounter::ret };
	};

	template<class _Ty>
	struct DimensionCounter<_Ty* const volatile>
	{
		typedef DimensionCounter<_Ty> NextCounter;
		using ElementType = typename NextCounter::ElementType;
		enum { ret = 1 + NextCounter::ret };
	};

	template<class _Ty>
	constexpr size_t GetDimensions(_Ty element)
	{
		return 0;
	}

	template<class _Ty, size_t size>
	constexpr size_t GetDimensions(_Ty(&arr)[size])
	{
		return DimensionCounter<_Ty[size]>::ret;
	}

	template<class _Ty>
	constexpr size_t GetDimensions(_Ty(&arr)[])
	{
		return DimensionCounter<_Ty[]>::ret;
	}

	template<class _Ty>
	struct ArrayInformation {
		using ElementType = _Ty;
	};


#define ArrayInfo ArrayInformation<_Ty>
	template<class _Ty>
	constexpr std::pair<size_t, size_t> GetArrayDimensionInfo(size_t dimension, size_t cur = size_t(0))
	{
		if (dimension == cur) {
			return { ArrayInfo::elementSize,ArrayInfo::size };
		}
		else {
			if constexpr (std::is_array_v<typename ArrayInfo::NextType>)
				return GetArrayDimensionInfo<typename ArrayInfo::NextType>(dimension, cur + 1);
			else return { 0,0 };
		}
	}

	template<class _Ty>
	constexpr typename DimensionCounter<_Ty>::ElementType& GetArrayElement(_Ty& arr, size_t index)
	{
		if constexpr (std::is_pointer_v<_Ty>) {
			const size_t nextTypeLength = sizeof(std::remove_pointer_t<_Ty>) / sizeof(typename DimensionCounter<_Ty>::ElementType);
			if constexpr (std::is_array_v<std::remove_pointer_t<_Ty>>)
				return GetArrayElement(arr[index / nextTypeLength], index % nextTypeLength);
			else return arr[index];
		}
		else {
			auto arrInfo = GetArrayInformation<_Ty>();
			const size_t nextTypeLength = arrInfo.ElementLength[0] / sizeof(typename ArrayInfo::ElementType);
			if constexpr (std::is_array_v<typename ArrayInfo::NextType>)
				return GetArrayElement(arr[index / nextTypeLength], index % nextTypeLength);
			else return arr[index];
		}
	}
	/*
	template<class ObjType, class _Ty>
	constexpr ObjType& GetArrayFromDimension(_Ty arr)
	{
		std::array<typename ArrayInformation<ObjType>::NextType, ArrayInformation<ObjType>::size> tmp{}£»
		if constexpr (std::is_same_v<DimensionCounter<_Ty>::ret, DimensionCounter<ObjType>::ret>)
			return GetArrayFromDimension<ObjType>(arr);
		else return &arr;
	}
	*/

	template<class _Ty, size_t _Size>
	struct ArrayInformation<_Ty[_Size]>
	{
		using NextType = _Ty;
		static constexpr size_t size = _Size;
		using DimensionCounter = DimensionCounter<_Ty[size]>;
		using ElementType = typename DimensionCounter::ElementType;
		using ArrayType = _Ty[size];
		static constexpr size_t elementSize = sizeof(_Ty);
		static constexpr size_t length = elementSize * size;
		static constexpr size_t dimensions = DimensionCounter::ret;
		ElementType* _ElementPtr;
		NextType* _NextTypePtr;
		std::array<size_t, dimensions> ElementLength;
		std::array<size_t, dimensions> ElementCount;

		constexpr ElementType& GetElement(size_t index) const
		{
			return GetArrayElement(_NextTypePtr, index);
		}
	};

	template<class ArrayInformation>
	constexpr auto GetArrayInformationBase(typename ArrayInformation::ElementType* pElem = nullptr, typename ArrayInformation::NextType* pNextType = nullptr)
	{
		ArrayInformation tmp{};
		tmp._NextTypePtr = pNextType;
		tmp._ElementPtr = pElem;
		for (size_t idx = 0; idx < tmp.dimensions; ++idx) {
			tmp.ElementLength[idx] = GetArrayDimensionInfo<ArrayInformation::ArrayType>(idx).first;
			tmp.ElementCount[idx] = GetArrayDimensionInfo<ArrayInformation::ArrayType>(idx).second;
		}
		return tmp;
	}

	template<class _Ty>
	constexpr auto GetArrayInformation(typename ArrayInfo::ElementType* ptr = nullptr)
	{
		return GetArrayInformationBase<ArrayInfo>(ptr);
		/*
		using ArrayType = typename ArrayInfo::ArrayType;
		ArrayInformation<ArrayType> tmp{};
		tmp._ElementPtr = ptr;
		//tmp._NextTypePtr = reinterpret_cast<std::add_const_t<typename ArrayInfo::NextType>*>(ptr);
		for (size_t idx = 0; idx < tmp.dimensions; ++idx) {
			tmp.ElementLength[idx] = GetArrayDimensionInfo<ArrayType>(idx).first;
			tmp.ElementCount[idx] = GetArrayDimensionInfo<ArrayType>(idx).second;
		}
		return tmp;
		*/
	}

	template<class _Ty, size_t size>
	constexpr auto GetArrayInformation(_Ty(&arr)[size])
	{
		return GetArrayInformationBase<ArrayInformation<_Ty[size]>>(&GetArrayElement(arr, 0), arr);
		/*
		using ArrayType = _Ty[size];
		ArrayInformation<ArrayType> tmp{};
		tmp._NextTypePtr = arr;
		//tmp._ElementPtr = reinterpret_cast<std::add_const_t<typename ArrayInfo::ElementType>*>(arr);
		for (size_t idx = 0; idx < tmp.dimensions; ++idx) {
			tmp.ElementLength[idx] = GetArrayDimensionInfo<ArrayType>(idx).first;
			tmp.ElementCount[idx] = GetArrayDimensionInfo<ArrayType>(idx).second;
		}
		return tmp;
		*/
	}

#undef ArrayInfo



XKEINNAMESPACE_END