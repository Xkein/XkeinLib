#pragma once

#include <processthreadsapi.h>
#include "tools.h"

XKEINNAMESPACE_START
	// mainly come from the book 'Windows via c/c++'
	class Time
	{
	private:
	public:
		Time() {
			QueryPerformanceFrequency(&liPerformanceFrequency);
		}
		union FileTime {
			FILETIME filetime;
			LONGLONG qwDataTime;
		};

		template<class _Func, class... _Tys>
		LONGLONG EstimateFunctionThreadTimes(_Func _Func, _Tys... _values)
		{
			FILETIME ftDummy;
			RaiseThreadPriority();

			GetThreadTimes(GetCurrentThread(), &ftDummy, &ftDummy, &ftKernelTimeStart.filetime, &ftUserTimeStart.filetime);
			
			_Func(_values...);

			GetThreadTimes(GetCurrentThread(), &ftDummy, &ftDummy, &ftKernelTimeEnd.filetime, &ftUserTimeEnd.filetime);

			DropBackThreadPriority();

			liPerformanceStart.QuadPart = ftKernelTimeStart.qwDataTime + ftUserTimeStart.qwDataTime;
			liPerformanceEnd.QuadPart = ftKernelTimeStart.qwDataTime + ftUserTimeStart.qwDataTime;

			return GetEstimateResult();
		}

		template<bool _divFrequency = false, class _Func, class... _Tys>
		LONGLONG EstimateFunctionPerformance(_Func _Func, _Tys... _values)
		{
			RaiseThreadPriority();

			QueryPerformanceCounter(&liPerformanceStart);
			_Func(_values...);
			QueryPerformanceCounter(&liPerformanceEnd);

			DropBackThreadPriority();

			if (_divFrequency) {
				liPerformanceStart.QuadPart /= liPerformanceFrequency.QuadPart;
				liPerformanceEnd.QuadPart /= liPerformanceFrequency.QuadPart;
			}
			return GetEstimateResult();
		}

		template<class _Func, class... _Tys>
		LONGLONG EstimateFunctionCycleTime(_Func _Func, _Tys... _values)
		{
			RaiseThreadPriority();

			QueryThreadCycleTime(GetCurrentThread(), (PULONG64)&liPerformanceStart.QuadPart);
			_Func(_values...);
			QueryThreadCycleTime(GetCurrentThread(), (PULONG64)&liPerformanceEnd.QuadPart);

			DropBackThreadPriority();

			return GetEstimateResult();
		}

		BOOL RaiseThreadPriority()
		{
			curThreadPriority = GetThreadPriority(GetCurrentThread());
			return SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);
		}

		BOOL DropBackThreadPriority()
		{
			return SetThreadPriority(GetCurrentThread(), curThreadPriority);
		}

		LONGLONG GetEstimateResult() const _NOEXCEPT
		{
			return liPerformanceEnd.QuadPart - liPerformanceStart.QuadPart;
		}

	private:
		int curThreadPriority;
		LARGE_INTEGER liPerformanceFrequency;
		LARGE_INTEGER liPerformanceStart;
		LARGE_INTEGER liPerformanceEnd;
		FileTime ftKernelTimeStart;
		FileTime ftKernelTimeEnd;
		FileTime ftUserTimeStart;
		FileTime ftUserTimeEnd;
	};
XKEINNAMESPACE_END