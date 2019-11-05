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
using namespace xkein;
using namespace xkeintest;
#include "xkein/Class.h"

using std::cin;
using std::cout; using std::endl;


class Student
{
public:
	static std::vector<Student> Array;


	struct ExamResult
	{
		double Total() const
		{
			return scores[0] + scores[1] + scores[2];
		}
		double Average() const
		{
			return Total() / 3.0;
		}
		double scores[3];
	} exam;
	int ID;
	char name[32];
	int Ranking;

	static Student* Find(int ID)
	{
		for (auto& rStudent : Student::Array) {
			if (rStudent.ID == ID)
				return &rStudent;
		}
		return nullptr;
	}

};
std::vector<Student> Student::Array;

int main()
{

	SortClass sssss;

	List<int, char*, int, int, int, int,int> list{ "id","name","NT","EN","PH","total","average" };
	list.SetWidth({ 10,15,7,7,7,7,7 });
	Student tmp;
	tmp.ID = 1906300037;
	strcpy(tmp.name, "me");
	tmp.exam.scores[0] = 150;
	tmp.exam.scores[1] = 150;
	tmp.exam.scores[2] = 150;
	Student::Array.push_back(tmp);
	int num;
	cout << "input the number of students:";
	cin >> num;
	puts("input the ID Name NT EN PH:");
	while (num--) {
		cin >> tmp.ID >> tmp.name >> tmp.exam.scores[0] >> tmp.exam.scores[1] >> tmp.exam.scores[2];
		Student::Array.push_back(tmp);
	}
	sssss.Sort(Student::Array.begin(), Student::Array.end(), [](const Student& left, const Student& right) {return left.exam.Total() >= right.exam.Total(); });

	for (auto& rStudent : Student::Array) {
		list.AddItem({ rStudent.ID,rStudent.name,rStudent.exam.scores[0],rStudent.exam.scores[1],rStudent.exam.scores[2],rStudent.exam.Total(),rStudent.exam.Average() });
	}

	list.PrintList();

	cout << "input the ID of student to search:";
	int id;
	cin >> id;

	list.SetHeader("rank", 0);
	list.SetWidth(4, 0);
	list.Clear();
	size_t length = Student::Array.size();
	for (size_t idx = 0; idx < length; idx++) {
		auto& rStudent = Student::Array[idx];
		if (rStudent.ID == id) {
			list.AddItem({ idx + 1,rStudent.name,rStudent.exam.scores[0],rStudent.exam.scores[1],rStudent.exam.scores[2],rStudent.exam.Total(),rStudent.exam.Average() });;
		}
	}

	list.PrintList();


	Interval interval{ 0,5 };
	Random random;
	Time time;
	DynamicArray<int> seq;
	seq.Resize(3000);

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
		result += time.EstimateFunctionCycleTime([&]() {sssss.Sort<SortMethod::BubbleSort>(seq.begin(), seq.end()); });
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

