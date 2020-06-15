#pragma once

#include <array>
#include <stringapiset.h>
#include "tools.h"

XKEINNAMESPACE_START
	class String
	{
	protected:
		typedef char UTF8;
	public:
		//all len must be plus 1 or it will cause a delete[] error.

		static UTF8* Unicode2Utf8(const wchar_t* unicode)
		{
			size_t len = ::WideCharToMultiByte(CP_UTF8, 0, unicode, -1, NULL, 0, NULL, NULL);
			char *utf8 = new char[len + 1];
			utf8[len] = '\0';
			::WideCharToMultiByte(CP_UTF8, 0, unicode, -1, utf8, len, NULL, NULL);
			return utf8;
		}

		static wchar_t* UTF82Unicode(const UTF8* utf8)
		{
			size_t utf8Len = strlen(utf8);
			size_t unicodeLen = ::MultiByteToWideChar(CP_UTF8, NULL, utf8, utf8Len, NULL, 0);
			wchar_t* unicode = new wchar_t[unicodeLen + 1];
			::MultiByteToWideChar(CP_UTF8, NULL, utf8, utf8Len, unicode, unicodeLen);
			unicode[unicodeLen] = L'\0';
			return unicode;
		}

		static wchar_t* Ansi2Unicode(const char *ansi)
		{
			size_t unicodeLen = MultiByteToWideChar(CP_ACP, 0, ansi, -1, NULL, 0);
			wchar_t* unicode = new wchar_t[unicodeLen + 1];
			MultiByteToWideChar(CP_ACP, 0, ansi, -1, unicode, unicodeLen);
			unicode[unicodeLen] = L'\0';
			return unicode;
		}

		static char* Unicode2Ansi(const wchar_t* unicode)
		{
			size_t ansiLen = WideCharToMultiByte(CP_ACP, NULL, unicode, -1, NULL, 0, NULL, FALSE);
			char* ansi = new char[ansiLen + 1];
			WideCharToMultiByte(CP_ACP, NULL, unicode, -1, ansi, ansiLen, NULL, FALSE);
			ansi[ansiLen] = '\0';
			return ansi;
		}
	public:
		String() : ansi(nullptr), unicode(nullptr), utf8(nullptr) {}

		template<bool isUtf8 = false>
		String(STATEMENTS(const utf8* pUtf8) const char* pAnsi) : ansi(nullptr), unicode(nullptr), utf8(nullptr)
		{
			operator=<isUtf8>(pAnsi);
		}

		String(const wchar_t* pUnicode)
		{
			operator=(pUnicode);
		}

		~String()
		{
			CleanUp();
		}

		void CleanUp()
		{
			if (unicode)
			{
				delete[] unicode;
				delete[] ansi;
				delete[] utf8;
			}
		}

		template<bool isUtf8 = false>
		operator const char* STATEMENTS(const utf8* pUtf8)() const _NOEXCEPT
		{
			return isUtf8 ? utf8 : ansi;
		}

		operator const wchar_t*() const _NOEXCEPT
		{
			return unicode;
		}

		template<bool isUtf8 = false>
		String& operator=(STATEMENTS(const utf8* pUtf8) const char* pAnsi)
		{
			CleanUp();
			size_t len = strlen(pAnsi) + 1;
			if (isUtf8) {
				utf8 = new UTF8[len];
				strcpy_s(utf8, len, pAnsi);
				unicode = UTF82Unicode(pAnsi);
				ansi = Unicode2Ansi(unicode);
			}
			else {
				ansi = new char[len];
				strcpy_s(ansi, len, pAnsi);
				unicode = Ansi2Unicode(ansi);
				utf8 = Unicode2Utf8(unicode);
			}
			return *this;
		}

		String& operator=(const wchar_t* pUnicode)
		{
			CleanUp();
			size_t len = wcslen(pUnicode) + 1;
			unicode = new wchar_t[len];
			wcscpy_s(unicode, len, pUnicode);
			ansi = Unicode2Ansi(unicode);
			utf8 = Unicode2Utf8(unicode);
			return *this;
		}

		bool operator==(String& right)
		{
			return CompareStringOrdinal(0, 0, 0, 0, 0);
		}

	public:
		char* ansi;
		wchar_t* unicode;
		UTF8* utf8;
	};

#pragma warning(push)
#pragma warning(disable: 4244)
	template <class _Ty, int size, class _Ty2>
	constexpr auto _DecArray(const _Ty(&arr)[size], _Ty2 decreasement)
	{
		std::array<_Ty, size> tmp{};
		for (int i = 0; i < size - 1; ++i) {
			tmp[i] = arr[i] - decreasement;
		}
		return tmp;
	}
#pragma warning(pop)

	// for using, #define DECSTRING(name, string) XKEIN__DECSTRING(name, string);
#define XKEIN__DECSTRING(name, string) \
namespace xkein_decarrayspace { constexpr auto name##dec = ::xkein::_DecArray(string, 1); } \
constexpr auto name = xkein_decarrayspace::name##dec.data()


XKEINNAMESPACE_END