//
// xkein.h
// 
// xkein's lib 
// from  2017.9.17
// to    2018.2.7
// 
// many utility things are included, but their correctness is not make sure
// if you find something wrong, please contact me and show details
// if you want some utility, please contact me and show details
// if you have some suggestions, please contact me and show details
// 
// CONTACT WAY :
// email 739502808@qq.com
// tencent qq 739502808
// 
// IMPORTANT :
// before using all things below, you should do some test
// some testing function had written, you can define _XKEIN_TEST_ to test them
// 
// NOTICE :
// you must define _XKEIN_TEST_ at least once
// you must test all test functions carefully.
// 
// DONE :
// SmartPtr
// FunctionInformation
// CreateCallFunction
// DecString
// _GetProcAddress
// std_function2callable
// lambda2function
// GetNullReference
// ptrlen
// String
// MemoryBuffer
// MemoryManager
// Random
// Time
// swap
// SortingAlgorithms
// Array
// DynamicArray
// Stack
// Queue
// Deque
// 
// TODO :
// BigRealNumber
// CALLWITHMEMORYSTACK
// VirtualMachineClass
// PortableExecutable
// File
// 
// CREDIT :
// all xkein.h user
// AlexB
// BenzzZX
//

#pragma once
#include "xkein/File.h"
#include "xkein/Function.h"
#include "xkein/Memory.h"
#include "xkein/SmartPtr.h"
#include "xkein/String.h"
#include "xkein/Random.h"
#include "xkein/Sort.h"
#include "xkein/Time.h"
#include "xkein/Graph.h"
#include "xkein/DataStruct.h"
#include "xkein/tools.h"
#include "xkein/ExceptionHandler.h"
#include "xkein/UI/Control.h"
#include "xkein/Class.h"
#include "xkein/Search.h"
#include "xkein/Sort.h"
#include "xkein/Math.h"

#include <map>

namespace xkein
{
	/*
	//template<class T>
	class BigRealNumber
	{


	protected:
		//template std::vector<T> numbers;
		std::vector<int> integers;
		std::vector<int> decimals;

	public:
		template<class T, size_t size = sizeof(T)>
		BigRealNumber(T& value)
		{
			integers.push_back((int)value);
			if (size == sizeof(long long)) {
				intergets.push_back(value >> 32);
			}
		}

		BigRealNumber(BigRealNumber& other)
		{
			numbers = other.numbers;
		}

		void FixNumber()
		{
			for (int idx = numbers.size() - 1; idx > 0; idx--) {
				if (numbers[idx] == T(0)) {
					numbers.pop_back();
				}
			}
		}

		BigRealNumber operator=(BigRealNumber& right)
		{
			numbers = right.numbers;
		}

		BigRealNumber operator+(BigRealNumber& right)
		{
			 T overflow = 0;

		}
	};
	*/

	class Operand {
		const char* name;
		int size;
	public:
		constexpr Operand(const char* _name, int _size) : name(_name), size(_size) {}

		constexpr operator const char*() const _NOEXCEPT {
			return name;
		}

		constexpr operator char* const() const _NOEXCEPT {
			return const_cast<char* const>(name);
		}

		constexpr operator int() const _NOEXCEPT {
			return size;
		}
	};

	constexpr Operand REG8{ "reg8", 1 };
	constexpr Operand REG16{ "reg16", 2 };
	constexpr Operand MEM8{ "mem8", 1 };
	constexpr Operand MEM16{ "mem16", 2 };
	constexpr Operand IMMED8{ "immed8", 1 };
	constexpr Operand IMMED16{ "immed16", 2 };
	constexpr Operand IMMED32{ "immed32", 4 };
	//constexpr OPERAND SEGREG { "segReg", ? };
	//http://www.felixcloutier.com/x86/index.html
	/*
	class DisassemblyCode
	{
		const BYTE* bytes;
		const char* assemblyCode;
	public:
		constexpr DisassemblyCode(const char* assemblyCode, int _size) _NOEXCEPT
			: assemblyCode(assemblyCode)
		{ }

	};
	*/
	class VirtualMachineClass
	{
		protected:
		class VirtualThread
		{
		protected:
			Stack<void*> stack;
			HANDLE hThread;
			DWORD threadId;

			struct {
				WORD TempSegCs;
				ULONG TempEsp;
				ULONG SegGs;
				ULONG SegEs;
				ULONG SegDs;
				ULONG Edx;
				ULONG Ecx;
				ULONG Eax;
				ULONG SegFs;
				ULONG Edi;
				ULONG Esi;
				ULONG Ebx;
				ULONG Ebp;
				ULONG ErrCode;
				ULONG Eip;
				ULONG SegCs;
				ULONG EFlags;
			};
			static DWORD WINAPI StartThread(VirtualThread* pThis);
		public:
			VirtualThread()
			{
				hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)StartThread, this, CREATE_SUSPENDED, &threadId);
			}

			virtual void Execute()
			{
				ResumeThread(hThread);
			}

			virtual void Wait()
			{

			}
		};

		std::map<int, char*> codes;
		DynamicArray<ULONG> bps;
		DynamicArray<VirtualThread> threads;
		MemoryManager memory;

		virtual void Execute()
		{
			for (VirtualThread& thread : threads)
			{
				thread.Execute();
			}
		}
		
		virtual void Stop()
		{

		}

		virtual void SetBreakPoint(ULONG address)
		{
			bps.PushBack(address);
		}

		virtual bool Exit()
		{
			threads.Clear();
		}

		virtual BYTE* AllocMemory(size_t size)
		{
			return memory.AllocMemory(size).GetBuffer<BYTE*>();
		}

		virtual void FreeMemory(void* buffer)
		{
			memory.FreeMemory(buffer);
		}
	};

	DWORD WINAPI VirtualMachineClass::VirtualThread::StartThread(VirtualThread* pThis)
	{
		return NULL;
	}

	class PortableExecutable
	{
	protected:
		virtual void Parse()
		{
			if (hModule) {
				ParseImage();
			} else {
				ParseFile();
			}
		}

		virtual void ParseImage()
		{
			pDOS_Header = (PIMAGE_DOS_HEADER)hModule;
			pNT_Headers = (PIMAGE_NT_HEADERS)((PUCHAR)hModule + pDOS_Header->e_lfanew);
			pSection_Header = (PIMAGE_SECTION_HEADER)((PUCHAR)pNT_Headers + sizeof(IMAGE_NT_HEADERS));
			pBase_Relocation = (PIMAGE_BASE_RELOCATION)(pNT_Headers->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress + pNT_Headers->OptionalHeader.ImageBase);
		}

		// though it is stupid, but it is hard to know what it is doing.
		virtual void ParseFile()
		{
			file.Read(sizeof(IMAGE_DOS_HEADER));
			pDOS_Header = (PIMAGE_DOS_HEADER)file.GetBuffer();

			file.SetPointer(pDOS_Header->e_lfanew - sizeof(IMAGE_DOS_HEADER));
			file.Read(sizeof(IMAGE_NT_HEADERS));
			pNT_Headers = (PIMAGE_NT_HEADERS)file.GetBuffer();

			file.Read(sizeof(IMAGE_SECTION_HEADER) * pNT_Headers->FileHeader.NumberOfSections);
			pSection_Header = (PIMAGE_SECTION_HEADER)file.GetBuffer();

			file.SetPointer(GetSection(".reloc")->PointerToRawData, NULL, FILE_BEGIN);

		}

	public:
		PortableExecutable(HMODULE hModule) : hModule(hModule)
		{
			if (hModule == NULL) {
				throw std::exception("xkein::PortableExecutable::bad HMODULE!");
			}
			Parse();
		}

		PortableExecutable(LPCSTR name)
		{
			if ((hModule = GetModuleHandleA(name)) == NULL) {
				if (file.Open(name) == FALSE) {
					throw std::exception("xkein::PortableExecutable::bad file name!");
				}
			}
			Parse();
		}

		PortableExecutable(LPCWSTR name)
		{
			if ((hModule = GetModuleHandleW(name)) == NULL) {
				if (file.Open(name) == FALSE) {
					throw std::exception("xkein::PortableExecutable::bad file name!");
				}
			}
			Parse();
		}

		virtual PIMAGE_SECTION_HEADER GetSection(const char* name) const
		{
			PIMAGE_SECTION_HEADER pSecHeader = pSection_Header;
			const int sectionCount = pNT_Headers->FileHeader.NumberOfSections;
			for (int idx = 0; idx < sectionCount; idx++, pSecHeader++) {
				if (!_strnicmp(name, (PCHAR)pSecHeader->Name, 8)) {
					return pSecHeader;
				}
			}
			return NULL;
		}

		virtual void Write(DWORD ROA) const
		{

		}

	protected:
		HMODULE hModule;
		File file;
	public:
		PIMAGE_DOS_HEADER pDOS_Header;
		PIMAGE_NT_HEADERS pNT_Headers;
		PIMAGE_SECTION_HEADER pSection_Header;
		PIMAGE_BASE_RELOCATION pBase_Relocation;
	};

}


// NOTICE: you must define _XKEIN_TEST_ at least once
#ifdef _XKEIN_TEST_
#include <iostream>
// NOTICE: you must test all this functions carefully.
namespace xkeintest
{
	using namespace xkein;
	//using namespace std;
	XKEIN__DECSTRING(decstringtest, "dbp");

	// it is ok, no memory is left
	void TestSmartPtr1()
	{
		int* start = new int;

		SmartPtr<int*> a(5);
		SmartPtr<int*, true> b;
		SmartPtr<int*> c(new int);
		std::cout << *a << " " << (*b = 7) << " " << *c << std::endl;
		delete c;
		c = SmartPtr<int*>(new int(8));
		int * de = c;
		c = nullptr;
		delete de;
		std::cout << a << " " << b << " " << c << std::endl;
		a = nullptr;
		std::cout << "b:" << b << std::endl;
		b = nullptr;
		c = nullptr;

		int arr[] = { 0,1,2,4,5,0 };
		SmartPtr<int*> pa(arr);
		for (int i : pa) {
			std::cout << i;
		}
		std::cout << std::endl;

		pa++;
		for (int i : pa) {
			std::cout << i;
		}
		std::cout << std::endl;

		pa.AllocArray(7, 9);
		pa += 2;
		for (int i : pa) {
			std::cout << i;
		}
		std::cout << std::endl;
		pa.Clear();

		int* end = new int;
		std::cout << start << " " << std::endl;
		delete start;
		delete end;
	}

	// there is some fucking problem, please be aware of it
	void TestSmartPtr2()
	{
		int i[] = { 1,2,3 };
		int* p = i;
		SmartPtr<int*> pi(i);
		SmartPtr<int*> pi2 = i;

		std::cout << i << std::endl;

		std::cout << p << " " << p[1] << " " << *p++ << " " << *p << std::endl;
		p = i;
		std::cout << p << " " << *++p << " " << *(p + 1) << std::endl;
		p = i;
		std::cout << ++p << " " << *++p << " " << *(p + 1) << std::endl;

		std::cout << pi << " " << pi.at(1) << " " << *pi++ << " " << *pi << std::endl;
		std::cout << pi2 << " " << *++pi2 << " " << *(pi2 + 1) << std::endl;
	}

	class TESTC
	{
	public:
		static void* tmp;
		TESTC() = default;
		TESTC(char* pStr) : str(pStr) {};
		virtual ~TESTC() = default;
		virtual int DO() { std::cout << str << std::endl; return 1; }
		char* str;
	};
	void* TESTC::tmp;
	// test Smart Function ptr and ptrtest1 ptrtest2
	void TestSmartPtr()
	{
		typedef void void_f();
		SmartPtr<void_f*> a(TestSmartPtr1), b(TestSmartPtr2);
		a();
		b();

		using tct = decltype(&TESTC::DO);
		SmartPtr<tct>sp(&TESTC::DO);
		TESTC t{ "hello world" };
		std::cout << sp(&t) << std::endl;

		DynamicArray<>bbbb;
		int aa = 5;
		std::cout << Convert<int&, int&>(aa) << std::endl;
		std::cout << Convert<int, int&>(aa) << std::endl;
		std::cout << &Convert<int&>(aa) << std::endl;
		std::cout << Convert<int&&>(&aa) << std::endl;

		class AA {
		public:
			void ffff() { aa--; }
			//void ffff(int); error! could not get overload function type
			int aa;
		};
		GETFUNCINFORMATIONTYPE(&AA::ffff, taa);
		int c = 0;
		auto ff = [&aa, &c]() { aa++; };
		TESTC::tmp = Convert<TESTC*>(ff);
		auto fff = []() {
			Convert<decltype(&sp.operator())>(&AA::ffff)(Convert<TESTC*>(TESTC::tmp));
		};
		Convert<decltype(fff)>(ff)();
		std::cout << aa << " " << c << std::endl;
	}

	// simple show
	void TestDECSTRING()
	{
		std::cout << decstringtest << std::endl;
	}

	// ***important***
	void TestSmartPtrFunctionInfoLambdaCreateFunc()
	{
		typedef int(*pt)(int, double, char, SmartPtr<int*>&);
		auto fun = [](int a, double b, char c, SmartPtr<int*>& p)->int { return (p.Clear(), (c + a) / (int)b); };
		// convert lambda to function and get its information
		GETFUNCINFORMATIONTYPE(lambda2function<pt>(fun), t);
		SmartPtr<int*> p;
		p.AllocArray(t::ArgSizes[2], 9);
		// show each value we will use
		std::cout << t::ArgSizes[2] << " " << t::ArgLength << " " << t::ArgPushTimes << " " << t::ArgCount << std::endl;
		// get length before clear
		std::cout << p.GetArrayLength() << " " << SmartPtr<pt, true>
			// create the function in memory which call fun and invoke
			(CreateCallFunction(lambda2function<pt>(fun)))(t::ArgLength, t::ArgPushTimes, t::ArgCount, p)
			// get length before clear
			<< " " << p.GetArrayLength() << std::endl;
		// get length after clear
		std::cout << p.GetArrayLength() << std::endl;
		auto gun = [&p]() { return p.push_back(5), p.back(); };
		GETFUNCINFORMATIONTYPE(gun, g);
		std::cout << "gun is not a real function because it has capture list" << g::is_function << std::endl;
	
		class A {
		public:
			constexpr int a(int) const volatile && noexcept {return 0; };
			constexpr int b(int) const volatile & noexcept { return 1; };
			constexpr int c(int) const volatile noexcept { return -1; };
			constexpr int d(int) const volatile & noexcept { return c(4); };
		};
		GETFUNCINFORMATIONTYPE(&A::a, ta);
		GETFUNCINFORMATIONTYPE(&A::b, tb);
		GETFUNCINFORMATIONTYPE(&A::c, tc);
		SmartPtr<ta::type__me> dddddd{ &A::a };
		tc::type__thiscall aaa{ tc::type__thiscall(&A::d) };
		int b = (GetNullReference<A>().*tb::type__me(&A::b))(5);
		std::cout << b;
		std::cout << (b = (std::move(A()).*dddddd.get())(5));
		std::cout << (b = (((A*)nullptr)->*aaa)(5));
		std::cout << dddddd(nullptr, 6) << std::endl;
	}

	void TestStringMemoryManager()
	{
		String* s;
		MemoryManager memoryManager;
		int * a = (int*)memoryManager.AllocMemory<int[4]>().GetBuffer();
		a[1] = 5;
		int * b;
		memoryManager.AllocMemory<int[4]>(b);
		b[2] = 6;
		memoryManager.AllocMemory(s);
		new (s) String();
		*s = L"aaa¶ªÀ×Â¥Ä³";
		std::cout << s->ansi << s->utf8;
		memoryManager.FreeMemory(s);
	}

	int TestRandomMemoryManager()
	{
		MemoryManager memoryManager;
		Random random;
		int loopcount = -1;
		//srand((unsigned)time(NULL));
		while (loopcount++, random.Generate(-0.5, 1.2333) < random.Generate(-1ll, 16ll))
		{
			int size = 100000;
			constexpr int testnums = 50;
			int c1[testnums], c2[testnums];
			memset(c1, 0, sizeof(c1));
			memset(c2, 0, sizeof(c2));
			int* a = memoryManager.AllocMemory(size * sizeof(int)).GetBuffer<int*>();
			int* b = memoryManager.AllocMemory(size * sizeof(int)).GetBuffer<int*>();

			for (int i = 0; i < size; i++) {
				a[i] = random.Generate(1, testnums);
				c1[a[i] - 1]++;
				b[i] = rand() % (testnums);
				c2[b[i]]++;
			}
			for (int c : c1)
				std::cout << (double)c / (double)size << " ";
			std::cout << std::endl << "-------------------" << std::endl;
			for (int c : c2)
				std::cout << (double)c / (double)size << " ";
			std::cout << std::endl << std::endl;
			memoryManager.FreeMemory(a);
			memoryManager.FreeMemory(b);
		}
		std::cout << "Your lucky value:" << loopcount << std::endl;
		return loopcount;
	}
	
	void TestArray()
	{
		int fuck[60]{ 0,0,0,0,9 };
		Random random;
		Array<int, 3> a({ 3,4,5 }, fuck);
		std::cout << a[0][0][4] << std::endl;

		int (*fuck2)[4][5] = new int[3][4][5]{ {} ,{},{ { { 9 } } } };
		Array<int, 3> b({ 3,4,5 }, fuck2);
		std::cout << b[0][0][40] << " " << b[2] << " " << b[2][0] << " " << b[2][-1][5] << " " << b[0][8][0] << std::endl;

		Array<int, 3> c({ 3,4,5 }, nullptr);
		for (int& v : c) {
			v = random.Generate(0);
		}
		std::cout << c[0][0][40][-1][1] << " " << c[2] << " " << c[2][0] << " " << c[2][-1][5] << " " << c[0][8][0] << std::endl;
	
		DynamicArray<Array<int, 3>> da;
		da.PushBack(c);
		da.PushBack(b);
		da.Insert(Array<int, 3>({ 5,6,2 }, new int[60]), 9);
		// notice that new array pushed to da's back
		int(*p)[60] = (int(*)[60])(int**)&da[2][0];
		(*p)[40] = da.length();

		std::cout << da[0][2][-1][5] << " " << da[2][0][0][40] << std::endl;

		delete[] p;

		[&]() {
			__try {
				da.construct_back(std::move(da[9]));
			}
			__except (EXCEPTION_CONTINUE_EXECUTION) {
				puts("DynamicArray da deconstructor error as expected");
			}
		}();
	}
	
	void TestGraph()
	{
		DirectedGraph<> g;
		auto& arr = g.get_nodes();
		auto pNode = g.CreateNode();
		g.LinkNode(pNode, g.CreateNode(),1);
		g.LinkNode(pNode, g.CreateNode(), 2);
		g.LinkNode(pNode, g.CreateNode(), 3);
		g.LinkNode(arr[1], arr[2], 4);
		g.LinkNode(arr[3], arr[2], 5);
		g.LinkNode(arr[2], g.CreateNode(), 6);
		g.LinkNode(arr[2], g.CreateNode(), 7);
		g.LinkNode(arr[5], g.CreateNode(), 8);
		g.LinkNode(arr[4], arr.back(), 9);
		auto p = g.TopoSort();
		for (size_t idx = 0; idx < arr.length(); idx++) {
			std::cout << p[idx]->index << " ";
		}
		delete p;
		std::cout << "longest path:" << g.LongestPath() << " shortest path:" << g.ShortestPath() << std::endl;

		auto pEdge = g.LinkNode(arr[2], arr[1], 1);
		std::cout << "longest path:" << g.LongestPath() << " shortest path:" << g.ShortestPath() << std::endl;
		g.CutEdge(pEdge);

		g.LinkNode(arr.back(), arr[3], 10);
		p = g.TopoSort();
		if(p == nullptr)
			std::cout << "toposort fail as expected because it has ring(s)" << std::endl;

		g.DestroyNode(arr[2]);
		p = g.TopoSort();
		for (size_t idx = 0; idx < arr.length(); idx++) {
			std::cout << p[idx]->index << " ";
		}
		delete p;

		//UndirectedGraph g;

		std::cout << std::endl;
	}

}
#endif // _XKEIN_TEST_

// END