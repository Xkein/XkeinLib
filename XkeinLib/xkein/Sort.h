#pragma once

#include "tools.h"

XKEINNAMESPACE_START

enum class SortMethod
{
	MergeSort, QuickSort, MergeInsertionSort, InsertionSort, StdSort, StdHeapSort, BubbleSort
};

class SortClass
{
public:
	template <class _Ty>
	using DefaultPred = std::less<>;
#define swap std::swap

	SortClass() _NOEXCEPT : CheckSequence(true), ThrowError(true)
	{
		QuickSortStruct.randomization = false;
	}


	template<class _Ty, class _Pr = DefaultPred<_Ty>>
	bool IsSorted(_Ty * const _First, _Ty * const _End, _Pr _Pred = _Pr())
	{
		if (CheckSequence)
		{
			const size_t _length = _End - _First;
			for (size_t idx = 1; idx < _length; idx++) {
				if (!_Pred(_First[idx - 1], _First[idx])) {
					if (ThrowError)
						throw std::logic_error("xkein::Sort Error");
					else
						return false;
				}
			}
		}
		return true;
	}

	template<class _Ty, size_t _length, class _Pr = DefaultPred<_Ty>>
	bool IsSorted(_Ty(&_array)[_length], _Pr _Pred = _Pr()) _NOEXCEPT
	{
		return IsSorted(&_array[0], &_array[_length], _Pred);
	}

	template<class _Iter, size_t _length, class _Pr = DefaultPred<_Iter>>
	bool IsSorted(_Iter & _First, _Iter & _End, _Pr _Pred = _Pr()) _NOEXCEPT
	{
		return IsSorted(&*_First, (&*(_End - 1)) + 1, _Pred);
	}

	template<SortMethod method = SortMethod::QuickSort, class _Ty, class _Pr = DefaultPred<_Ty>>
	_Ty * Sort(_Ty * const _First, _Ty * const _End, _Pr _Pred = _Pr()) _NOEXCEPT
	{
		(this->*GetSortFunction<_Ty, _Pr>(method))(_First, _End, _Pred);
		IsSorted(_First, _End, _Pred);
		return _First;
	}

	template<SortMethod method = SortMethod::QuickSort, class _Iter, class _Pr = DefaultPred<_Iter>>
	auto Sort(_Iter & _First, _Iter & _End, _Pr _Pred = _Pr()) _NOEXCEPT
	{
		return Sort<method>(&*_First, (&*(_End - 1)) + 1, _Pred);
	}

	template<SortMethod method = SortMethod::QuickSort, class _Ty, size_t _length, class _Pr = DefaultPred<_Ty>>
	_Ty * Sort(_Ty(&_array)[_length], _Pr _Pred = _Pr()) _NOEXCEPT
	{
		return Sort<method>(&_array[0], &_array[_length], _Pred);
	}


protected:
	template<class _Ty, class _Pr>
	using SortFunction = _Ty * (__thiscall SortClass::*)(_Ty* const, _Ty* const, _Pr) _NOEXCEPT;

#define SORT_FUNCTION(name)													\
	template<class _Ty, class _Pr>						\
	_Ty * name(_Ty* const _First, _Ty* const _End, _Pr _Pred) _NOEXCEPT

	/*
	#define SORTARRAY_FUNCTION(name)										\
		template<class _Ty, size_t _length, class _Pr = DefaultPred<_Ty>>	\
		_Ty * name(_Ty(&_array)[_length], _Pr _Pred = _Pr())			\
		{																	\
			return name(&_array[0], &_array[_length], _Pred);				\
		}
	*/

	template<class _Ty, class _Pr>
	_Ty * Merge(_Ty * const _First, size_t _mid, size_t _length, _Pr _Pred) _NOEXCEPT
	{
		size_t idxl = 0, idxr = _mid, idx = 0;
		_Ty* buf = new _Ty[_length];

		while (idx < _length)
		{
			if (idxl > _mid - 1) {
				while (idx < _length) {
					buf[idx++] = _First[idxr++];
				}
			}
			else if (idxr >= _length) {
				while (idx < _length) {
					buf[idx++] = _First[idxl++];
				}
			}
			else {
				buf[idx++] = !_Pred(_First[idxl], _First[idxr]) ? _First[idxr++] : _First[idxl++];
			}
		}

		for (idx = 0; idx < _length; idx++) {
			_First[idx] = buf[idx];
		}

		delete[] buf;

		return _First;
	}

	SORT_FUNCTION(MergeSortBase)
	{
		const size_t _length = _End - _First;
		if (_length <= 1) {
			return _First;
		}
		else if (_length == 2) {
			_Ty* const _Last = _End - 1;
			if (!_Pred(*_First, *_Last)) {
				swap(*_First, *_Last);
			}
			return _First;
		}
		else
		{
			const size_t _mid = _length / 2;

			MergeSortBase(_First, _First + _mid, _Pred);
			MergeSortBase(_First + _mid, _End, _Pred);

			Merge(_First, _mid, _length, _Pred);

			return _First;
		}
	}

	SORT_FUNCTION(MergeSort)
	{
		return MergeSortBase(_First, _End, _Pred);
	}


	SORT_FUNCTION(InsertionSort)
	{
		const size_t _length = _End - _First;

		if (_length <= 1) {
			return _First;
		}
		else
		{
			if (!_Pred(*_First, _First[1])) {
				swap(*_First, _First[1]);
			}

			for (size_t idx = 2; idx < _length; idx++) {
				_Ty tmp = _First[idx];
				size_t i = idx;

				while (i > 0) {
					if (!_Pred(_First[i - 1], tmp)) {
						_First[i] = _First[i - 1];
					}
					else break;
					i--;
				}
				_First[i] = tmp;
			}

			return _First;
		}
	}

	SORT_FUNCTION(MergeInsertionSortBase)
	{
		const size_t _length = _End - _First;

		if (_length <= 64) {
			return InsertionSort(_First, _End, _Pred);
		}
		else {
			const size_t _mid = _length / 2;

			MergeInsertionSortBase(_First, _First + _mid, _Pred);
			MergeInsertionSortBase(_First + _mid, _End, _Pred);

			Merge(_First, _mid, _length, _Pred);

			return _First;
		}
	}

	// slow
	SORT_FUNCTION(MergeInsertionSort)
	{
		return MergeInsertionSortBase(_First, _End, _Pred);
	}


	SORT_FUNCTION(StdHeapSort)
	{
		std::make_heap(_First, _End, _Pred);
		std::sort_heap(_First, _End, _Pred);
		return _First;
	}

	SORT_FUNCTION(StdSort)
	{
		std::sort(_First, _End, _Pred);
		return _First;
	}

	SORT_FUNCTION(QuickSortBase)
	{
		const size_t _length = _End - _First;
		if (_length <= 1) {
			return _First;
		}
		else if (_length == 2)
		{
			_Ty* const _Last = _End - 1;
			if (!_Pred(*_First, *_Last)) {
				swap(*_First, *_Last);
			}
			return _First;
		}
		else
		{
			_Ty* _Pivot = QuickSort_GetPivotPointer(_First, _End, _Pred);
			_Ty* _Iter1 = _First;
			for (_Ty* _Iter2 = _Iter1 + 1; _Iter2 < _End; _Iter2++)
			{
				if (!_Pred(*_Pivot, *_Iter2)) {
					swap(*++_Iter1, *_Iter2);
				}
			}
			swap(*_Pivot, *_Iter1);
			QuickSortBase(_First, _Iter1, _Pred);
			QuickSortBase(_Iter1 + 1, _End, _Pred);
		}
		return _First;
	}

	SORT_FUNCTION(QuickSort_GetPivotPointer)
	{
		UNREFERENCED_PARAMETER(_Pred);
		if (QuickSortStruct.randomization) {
			return _First + random.Generate(0, _End - _First - 1);
		}
		else return _First;
	}

	SORT_FUNCTION(QuickSort)
	{
		QuickSortBase(_First, _End, _Pred);
		QuickSortStruct.randomization = false;
		return _First;
	}

	SORT_FUNCTION(BubbleSort)
	{
		_Ty* _EndIter = _End;
		while (--_EndIter > _First) {
			for (_Ty* _Iter = _First; _Iter < _EndIter; _Iter++) {
				_Ty* nextIter = _Iter + 1;
				if (!_Pred(*_Iter, *nextIter)) {
					swap(*_Iter, *nextIter);
				}
			}
		}
		return _First;
	}

	// not necessary
	//SORTARRAY_FUNCTION(MergeSort)
	//SORTARRAY_FUNCTION(InsertionSort)
	//SORTARRAY_FUNCTION(MergeInsertionSort)
	//SORTARRAY_FUNCTION(QuickSort)

	template<class _Ty, class _Pr>
	SortFunction<_Ty, _Pr> GetSortFunction(SortMethod method) const _NOEXCEPT
	{
		switch (method)
		{
		case SortMethod::MergeSort:
			return &SortClass::MergeSort<_Ty, _Pr>;
		case SortMethod::QuickSort:
			return &SortClass::QuickSort<_Ty, _Pr>;
		case SortMethod::MergeInsertionSort:
			return &SortClass::MergeInsertionSort<_Ty, _Pr>;
		case SortMethod::InsertionSort:
			return &SortClass::InsertionSort<_Ty, _Pr>;
		case SortMethod::StdSort:
			return &SortClass::StdSort<_Ty, _Pr>;
		case SortMethod::StdHeapSort:
			return &SortClass::StdHeapSort<_Ty, _Pr>;
		case SortMethod::BubbleSort:
			return &SortClass::BubbleSort<_Ty, _Pr>;
		default:
			break;
		}
		return nullptr;
	}

public:
	bool CheckSequence;
	bool ThrowError;
	struct
	{
		bool randomization;
	} QuickSortStruct;

protected:
	Random random;
#undef swap
#undef SORT_FUNCTION
};

XKEINNAMESPACE_END