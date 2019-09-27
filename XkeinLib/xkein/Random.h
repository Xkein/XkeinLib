#pragma once

//#include <ctime>
#include "tools.h"
#include "Math.h"
//#include <random>
XKEINNAMESPACE_START
	class Random
	{
	protected:
		typedef int SeedType;
		SeedType Seed;

		int GenerateBase() _NOEXCEPT
		{
			int tmp = Seed;
			Seed ^= Seed >> 19 ^ 0xAAAAAAAAu;
			Seed ^= Seed << 7 ^ 0xF4959A62u;
			Seed ^= Seed << 17 ^ 0xB6A659Du;
			Seed ^= tmp >> 13;
			return Seed;
		}
	public:
		Random()
		{
			ResetSeed();
		}

		void ResetSeed(SeedType seed = -1)
		{
			Seed = seed;
		}

		int Generate(int min, int max = INT_MAX) _NOEXCEPT
		{
			return ShiftValueToInterval(GenerateBase(), { min, max });
		}

		long long Generate(long long min, long long max = LLONG_MAX) _NOEXCEPT
		{
			return ShiftValueToInterval(Convert<long long>(std::pair(GenerateBase(), GenerateBase())), { min, max });
		}

		double Generate(double min = 0.0, double max = 1.0) _NOEXCEPT
		{
			return ShiftValueToInterval(GenerateBase() / (double)GenerateBase(), { min, max });
		}

		void FillRandomData(void* pStart, void* pEnd)
		{
			char* start = (char*)pStart;

			size_t length = (char*)pEnd - start;
			constexpr size_t size_i64 = sizeof(long long);
			constexpr size_t size_i32 = sizeof(int);
			constexpr size_t size_i16 = sizeof(short);
			constexpr size_t size_i8 = sizeof(char);

			while (length >= size_i64) {
				*((long long*&)start)++ = Generate(LLONG_MIN);
				length -= size_i64;
			}
			while (length >= size_i32) {
				*((int*&)start)++ = Generate(INT_MIN);
				length -= size_i32;
			}
			while (length >= size_i16) {
				*((short*&)start)++ = (short)Generate(INT_MIN);
				length -= size_i16;
			}
			while (length >= size_i8) {
				*start++ = (char)Generate(INT_MIN);
				length -= size_i8;
			}
		}
	};
XKEINNAMESPACE_END
