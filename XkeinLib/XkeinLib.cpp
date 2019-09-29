// XkeinLib.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <functional>
#include <iostream>
#include <vector>

#define NOSTACKCALLRECURSIVELYFUNC(func) \
helper func = []



#define _XKEIN_TEST_
//#define GLOBAL_INIT_EXCEPTIONHANDLER
#include "xkein.h"
using namespace xkein;
using namespace xkeintest;
#include<iomanip>
#include<map>
#include<variant>
#include<typeinfo>
#include "xkein/Class.h"
#include <set>

using std::cout; using std::endl;


void tttest(double a, double b)
{
	std::cout << a << b << std::endl;
}


int main() noexcept
{
	
	Interval interval{ 0,5 };

	Random random;
	Time time;
	DynamicArray<int> seq;

	seq.Resize(25);

	long long result = 0;


	SortClass sssss;

	random.FillRandomData(seq.begin(), seq.end());
	sssss.Sort<SortMethod::BubbleSort>(seq.begin(), seq.end());
	

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
		result += time.EstimateFunctionCycleTime([&]() {sssss.Sort<SortMethod::StdSort>(seq.begin(), seq.end()); });
	}
	cout << result / 100i64 << endl;
	result = 0;
	random.ResetSeed();

	for (int idx = 0; idx < 100; idx++) {
		random.FillRandomData(seq.begin(), seq.end());
		//sssss.QuickSortStruct.randomization = true;
		result += time.EstimateFunctionCycleTime([&]() {sssss.Sort<SortMethod::StdHeapSort>(seq.begin(), seq.end()); });
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

