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

struct Date
{
	enum class Month
	{
		January = 1, February, March, April, May, June, July, August, September, October, November, December
	};
	int year;
	int month;
	int day;

	int days() const _NOEXCEPT
	{
		int tmp = 0;
		for (int _month = 1; _month < month; _month++) {
			tmp += GetMonthDays(static_cast<Month>(_month));
		}
		return tmp += day;
	}

	int GetMonthDays(Month _month) const _NOEXCEPT
	{
		switch (_month)
		{
		case Month::January:
		case Month::March:
		case Month::May:
		case Month::July:
		case Month::August:
		case Month::October:
		case Month::December:
			return 31;
		case Month::April:
		case Month::June:
		case Month::September:
		case Month::November:
			return 30;
		case Month::February:
			return IsLeapYear() ? 29 : 28;
		}
		return -1;
	}
	bool IsLeapYear() const _NOEXCEPT
	{
		if (year % 4 == 0)
		{
			if (year % 100 == 0) {
				if (year % 400) {
					return false;
				}
			}
			return true;
		}
		return false;
	}
};

enum CompareSign
{
	Less = -1, Equal, Greater
};

template<class _Ty>
constexpr int Compare(_Ty left, _Ty right) _NOEXCEPT
{
	if (left < right)
		return CompareSign::Less;
	else if (left == right)
		return CompareSign::Equal;
	else //if (left > right)
		return CompareSign::Greater;
}


template<class _Ty>
constexpr int Compare(_Ty left, _Ty right, bool leftPositive, bool rightPositive) _NOEXCEPT
{
	if (left < right)
		return rightPositive ? CompareSign::Less : CompareSign::Greater;
	else if (left == right)
		return CompareSign::Equal;
	else //if (left > right)
		return leftPositive ? CompareSign::Greater : CompareSign::Less;
}

class BigNumber
{
protected:
	using _MyT = BigNumber;
	template<class _Ty>
	using _Vector = DynamicArray<_Ty>;
	static constexpr int maxcap = 10000;
	static _Vector<BigNumber> calTemp;
	bool positive;
	_Vector<int> numbers;
	_MyT* pDivResult;
	_MyT* pModResult;
	//DynamicArray<int> integers;
	//DynamicArray<int> decimals;

public:

	BigNumber() _NOEXCEPT : numbers(), positive(true), pDivResult(nullptr), pModResult(nullptr)
	{
		this->operator=(0);
	}

	template<class _Ty>
	BigNumber(_Ty value) _NOEXCEPT : numbers(), positive(true), pDivResult(nullptr), pModResult(nullptr)
	{
		this->operator=(value);
	}

	BigNumber(const _MyT& other) _NOEXCEPT : pDivResult(nullptr), pModResult(nullptr)
	{
		this->operator=(other);
	}

	void Normalize() _NOEXCEPT
	{
		for (int idx = numbers.length() - 1; idx > 0; idx--) {
			if (numbers[idx] == 0) {
				numbers.PopBack();
			}
			else break;
		}
		if (numbers.length() == 1 && numbers[0] == 0) {
			positive = true;
		}
	}

	_MyT& operator=(const _MyT& right) _NOEXCEPT
	{
		numbers = right.numbers;
		positive = right.positive;
		return *this;
	}

	template<class _Ty>
	_MyT& operator=(_Ty value) _NOEXCEPT
	{
		numbers.Clear();

		_Ty tmp = value;
		do {
			numbers.PushBack(tmp % maxcap);
		} while (tmp /= maxcap);

		positive = value >= _Ty(0);
		return *this;
	}

	_MyT& operator+=(const _MyT& right) _NOEXCEPT
	{
		if (positive != right.positive)
			return this->operator-=(-right);
		int overflow = 0;
		auto& rightNums = right.numbers;
		const size_t leftLen = numbers.length();
		const size_t rightLen = rightNums.length();
		//size_t maxLen = std::max(leftLen, rightLen);
		for (size_t idx = 0; idx < rightLen; idx++)
		{
			bool out_of_range = idx >= leftLen;
			int tmp = out_of_range ? rightNums[idx] + overflow : numbers[idx] + rightNums[idx] + overflow;
			overflow = 0;
			if (tmp >= maxcap) {
				overflow++;
				tmp -= maxcap;
			}
			if (out_of_range) {
				numbers.PushBack(tmp);
			}
			else {
				numbers[idx] = tmp;
			}
		}
		if (overflow) {
			numbers.PushBack(overflow);
		}
		return *this;
	}

	_MyT& operator-=(const _MyT& right) _NOEXCEPT
	{
		if (positive != right.positive)
			return this->operator+=(-right);
		switch (Compare(right))
		{
		case CompareSign::Equal:
			return this->operator=(0);
		case CompareSign::Less:
			return this->operator=(-(_MyT(right) -= *this));
		}
		int overflow = 0;
		auto& rightNums = right.numbers;
		const size_t rightLen = rightNums.length();
		//size_t minLen = std::min(numbers.length(), rightNums.length());
		for (size_t idx = 0; idx < rightLen; idx++)
		{
			int tmp = numbers[idx] - rightNums[idx] - overflow;
			overflow = 0;
			if (tmp < 0) {
				overflow++;
				tmp += maxcap;
			}
			numbers[idx] = tmp;
		}
		if (overflow) {
			numbers[rightLen] -= overflow;
		}
		Normalize();
		return *this;
	}

	_MyT& operator*=(const _MyT& right) _NOEXCEPT
	{
		auto& rightNums = right.numbers;
		const size_t leftLen = numbers.length();
		const size_t rightLen = rightNums.length();
		const size_t maxLen = leftLen + rightLen;
		int* tmp = new int[maxLen];
		memset(tmp, 0, sizeof(int) * maxLen);

		int overflow = 0;
		for (size_t i = 0; i < leftLen; i++)
		{
			for (size_t j = 0; j < rightLen; j++)
			{
				int& cur = tmp[i + j];
				cur += numbers[i] * rightNums[j] + overflow;
				overflow = 0;
				if (cur >= maxcap) {
					overflow = cur / maxcap;
					cur %= maxcap;
				}
			}

			if (overflow) {
				tmp[i + rightLen] += overflow;
				overflow = 0;
			}
		}

		numbers.Resize(maxLen);
		for (size_t idx = 0; idx < maxLen; idx++)
		{
			numbers[idx] = tmp[idx];
		}
		Normalize();
		delete[] tmp;

		positive = !(positive ^ right.positive);

		return *this;
	}

	_MyT& operator/=(const _MyT& right) _NOEXCEPT
	{
		div(right);
		*this = *pDivResult;
		return *this;
	}
	_MyT& operator%=(const _MyT& right) _NOEXCEPT
	{
		div(right);
		*this = *pModResult;
		return *this;
	}

	const _MyT operator++() _NOEXCEPT
	{
		*this += 1;
		return *this;
	}

	const _MyT operator++(int) _NOEXCEPT
	{
		_MyT tmp(*this);
		*this += 1;
		return tmp;
	}

	const _MyT operator--() _NOEXCEPT
	{
		*this -= 1;
		return *this;
	}

	const _MyT operator--(int) _NOEXCEPT
	{
		_MyT tmp(*this);
		*this -= 1;
		return tmp;
	}

	const _MyT& GetDivResult() const _NOEXCEPT
	{
		return *pDivResult;
	}

	const _MyT& GetModResult() const _NOEXCEPT
	{
		return *pModResult;
	}

	const _MyT operator+(const _MyT& right) const _NOEXCEPT
	{
		_MyT tmp(*this);
		return tmp += right;
	}

	const _MyT operator-(const _MyT& right) const _NOEXCEPT
	{
		_MyT tmp(*this);
		return tmp -= right;
	}

	const _MyT operator*(const _MyT& right) const _NOEXCEPT
	{
		_MyT tmp(*this);
		return tmp *= right;
	}

	const _MyT operator/(const _MyT& right) const _NOEXCEPT
	{
		_MyT tmp(*this);
		return tmp /= right;
	}

	const _MyT operator%(const _MyT& right) const _NOEXCEPT
	{
		_MyT tmp(*this);
		return tmp %= right;
	}

	const _MyT operator-() const _NOEXCEPT
	{
		_MyT tmp(*this);
		tmp.positive = !positive;
		return tmp;
	}

	const _MyT operator+() const _NOEXCEPT
	{
		return _MyT(*this);
	}
/*
if ( left <  right ) return -1;
if ( left == right ) return 0;
if ( left >  right ) return 1;
}*/
	static int Compare(const _MyT& left, const _MyT& right) _NOEXCEPT
	{
		auto& leftNums = left.numbers;
		auto& rightNums = right.numbers;
		bool leftPos = left.positive;
		bool rightPos = right.positive;
		const size_t leftLen = leftNums.length();
		const size_t rightLen = rightNums.length();

		auto ret = xkein::Compare(leftLen, rightLen, leftPos, rightPos);
		if (ret) {
			return ret;
		}

		// same length
		if (leftPos != rightPos)
			return leftPos ? CompareSign::Greater : CompareSign::Less;

		//same sign
		for (int idx = leftLen - 1; idx >= 0; idx--)
		{
			ret = xkein::Compare(leftNums[idx], rightNums[idx]);
			if (ret) {
				return leftPos ? ret : -ret;
			}
		}

		return CompareSign::Equal;
	}

	int Compare(const _MyT& right) const _NOEXCEPT
	{
		return Compare(*this, right);
	}

	bool operator!=(const _MyT& right) const _NOEXCEPT
	{
		return Compare(right);
	}
	bool operator==(const _MyT& right) const _NOEXCEPT
	{
		return Compare(right) == CompareSign::Equal;
	}
	bool operator>=(const _MyT& right) const _NOEXCEPT
	{
		return Compare(right) != CompareSign::Less;
	}
	bool operator<=(const _MyT& right) const _NOEXCEPT
	{
		return Compare(right) != CompareSign::Greater;
	}
	bool operator>(const _MyT& right) const _NOEXCEPT
	{
		return Compare(right) == CompareSign::Greater;
	}
	bool operator<(const _MyT& right) const _NOEXCEPT
	{
		return Compare(right) == CompareSign::Less;
	}

	char* to_string(char* buffer) const _NOEXCEPT
	{
		char* iter = buffer;
		if (positive == false) {
			*iter++ = '-';
		}
		bool flag = false;
		for (int idx = numbers.length() - 1; idx >= 0; idx--)
		{
			if (flag) {
				sprintf(iter, "%.4d", numbers[idx]);
			}
			else {
				flag = true;
				sprintf(iter, "%d", numbers[idx]);
			}
			iter += strlen(iter);
		}
		return buffer;
	}

	void get_string(char* buffer) _NOEXCEPT
	{
		numbers.Clear();
		size_t len = strlen(buffer);
		size_t rest = len % sizeof(int);
		numbers.Resize(rest ? len / sizeof(int) + 1 : len / sizeof(int));
		auto iter = numbers.end() - 1;
		int tmp = 0;
		size_t idx = 0;
		for (; idx < rest; idx++) {
			tmp = tmp * 10 + buffer[idx] - '0';
		}
		if (tmp) {
			*iter-- = tmp;
			tmp = 0;
		}

		size_t counter = 0;
		for (; idx < len; idx++)
		{
			tmp = tmp * 10 + buffer[idx] - '0';
			if (++counter % sizeof(int) == 0) {
				*iter-- = tmp;
				tmp = 0;
			}
		}
		if (tmp) {
			*iter = tmp;
		}
	}

	int length() const _NOEXCEPT
	{
		int len = numbers.length()-1;
		int tmp = numbers[len];
		len *= 4;
		do {
			len++;
		} while (tmp/=10);
		return len;
	}
protected:
	void div(const _MyT& right) _NOEXCEPT
	{
		if (pDivResult == nullptr) {
			pDivResult = new _MyT;
		}
		if (pModResult == nullptr) {
			pModResult = new _MyT;
		}

		*pDivResult = 0;
		*pModResult = *this;

		if (*this < right) {
			return;
		}

		int dlen = 0;
		int len = length() + 1;
		int tlen = calTemp.length();
		for (int i = tlen; i < len; i++) {
			calTemp.PushBack(*pDivResult);
		}
		calTemp[0] = right;
		calTemp[0].positive = true;
		pModResult->positive = true;
		while (calTemp[dlen++] < *pModResult)
		{
			calTemp[dlen] = calTemp[dlen - 1] * 10;
		}
		dlen--;

		for (int i = dlen - 1; i >= 0; i--)
		{
			*pDivResult *= 10;
			while (*pModResult >= calTemp[i]) {
				++*pDivResult;
				*pModResult -= calTemp[i];
			}
		}


		pDivResult->positive = !(positive ^ right.positive);
		pModResult->positive = !(positive ^ right.positive);
		pDivResult->Normalize();
		pModResult->Normalize();
	}
};

DynamicArray<BigNumber> BigNumber::calTemp;

template<class _Ty, std::enable_if_t<std::is_arithmetic_v<_Ty>> = 0>
BigNumber operator+(_Ty left, const BigNumber& right) _NOEXCEPT
{
	return right + left;
}

template<class _Ty, std::enable_if_t<std::is_arithmetic_v<_Ty>> = 0>
BigNumber operator-(_Ty left, const BigNumber& right) _NOEXCEPT
{
	return -(right - left);
}

template<class _Ty, std::enable_if_t<std::is_arithmetic_v<_Ty>> = 0>
BigNumber operator*(_Ty left, const BigNumber& right) _NOEXCEPT
{
	return right * left;
}

template<class _Ty, std::enable_if_t<std::is_arithmetic_v<_Ty>> = 0>
BigNumber operator/(_Ty left, const BigNumber& right) _NOEXCEPT
{
	BigNumber tmp(left);
	return tmp /= right;
}

template<class _Ty, std::enable_if_t<std::is_arithmetic_v<_Ty>> = 0>
bool operator!=(_Ty left, const BigNumber& right) _NOEXCEPT
{
	return right != left;
}

template<class _Ty, std::enable_if_t<std::is_arithmetic_v<_Ty>> = 0>
bool operator==(_Ty left, const BigNumber& right) _NOEXCEPT
{
	return right == left;
}

template<class _Ty, std::enable_if_t<std::is_arithmetic_v<_Ty>> = 0>
bool operator>=(_Ty left, const BigNumber& right) _NOEXCEPT
{
	return (right > left) == false;
}

template<class _Ty, std::enable_if_t<std::is_arithmetic_v<_Ty>> = 0>
bool operator<=(_Ty left, const BigNumber& right) _NOEXCEPT
{
	return (right < left) == false;
}

template<class _Ty, std::enable_if_t<std::is_arithmetic_v<_Ty>> = 0>
bool operator>(_Ty left, const BigNumber& right) _NOEXCEPT
{
	return (right >= left) == false;
}

template<class _Ty, std::enable_if_t<std::is_arithmetic_v<_Ty>> = 0>
bool operator<(_Ty left, const BigNumber& right) _NOEXCEPT
{
	return (right <= left) == false;
}


template<class _Ty>
_Ty GCD(_Ty a, _Ty b) _NOEXCEPT
{
	if (a < b) {
		std::swap(a, b);
	}
	//while (b ^= a ^= b ^= a %= b);
	while (a %= b, std::swap(a, b), b);
	return a;
}

template<class _Ty>
_Ty LCM(_Ty a, _Ty b) _NOEXCEPT
{
	return a * (b / GCD(a, b));
}

template<class _Ty>
_Ty QuickPow(_Ty x, _Ty n) _NOEXCEPT
{
	_Ty ret = 1;
	_Ty base = x;
	while (n) {
		if (n & 1)
			ret *= base;
		base *= base;
		n >>= 1;
	}
	return ret;
}

XKEINNAMESPACE_END