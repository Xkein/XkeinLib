#pragma once

#include <crtdbg.h>
#include <winnt.h>
#include <WinBase.h>
#include "DataStruct.h"

XKEINNAMESPACE_START
	class ExceptionHandler
	{
		typedef _invalid_parameter_handler INVALID_PARAMETER_HANDLER;
	protected:
		static void __cdecl InvalidParameterHandler(
			PCTSTR expression,
			PCTSTR function,
			PCTSTR file,
			unsigned int line,
			uintptr_t pReserved)
		{
			for (INVALID_PARAMETER_HANDLER handler : pExceptionHandler->invalid_parameter_handlers)
			{
				handler(expression, function, file, line, pReserved);
			}
		}

		static LONG __stdcall UnhandledExceptionFilter(PEXCEPTION_POINTERS ExceptionInfo) _NOEXCEPT
		{
			
			for (LPTOP_LEVEL_EXCEPTION_FILTER handler : pExceptionHandler->unhandled_handlers)
			{
				if (LONG result = handler(ExceptionInfo))
					if(result != EXCEPTION_CONTINUE_SEARCH)
						return result;
			}
			return EXCEPTION_CONTINUE_SEARCH;
		}
		
		static LONG NTAPI VectoredExceptionHandler(PEXCEPTION_POINTERS ExceptionInfo) _NOEXCEPT
		{
			for (PVECTORED_EXCEPTION_HANDLER handler : pExceptionHandler->vectored_exception_handlers)
			{
				if (handler(ExceptionInfo) != EXCEPTION_CONTINUE_SEARCH)
					return EXCEPTION_CONTINUE_EXECUTION;
			}
			return EXCEPTION_CONTINUE_SEARCH;
		}

		static LONG NTAPI VectoredContinueHandler(PEXCEPTION_POINTERS ExceptionInfo) _NOEXCEPT
		{
			for (PVECTORED_EXCEPTION_HANDLER handler : pExceptionHandler->vectored_continue_handlers)
			{
				if (LONG result = handler(ExceptionInfo))
					if (result != EXCEPTION_CONTINUE_SEARCH)
						return result;
			}
			return EXCEPTION_CONTINUE_SEARCH;
		}

		ExceptionHandler() _NOEXCEPT
		{
			old_invalid_parameter_handler = _set_invalid_parameter_handler(InvalidParameterHandler);
			old_UnhandledExceptionFilter = SetUnhandledExceptionFilter(UnhandledExceptionFilter);
			pVectoredExceptionHandler = AddVectoredExceptionHandler(true, VectoredExceptionHandler);
			pVectoredContinueHandler = AddVectoredContinueHandler(true, VectoredContinueHandler);
		}

		~ExceptionHandler() _NOEXCEPT
		{
			_set_invalid_parameter_handler(old_invalid_parameter_handler);
			SetUnhandledExceptionFilter(old_UnhandledExceptionFilter);
			RemoveVectoredExceptionHandler(pVectoredExceptionHandler);
			RemoveVectoredContinueHandler(pVectoredContinueHandler);
		}

	public:
		static void CreateExceptionHandler() _NOEXCEPT
		{
			if(pExceptionHandler == nullptr)
				pExceptionHandler = new ExceptionHandler();
		}

		static void DeleteExceptionHandler() _NOEXCEPT
		{
			if (pExceptionHandler != nullptr) {
				delete pExceptionHandler;
				pExceptionHandler = nullptr;
			}
		}

		static bool CanTry() _NOEXCEPT
		{

		}

		static bool TryBegin() _NOEXCEPT
		{

		}

		static bool TryEnd() _NOEXCEPT
		{

		}

		template<class _Func, class... _Args>
		static auto Try(_Func func, _Args&&... args) _NOEXCEPT_COND(_NOEXCEPT_OPER(func))
		{
			TryBegin();
			auto ret = func(std::forward<_Args>(args)...);
			TryEnd();
			return ret;
		}

		static void AddUnhandledExceptionHandler(LPTOP_LEVEL_EXCEPTION_FILTER handler) _NOEXCEPT
		{
			pExceptionHandler->unhandled_handlers.PushBack(handler);
		}

		static void InsertUnhandledExceptionHandler(LPTOP_LEVEL_EXCEPTION_FILTER handler, size_t idx) _NOEXCEPT
		{
			pExceptionHandler->unhandled_handlers.Insert(handler, idx);
		}

		static void RemoveUnhandledExceptionHandler(LPTOP_LEVEL_EXCEPTION_FILTER handler) _NOEXCEPT
		{
			pExceptionHandler->unhandled_handlers.Remove(handler);
		}

		static void AddVectorExceptionHandler(PVECTORED_EXCEPTION_HANDLER handler) _NOEXCEPT
		{
			pExceptionHandler->vectored_exception_handlers.PushBack(handler);
		}

		static void InsertVectorExceptionHandler(PVECTORED_EXCEPTION_HANDLER handler, size_t idx) _NOEXCEPT
		{
			pExceptionHandler->vectored_exception_handlers.Insert(handler, idx);
		}

		static void RemoveVectorExceptionHandler(PVECTORED_EXCEPTION_HANDLER handler) _NOEXCEPT
		{
			pExceptionHandler->vectored_exception_handlers.Remove(handler);
		}

		static bool Set() _NOEXCEPT
		{

		}

		static void AddInvalidParameterHandler(INVALID_PARAMETER_HANDLER handler) _NOEXCEPT
		{
			pExceptionHandler->invalid_parameter_handlers.PushBack(handler);
			if (pExceptionHandler->invalid_parameter_handlers.length() == 1)
				_CrtSetReportMode(_CRT_ASSERT, false);
		}

		static void InsertInvalidParameterHandler(INVALID_PARAMETER_HANDLER handler, size_t idx) _NOEXCEPT
		{
			pExceptionHandler->invalid_parameter_handlers.Insert(handler, idx);
			if (pExceptionHandler->invalid_parameter_handlers.length() == 1)
				_CrtSetReportMode(_CRT_ASSERT, false);
		}

		static void RemoveInvalidParameterHandler(INVALID_PARAMETER_HANDLER handler) _NOEXCEPT
		{
			pExceptionHandler->invalid_parameter_handlers.Remove(handler);
			if (pExceptionHandler->invalid_parameter_handlers.empty())
				_CrtSetReportMode(_CRT_ASSERT, true);
		}

	protected:
		static ExceptionHandler* pExceptionHandler;
		INVALID_PARAMETER_HANDLER old_invalid_parameter_handler;
		LPTOP_LEVEL_EXCEPTION_FILTER old_UnhandledExceptionFilter;
		PVOID pVectoredExceptionHandler;
		PVOID pVectoredContinueHandler;
		DynamicArray<LPTOP_LEVEL_EXCEPTION_FILTER> unhandled_handlers;
		DynamicArray<PVECTORED_EXCEPTION_HANDLER> vectored_exception_handlers;
		DynamicArray<PVECTORED_EXCEPTION_HANDLER> vectored_continue_handlers;
		DynamicArray<INVALID_PARAMETER_HANDLER> invalid_parameter_handlers;
	};

	ExceptionHandler* ExceptionHandler::pExceptionHandler{
#ifdef GLOBAL_INIT_EXCEPTIONHANDLER
		new ExceptionHandler()
#else
		nullptr
#endif
	};
XKEINNAMESPACE_END