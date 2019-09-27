#pragma once
#include "tools.h"

XKEINNAMESPACE_START

enum SearchMethod
{
	BinarySearch, SequenceSearch, InsertionSearch, FibonacciSearch, BlockingSearch, HashSearch
};

class SearchClass
{
protected:

	template<class _Ty>
	using SearchFunction = void*;

	template<class _Ty>
	_Ty* BinarySearch(_Ty* const _First, _Ty* const _Last, _Ty& value)
	{
		const size_t _length = _Last - _First;
		int low, high, mid;
		low = 0;
		high = n - 1;
		while (low <= high)
		{
			_Ty& mid = (low + high) / 2;
			if (a[mid] == value)
				return mid;
			if (a[mid] > value)
				high = mid - 1;
			if (a[mid] < value)
				low = mid + 1;
		}
		return nullptr;
	}

	template<class _Ty>
	SearchFunction<_Ty> GetSearchFunction(SearchMethod method) const
	{
		switch (method)
		{
		case SearchMethod::BinarySearch:
			break;
		case SearchMethod::SequenceSearch:
			break;
		case SearchMethod::InsertionSearch:
			break;
		case SearchMethod::FibonacciSearch:
			break;
		case SearchMethod::BlockingSearch:
			break;
		case SearchMethod::HashSearch:
			break;
		default:
			break;
		}
		return nullptr;
	}


public:


	template<SearchMethod method, class _Ty>
	_Ty& Search(_Ty& object, _Ty* _first, _Ty* _end) _NOEXCEPT
	{
		auto searchFunction = GetSearchFunction<_Ty>(method);
	}

	template<SearchMethod method, class _Ty>
	_Ty& AsynchronousSearch(_Ty& object, _Ty* _first, _Ty* _end) _NOEXCEPT
	{
		auto searchFunction = GetSearchFunction<_Ty>(method);
	}

};


XKEINNAMESPACE_END