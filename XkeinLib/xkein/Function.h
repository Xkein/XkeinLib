#pragma once

#include <functional>
#include "DataStruct.h"
#include "tools.h"

XKEINNAMESPACE_START

	template<class _Func>
	struct FunctionInformation {
		static constexpr bool is_function = false;
	};

#ifdef _M_IX86
	constexpr size_t _pushsize = 4;
#else
	constexpr size_t _pushsize = 8;
#endif

	constexpr size_t GetArgPushSize(size_t size) {
		return size != 0 ? std::max(size, _pushsize) : 0;
	}

	template<class _Ty>
	constexpr size_t GetArgSize() {
		return std::is_reference_v<_Ty> ? _pushsize : sizeof(_Ty);
	}

	template<class... Types>
	struct ArgLengthHelper {
		using type = void;
		enum { ret = 0 };
	};
	template<class head, class... tails>
	struct ArgLengthHelper<head, tails...>
	{
		using type = head;
		typedef ArgLengthHelper<tails...> _Tails;
		enum { ret = GetArgPushSize(GetArgSize<head>()) + _Tails::ret };
	};
	/* not work good
	template<size_t index, size_t cur, class... Types>
	struct ArgSizeHelper {
	enum { ret = 0 };
	};
	template<size_t index, size_t cur, class head, class... tails>
	struct ArgSizeHelper<index, cur, head, tails...> {
	enum { ret = index == cur ? sizeof(head) : ArgSizeHelper<index, cur + 1, tails...>::ret };
	};
	template<size_t size, class... Types>
	constexpr size_t GetArgSize(size_t index, size_t cur = 0) {
	static_assert(size > index, "xkein::GetArgSize::out of range!");
	return ArgSizeHelper<index, cur, Types...>::ret;
	}
	*/
	template<size_t>
	constexpr size_t GetArgSize(size_t index, size_t cur = size_t(0)) {
		UNREFERENCED_PARAMETER(index);
		UNREFERENCED_PARAMETER(cur);
		return size_t(0);
	}
	template<size_t size, class head, class... tails>
	constexpr size_t GetArgSize(size_t index, size_t cur = size_t(0)) {
		//static_assert(size > index, "xkein::GetArgSize::out of range!");

		if (index == cur) {
			return GetArgSize<head>();
		}
		else if (size > index) {
			return GetArgSize<size, tails...>(index, cur + 1);
		}
		else {
			throw std::out_of_range("xkein::GetArgSize::out of range!");
			return size_t(-1);
		}
	}

	template<size_t size, class... Types>
	constexpr auto GetEachArgSize()
	{
		std::array<size_t, size> tmp{};
		for (size_t i = 0; i < size; ++i) {
			tmp[i] = GetArgPushSize(GetArgSize<size, Types...>(i));
		}
		return tmp;
	}


#define _GET_FUNCTION_INFORMATION_(CALL_OPT, XCV_OPT, XREF_OPT, NOEXCEPT_OPT) \
	template<class _Ret, class... _Args> \
	struct FunctionInformation<_Ret(CALL_OPT*)(_Args...) NOEXCEPT_OPT> { \
		typedef _Ret(CALL_OPT* type__me)(_Args...) NOEXCEPT_OPT; \
		typedef _Ret(__stdcall* type__stdcall)(_Args...) NOEXCEPT_OPT; \
		typedef _Ret(__cdecl* type__cdecl)(_Args...) NOEXCEPT_OPT; \
		typedef _Ret(__fastcall* type__fastcall)(_Args...) NOEXCEPT_OPT; \
		typedef _Ret(__thiscall* type__thiscall)(_Args...) NOEXCEPT_OPT; \
		typedef _Ret(__vectorcall* type__vectorcall)(_Args...) NOEXCEPT_OPT; \
		typedef _Ret ret; \
		typedef ArgLengthHelper<_Args...> ArgLengthHelper; \
		typedef typename std::add_pointer<type__me>::type p_func; \
		static constexpr size_t ArgCount = sizeof...(_Args); \
		static constexpr size_t ArgLength = ArgLengthHelper::ret; \
		static constexpr size_t ArgPushTimes = ArgLength / _pushsize; \
		static constexpr auto ArgSizes = GetEachArgSize<std::max(ArgCount, 1u), _Args...>(); \
		static constexpr bool is_function = true; \
		static constexpr bool is__cdecl = std::is_same_v<type__me, type__cdecl>; \
		static constexpr bool is__stdcall = std::is_same_v<type__me, type__stdcall>; \
		static constexpr bool is__fastcall = std::is_same_v<type__me, type__fastcall>; \
		static constexpr bool is__thiscall = std::is_same_v<type__me, type__thiscall>; \
		static constexpr bool is__vectorcall = std::is_same_v<type__me, type__vectorcall>; \
		static constexpr bool is__noexcept = std::is_nothrow_invocable_v<type__me>; \
		constexpr FunctionInformation() _NOEXCEPT { /* construct empty FunctionInformation */ } \
	};

#define _GET_CLASS_FUNCTION_INFORMATION_(CALL_OPT, CV_OPT, REF_OPT, NOEXCEPT_OPT) \
	template<class _Ret, class _Class, class... _Args> \
	struct FunctionInformation<_Ret(CALL_OPT _Class::*)(_Args...) CV_OPT REF_OPT NOEXCEPT_OPT> { \
		typedef _Ret(CALL_OPT _Class::* type__me)(_Args...) CV_OPT REF_OPT NOEXCEPT_OPT; \
		typedef _Ret(__stdcall _Class::* type__stdcall)(_Args...) CV_OPT REF_OPT NOEXCEPT_OPT; \
		typedef _Ret(__cdecl _Class::* type__cdecl)(_Args...) CV_OPT REF_OPT NOEXCEPT_OPT; \
		typedef _Ret(__fastcall _Class::* type__fastcall)(_Args...) CV_OPT REF_OPT NOEXCEPT_OPT; \
		typedef _Ret(__thiscall _Class::* type__thiscall)(_Args...) CV_OPT REF_OPT NOEXCEPT_OPT; \
		typedef _Ret(__vectorcall _Class::* type__vectorcall)(_Args...) CV_OPT REF_OPT NOEXCEPT_OPT; \
		typedef std::conditional_t<std::is_same_v<int REF_OPT, int&&>, _Class&&, _Class&> invoke_class_reference; \
		typedef _Ret ret; \
		typedef ArgLengthHelper<_Args...> ArgLengthHelper; \
		typedef typename std::add_pointer<type__me>::type p_func; \
		static constexpr size_t ArgCount = sizeof...(_Args); \
		static constexpr size_t ArgLength = ArgLengthHelper::ret; \
		static constexpr size_t ArgPushTimes = ArgLength / _pushsize; \
		static constexpr auto ArgSizes = GetEachArgSize<std::max(ArgCount, 1u), _Args...>(); \
		static constexpr bool is_function = true; \
		static constexpr bool is__cdecl = std::is_same_v<type__me, type__cdecl>; \
		static constexpr bool is__stdcall = std::is_same_v<type__me, type__stdcall>; \
		static constexpr bool is__fastcall = std::is_same_v<type__me, type__fastcall>; \
		static constexpr bool is__thiscall = std::is_same_v<type__me, type__thiscall>; \
		static constexpr bool is__vectorcall = std::is_same_v<type__me, type__vectorcall>; \
		static constexpr bool is__noexcept = std::is_same_v<int() NOEXCEPT_OPT, int() noexcept>; \
		static constexpr bool is__rvalue_member_function = std::is_same_v<int REF_OPT, int&&>; \
		static constexpr bool is__const_member_function = std::is_same_v<CV_OPT int, const int> || std::is_same_v<CV_OPT int, const volatile int>; \
		static constexpr bool is__volatile_member_function = std::is_same_v<CV_OPT int, volatile int> || std::is_same_v<CV_OPT int, const volatile int>; \
		constexpr FunctionInformation() _NOEXCEPT { /* construct empty FunctionInformation */ } \
	};

#pragma warning(push)
#pragma warning(disable: 4003)
#pragma warning(disable: 4002)
	_MEMBER_CALL(_GET_FUNCTION_INFORMATION_, , ,)
	_MEMBER_CALL(_GET_FUNCTION_INFORMATION_, , , _NOEXCEPT)
	_MEMBER_CALL_CV_REF_NOEXCEPT(_GET_CLASS_FUNCTION_INFORMATION_)
#pragma warning(pop)
#undef _GET_FUNCTION_INFORMATION_
#undef _GET_CLASS_FUNCTION_INFORMATION_

	template<class _Func>
	constexpr FunctionInformation<_Func> GetFunctionInformation(_Func) {
		return FunctionInformation<_Func>();
	}

#define GETFUNCINFORMATIONTYPE(func, typname) \
	typedef decltype(::xkein::GetFunctionInformation(func)) typname

	/**
	* create a simple stdcall or cdcel function :
	** push ebp
	** mov ebp, esp
	*** push part...
	** mov eax, func
	** call eax
	** add esp, iscdecl ? ArgPushTimes * 4 : 0
	** pop ebp
	** retn iscdecl ? 0 : ArgPushTimes * 4
	**/
	template<class Func>
	Func CreateCallFunction32(Func func)
	{
		GETFUNCINFORMATIONTYPE(func, FuncInformation);

		static_assert(!FuncInformation::is__cdecl || !FuncInformation::is__stdcall
			, "sorry, CreateCallFunction32() could not create the function without the __stdcall or __cdcel declaration");

		constexpr bool iscdecl = FuncInformation::is__cdecl;
		size_t ArgPushTimes = FuncInformation::ArgPushTimes;
		const int codeAOffset = 1 + 2 + 3 * ArgPushTimes + 5 + 2 + 6 + 1 + 3 - 1;
		BYTE* pCode = new BYTE[codeAOffset + 1];

		pCode[0] = 0x55;
		pCode[1] = 0x8B;
		pCode[2] = 0xEC;

		pCode[codeAOffset - 16] = 0xB8;
		(DWORD&)pCode[codeAOffset - 15] = (DWORD)func;
		pCode[codeAOffset - 11] = 0xFF;
		pCode[codeAOffset - 10] = 0xD0;
		pCode[codeAOffset - 9] = 0x81;
		pCode[codeAOffset - 8] = 0xC4;
		(DWORD&)pCode[codeAOffset - 7] = iscdecl ? ArgPushTimes * 4 : 0;
		pCode[codeAOffset - 3] = 0x5D;
		pCode[codeAOffset - 2] = 0xC2;
		(WORD&)pCode[codeAOffset - 1] = WORD(iscdecl ? 0 : ArgPushTimes * 4);

		for (int aOffset = codeAOffset - 19, ebpOffset = 8; ArgPushTimes; aOffset -= 3, ebpOffset += 4) {
			pCode[aOffset] = 0xFF;
			pCode[aOffset + 1] = 0x75;
			pCode[aOffset + 2] = BYTE(ebpOffset);
			ArgPushTimes--;
		}

		VirtualProtectEx(GetCurrentProcess(), pCode, codeAOffset + 1, PAGE_EXECUTE_READWRITE, (DWORD*)&ArgPushTimes);

		return (Func)pCode;
	}


#ifdef _M_IX86
#define CreateCallFunction CreateCallFunction32
#else
#define CreateCallFunction64
#define CreateCallFunction CreateCallFunction64
#endif


	/**
	* this function is mainly used with lambda body or convert to a function pointer
	* @note you must giving the right type in order to return non-nullptr or right lambda body
	*
	* @force force get target. if true, the return value may not proper
	* @param descriptor class T2 descriptor
	* @param p
	* @retn return a callable object
	**/
	template<bool force = false, class _Ty, class _Ty2> inline
		_Ty2& std_function2callable(std::function<_Ty>& p, _Ty2& descriptor)
	{
		_Ty2& obj = *p.target<_Ty2>();
		if (force) {
			_asm {
				mov eax, obj
				test eax, eax
				jnz done_
				mov ecx, p
				mov eax, [ecx]
				call[eax + 14h]
				done_:
			}
		}
		return obj;
	}


	/**
	* proper convert function
	* @note you must giving the right type in order to return non-nullptr
	*
	* @param p
	* @retn return a callable object
	**/
	template<class _Ty, class _Ty2> inline
	_Ty& std_function2callable(std::function<_Ty2>& p)
	{
		_Ty& obj = *p.target<_Ty>();
		return obj;
	}
	//T& Obj = (((T**)p)[9])[1];
	//return Obj;

	/**
	* if compile as release it will get the right return
	* as debug, it is no problem but little jump
	*
	* @param p a reference of a lambda body
	* @retn return a function pointer
	**/
	template<class Func, class _Ty> inline
	Func lambda2function(_Ty& p) _NOEXCEPT
	{
		Func func = p;
		return func;
	}

	size_t GuessFunctionLength(void* pFunc) _NOEXCEPT
	{
		BYTE* pCode = static_cast<BYTE*>(pFunc);
		for (DWORD length = 0;; ++length, ++pCode)
		{

		}

		return 0;
	}


	class LambdaObject
	{
	public:
		template <class _LTy>
		LambdaObject(_LTy& lamb) _NOEXCEPT
		{
			pObject = Convert<void*>(&lamb);
		}

		LambdaObject* GetThis() const _NOEXCEPT
		{
			return Convert<LambdaObject*>(pObject);
		}

		void* pObject;
	};

	template <class _LTy>
	class LambdaFunction
	{
		using LFuncType = decltype(&_LTy::operator());
	public:
		LambdaFunction(_LTy& lamb) _NOEXCEPT
		{
			_Func = &_LTy::operator();
		}
		union {
			LFuncType _Func;
			void* _Ptr;
		};
	};

	template<class _Ret, class... _Types>
	class LambdaClass
	{
		using CallType = _Ret(LambdaObject::*)(_Types...);
	public:
		template <class _LTy>
		LambdaClass(_LTy& lamb) _NOEXCEPT : _Obj(lamb)
		{
			_Func = Convert<CallType>(LambdaFunction(lamb)._Ptr);
		}

		_Ret operator()(_Types&&... _Args)
		{
			return (_Obj.GetThis()->*_Func)(std::forward<_Types>(_Args)...);
		}

		LambdaObject _Obj;
		CallType _Func;

	};
 

XKEINNAMESPACE_END