#pragma once

#include "..\DataStruct.h"
#include <utility>
#include <algorithm>
#include <iostream>
#include <string>

XKEINNAMESPACE_START

template<class... _Types>
class List
{
public:
	static constexpr int typeCounts = TypesCounter<_Types...>::ret;
	using ItemTuble = std::tuple<_Types...>;

	static constexpr size_t tabSize = 2;

	List() : items()
	{
		InitWidths();
		InitHeaders();
	}

	List(const char* const* pHeaders) : items()
	{
		InitWidths();
		SetHeader(pHeaders);
	}

	List(std::initializer_list<const char*> headerList) : items()
	{
		InitWidths();
		SetHeader(headerList);
	}

	void InitWidths(size_t defaultWidth = 10) _NOEXCEPT
	{
		for (auto& width : widths) {
			width = defaultWidth;
		}
	}

	void InitHeaders(const char* defaultHeader = "<unnamed>") _NOEXCEPT
	{
		for (auto& header : headers) {
			header = defaultHeader;
		}
	}

	void SetHeader(const char* header, size_t idx) _NOEXCEPT
	{
		if (idx < typeCounts) {
			headers[idx] = header;
		}
	}

	void SetHeader(const char* const* pHeaders) _NOEXCEPT
	{
		for (int idx = 0; idx < typeCounts; idx++) {
			headers[idx] = pHeaders[idx];
		}
	}

	void SetHeader(std::initializer_list<const char*> headerList) _NOEXCEPT
	{
		SetHeader(headerList.begin());
	}

	void AddItem(const ItemTuble& item) _NOEXCEPT_COND(_NOEXCEPT_OPER(std::is_nothrow_copy_constructible_v<ItemTuble> && std::is_nothrow_destructible_v<ItemTuble>))
	{
		items.PushBack(item);
	}

	void InsertItem(const ItemTuble& item, size_t idx) _NOEXCEPT_COND(_NOEXCEPT_OPER(std::is_nothrow_copy_constructible_v<ItemTuble> && std::is_nothrow_destructible_v<ItemTuble>))
	{
		items.Insert(item, idx);
	}

	void SetWidth(size_t width, size_t idx) _NOEXCEPT
	{
		if (idx < typeCounts) {
			widths[idx] = width;
		}
	}

	void SetWidth(const size_t* pWidths) _NOEXCEPT
	{
		for (int idx = 0; idx < typeCounts; idx++) {
			widths[idx] = pWidths[idx];
		}
	}

	void SetWidth(std::initializer_list<size_t> widthList) _NOEXCEPT
	{
		SetWidth(widthList.begin());
	}

	void PrintList()
	{
		using std::cout;
		using std::endl;

		const auto oldWidth = cout.width();

		size_t sepPos[typeCounts + 1]{ 0 };
		size_t totalWidth = 0;
		const size_t totalHeight = items.length() * 2 + 3;
		for (size_t idx = 0; idx < typeCounts; idx++) {
			sepPos[idx + 1] = totalWidth += widths[idx] + tabSize;
		}

		size_t curWidth = 0;
		std::string buf;
		buf.resize(3 * (totalWidth - 2 * typeCounts - 1));
		enum class ListTabType
		{
			head = -1, middle, tail
		};
		auto PrintTabLine = [&](ListTabType type) {
			const auto tabHead = type == ListTabType::middle ? "©À" : type == ListTabType::head ? "©°" : "©¸";
			const auto tabMiddle = type == ListTabType::middle ? "©à" : type == ListTabType::head ? "©Ð" : "©Ø";
			const auto tabTail = type == ListTabType::middle ? "©È" : type == ListTabType::head ? "©´" : "©¼";
			//buf[sepPos[0]] = tabHead;
			size_t curPos = 0;
			buf.replace(curPos, tabSize, tabHead);
			for (size_t idx = 0; idx < typeCounts; idx++) {
				curWidth = widths[idx];
				while(curWidth--)
					buf.replace(curPos += tabSize, tabSize, "©¤");
				//buf[sepPos[idx + 1]] = tabMiddle;
				buf.replace(curPos += tabSize, tabSize, tabMiddle);
			}
			//buf[sepPos[typeCounts]] = tabTail;
			buf.replace(curPos, tabSize, tabTail);
			cout << buf << endl;
		};

		PrintTabLine(ListTabType::head);

		constexpr auto tab = "©¦";
		//buf[sepPos[0]] = tab;
		buf.replace(sepPos[0], tabSize, tab);
		for (size_t idx = 0; idx < typeCounts; idx++) {
			size_t headerLength = strlen(headers[idx]);
			size_t spaceWidth = widths[idx] - headerLength;

			if (headerLength > widths[idx])
				throw std::out_of_range("xkein::Console::List::too short width!");

			if(spaceWidth)
				buf.replace(sepPos[idx] + tabSize, spaceWidth, spaceWidth, ' ');
			buf.replace(sepPos[idx] + tabSize + spaceWidth, headerLength, headers[idx]);
			//buf[sepPos[idx + 1]] = tab;
			buf.replace(sepPos[idx + 1], tabSize, tab);
		}
		buf.replace(buf.begin() + sepPos[typeCounts] + tabSize, buf.end(), buf.size(), '\0');
		//buf[sepPos[typeCounts] + tabSize] = '\0';
		cout << buf << endl;

		for (size_t curHeight = 3, itemIdx = 0; curHeight < totalHeight; curHeight++) {
			if (curHeight % 2) {
				PrintTabLine(ListTabType::middle);
			}
			else {
				PrintItems(items[itemIdx++], std::make_index_sequence<typeCounts>());
			}
		}

		PrintTabLine(ListTabType::tail);

		cout.width(oldWidth);
	}

	void Clear() _NOEXCEPT
	{
		items.Clear();
	}

	void Erase(size_t idx) _NOEXCEPT
	{
		items.Erase(idx);
	}

protected:

	template<class _Ty, size_t... idxes>
	void PrintItems(_Ty& obj, std::index_sequence<idxes...>) _NOEXCEPT
	{
		using std::cout;
		using std::endl;

		constexpr auto tab = "©¦";
		cout.write(tab, tabSize);
		std::initializer_list{ (cout.width(widths[idxes]), cout << std::get<idxes>(obj), cout.write(tab, tabSize), idxes)... };
		cout << endl;
	}


	const char* headers[typeCounts];
	size_t widths[typeCounts];
	DynamicArray<ItemTuble> items;
};

XKEINNAMESPACE_END