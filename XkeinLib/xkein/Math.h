#pragma once

#include "tools.h"

XKEINNAMESPACE_START

template<class _Ty>
struct Interval : public std::pair<_Ty, _Ty>
{
	using base = std::pair<_Ty, _Ty>;
protected:
	static constexpr unsigned int GetIntervalTypeValue(char left, char right) _NOEXCEPT
	{
		return MAKEWORD(left, right);
	}
public:
	enum class IntervalType {
		Open_Open = GetIntervalTypeValue('(', ')'),
		Close_Close = GetIntervalTypeValue('[', ']'),
		Open_Close = GetIntervalTypeValue('(', ']'),
		Close_Open = GetIntervalTypeValue('[', ')')
	};

	Interval(char left, _Ty leftValue, _Ty rightValue, char right) _NOEXCEPT : base(leftValue, rightValue)
	{
		SetIntervalType(left, right);
	}

	Interval(_Ty leftValue, _Ty rightValue, IntervalType intervalType = IntervalType::Close_Close) _NOEXCEPT : base(leftValue, rightValue)
	{
		SetIntervalType(intervalType);
	}

	void SetIntervalType(char left, char right) _NOEXCEPT
	{
		interval_type = GetIntervalType((IntervalType)GetIntervalTypeValue(left, right));
	}

	void SetIntervalType(IntervalType intervalType) _NOEXCEPT
	{
		interval_type = GetIntervalType(intervalType);
	}

	bool Contain(_Ty value) const _NOEXCEPT
	{
		return Contain(value, GetIntervalType());
	}

	bool Contain(_Ty value, IntervalType intervalType) const _NOEXCEPT
	{
		switch (GetIntervalType(intervalType))
		{
		case IntervalType::Open_Open:
			return first < value && value < second;
		case IntervalType::Close_Close:
			return first <= value && value <= second;
		case IntervalType::Open_Close:
			return first < value && value <= second;
		case IntervalType::Close_Open:
			return first <= value && value < second;
		default:
			return false;
		}
	}

	static IntervalType GetIntervalType(IntervalType intervalType)
	{
		switch (intervalType)
		{
		case IntervalType::Open_Open:
		case IntervalType::Close_Close:
		case IntervalType::Open_Close:
		case IntervalType::Close_Open:
			return intervalType;
		default:
			throw std::invalid_argument("xkein::Interval::invalid IntervalType");
		}
	}

	IntervalType GetIntervalType() const _NOEXCEPT
	{
		return GetIntervalType(interval_type);
	}

	bool IsLeftHandValue(_Ty value) const _NOEXCEPT
	{
		return IsLeftHandValue(value, GetIntervalType());
	}

	bool IsLeftHandValue(_Ty value, IntervalType intervalType) const _NOEXCEPT
	{
		switch (GetIntervalType(intervalType))
		{
		case IntervalType::Open_Open:
		case IntervalType::Open_Close:
			return value <= first;
		case IntervalType::Close_Close:
		case IntervalType::Close_Open:
			return value < first;
		default:
			return false;
		}
	}

	bool IsRightHandValue(_Ty value) const _NOEXCEPT
	{
		return IsRightHandValue(value, GetIntervalType());
	}

	bool IsRightHandValue(_Ty value, IntervalType intervalType) const _NOEXCEPT
	{
		switch (GetIntervalType(intervalType))
		{
		case IntervalType::Open_Open:
		case IntervalType::Close_Open:
			return second <= value;
		case IntervalType::Open_Close:
		case IntervalType::Close_Close:
			return second < value;
		default:
			return false;
		}
	}

	_Ty Length() const _NOEXCEPT
	{
		return second - first;
	}

protected:
	IntervalType interval_type;
};


template<class _Ty>
_Ty ShiftValueToInterval(_Ty value, Interval<_Ty> interval) _NOEXCEPT
{
	_Ty result{ value };

	if (interval.IsLeftHandValue(value))
	{
		result += (_Ty)ceil((interval.first - result) / (double)interval.Length()) * interval.Length();
	}
	else if (interval.IsRightHandValue(value))
	{
		result -= (_Ty)ceil((result - interval.second) / (double)interval.Length()) * interval.Length();
	}

	return result;
}

XKEINNAMESPACE_END