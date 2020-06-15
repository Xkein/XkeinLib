// XkeinLib.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <functional>
#include <iostream>
#include <vector>
#include <iomanip>

#define NOSTACKCALLRECURSIVELYFUNC(func) \
helper func = []



#define _XKEIN_TEST_
//#define GLOBAL_INIT_EXCEPTIONHANDLER
#include "xkein.h"
#include "xkein/Class.h"

using std::cin;
using std::cout; using std::endl;
using std::istream; using std::ostream;
char buf[500], buf2[500];
int intarr[]{1,2,12,15,15,13,135,135,135,1};
using namespace xkein;

template <class Lambda>
auto test(Lambda l)
{
	return &Lambda::operator();
}
auto lexm = [](int a, int b) {return 0; };
class A
{
public:
	virtual int test(LambdaClass<int, int, int> lam, int a, int b)
	{
		return lam(std::forward<int>(a), std::forward<int>(b));
	}
};

int main()
{
	int c = 1;
	auto ll = [&c](int a, int b) {return a + b + c; };
	A a;
	LambdaClass<int, int, int> lam(ll);
	cout << lam(2, 5) << endl;
	c = 7;
	cout << a.test(ll, 2, 5) << endl;
	cout << a.test([&ll, c](int a, int b) {return a + b + ll(2, 5) + c; }, 2, 5) << endl;

	int aaaa;
	auto l = [aaaa](int b) {return true; };
	auto ret = test(l);
	(l.*ret)(1);
	l.operator()(1);
	l(1);
	void* p = &l;

	long long test = (long long)(-0.0);

	double fAngle = std::acos(1.0);
	cout << Convert<long long>(fAngle) << endl;
	int nIndex = 31 - int(fAngle / (3.141592654f / 2) * 32.0);
	cout << nIndex << endl;
	fAngle = -0.0;
	bool ttt = 0.0 == fAngle;
	bool ttt2 = -0.0 == fAngle;
	nIndex = 31 - int(fAngle / (3.141592654f / 2) * 32.0);
	__asm {
		cvttss2si   eax, xmm0
		mov [nIndex], eax
	}
	cout << nIndex << endl;

	using namespace xkeintest;

	/*
	char n[100001];
	const int mod = 1e9 + 7;
	int x;
	cin >> x >> n;//x是底数，n是指数
	long long len = strlen(n), t = 0, i, ans = 1;
	for (i = 0; i < len; i++)t = (t * 10 + n[i] - '0') % (mod - 1);
	while (t > 0) {
		if (t & 1) ans = ans * x % mod;
		x = x * x % mod;
		t >>= 1;
	}
	cout << ans << endl;
	*/
	constexpr auto ee = GetArrayInformation(intarr); 
	constexpr const int* ppa = ee._ElementPtr;
	//constexpr int aaaaaa = ee.GetElement(6);
	BigNumber a1, b1;
	cin >> buf;
	a1.get_string(buf);
	
	a1 /= 2;

	cout << a1.to_string(buf) << endl;

	b1 = 2;
	int pw = 1023;
	for (size_t i = 1; i < pw; i++)
	{
		sprintf(buf2, "%.0f", pow(2, i + 1));
		a1 *= b1;
		a1.to_string(buf);
		if (strcmp(buf, buf2) != 0) {
			cout << buf << endl;
			cout << buf2 << endl;
		}
	}
	cout << buf << endl;
	SortClass sssss;
	Interval interval{ 0,5 };
	Random random;
	Time time;
	DynamicArray<int> seq;
	seq.Resize(64);
	int arr[2][3][2]{};
	//int arr2[2], arr3[ee.GetElement(6)];
	//cout << ee.ElementLength[2] << ee.GetElement(6);
	long long result = 0;

	for (int idx = 0; idx < 100; idx++) {
		random.FillRandomData(seq.begin(), seq.end());
		result += time.EstimateFunctionCycleTime([&]() {sssss.Sort<SortMethod::QuickSort>(seq.begin(), seq.end()); });
	}
	cout << result / 100i64 << endl; 
	result = 0;
	random.ResetSeed();

	for (int idx = 0; idx < 100; idx++) {
		random.FillRandomData(seq.begin(), seq.end());
		result += time.EstimateFunctionCycleTime([&]() {sssss.Sort<SortMethod::MergeInsertionSort>(seq.begin(), seq.end()); });
	}
	cout << result / 100i64 << endl;
	result = 0;
	random.ResetSeed();

	for (int idx = 0; idx < 100; idx++) {
		random.FillRandomData(seq.begin(), seq.end());
		result += time.EstimateFunctionCycleTime([&]() {sssss.Sort<SortMethod::MergeSort>(seq.begin(), seq.end()); });
	}
	cout << result / 100i64 << endl;
	result = 0;
	random.ResetSeed();

	for (int idx = 0; idx < 100; idx++) {
		random.FillRandomData(seq.begin(), seq.end());
		result += time.EstimateFunctionCycleTime([&]() {sssss.Sort<SortMethod::InsertionSort>(seq.begin(), seq.end()); });
	}
	cout << result / 100i64 << endl;
	result = 0;
	random.ResetSeed();

	for (int idx = 0; idx < 100; idx++) {
		random.FillRandomData(seq.begin(), seq.end());
		result += time.EstimateFunctionCycleTime([&]() {sssss.Sort<SortMethod::SelectionSort>(seq.begin(), seq.end()); });
	}
	cout << result / 100i64 << endl;
	result = 0;
	random.ResetSeed();
	
	//random.FillRandomData(seq.begin(), seq.end());
	//sssss.Sort<SortMethod::InsertionSort>((int*)seq.begin(), (int*)seq.end());

	TestSmartPtr();
	TestSmartPtrFunctionInfoLambdaCreateFunc();
	TestStringMemoryManager();
	ExceptionHandler::CreateExceptionHandler();
	//ExceptionHandler::DeleteExceptionHandler();
	std::map<int, int>d;
	//TestSmartPtrFunctionInfoLambdaCreateFunc();
	TestGraph();

	while (true) {
		DirectedGraph<> g;
		for (size_t level = random.Generate(10, 100); level; level--) {
			size_t oldNodeCount = g.get_nodes().length();
			for (size_t newNodeCount = random.Generate(1, 2); newNodeCount; newNodeCount--) {
				g.LinkNode(g.get_nodes()[random.Generate(0, oldNodeCount)], g.CreateNode(), random.Generate(1, 10));
			}
			g.LinkNode(g.get_nodes()[random.Generate(0, g.get_nodes().length() - 1)], g.get_nodes()[random.Generate(0, g.get_nodes().length() - 1)], random.Generate(1, 10));
		}
		cout.setf(std::ios::left);
		if(time.EstimateFunctionPerformance([&]() {
			cout << std::setw(4) << g.get_nodes().length() << " "
				<< std::setw(4) << g.get_edges().length() << " "
				<< std::setw(4) << g.LongestPath() << " " << g.ShortestPath() << endl;
		}) > 0x10000)
			time.GetEstimateResult();
		g.~DirectedGraph();
		//Sleep(200);
	}

	return 0;
}

