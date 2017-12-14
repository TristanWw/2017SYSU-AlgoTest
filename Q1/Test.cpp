#include "QuickSort.hpp"

#include <iostream>
#include <vector>
#include <memory>
#include <stdlib.h>

using namespace std;

using TestContainerType = vector<int>;

// the test data set
const TestContainerType dataSet{ 8,18,2,16,6,4,40, 3,5,7,1,9,22,11,13,10,20 };
int dataSetArray[] = { 8,18,2,16,6,4,40, 3,5,7,1,9,22,11,13,10,20 };
int dataSetArrayLength = 17;

// a small utility to show the content of containers
template<typename Iter>
void ShowArray(Iter begin, Iter end) {
	for (; begin != end; ++begin)cout << *begin << " ";
	cout << "\n";
}

// a test util function to check the correctness of QuickSort
template<typename PartitionPolicy>
void TestQuickSortCorrectness(PartitionPolicy policy) {

	minstd_rand randomEngine;
	// set the seed of the random engine
	randomEngine.seed((int)chrono::duration_cast<chrono::seconds>(chrono::system_clock::now().time_since_epoch()).count());
	uniform_int_distribution<> distribution(-1000, 1000);
	// testing the correctness of QuickSort using vectors from size 0 to 300
	for (auto i = 0; i < 300; ++i) {
		TestContainerType tempVec(i);
		TestContainerType tempVecCopy(i);
		for (auto j = 0; j < i; ++j) {
			tempVec[j] = distribution(randomEngine);
		}
		copy(tempVec.begin(), tempVec.end(), tempVecCopy.begin());

		// use QuickSort method
		QuickSort(tempVec.begin(), tempVec.end(), less<>{}, policy);
		// use std::sort method
		sort(tempVecCopy.begin(), tempVecCopy.end());

		// make sure that every element is correct
		for (auto j = 0U; j < tempVec.size(); ++j) {
			if (tempVec[j] != tempVecCopy[j])
				throw runtime_error{ "sort result mismatch" };
		}
	}
}

// A special compare functor that can count the number of comparisons 
template<typename CounterType = int>
struct CountCompare {
	CountCompare(shared_ptr<CounterType> _counter) :counter{ _counter } {}

	template<typename Type>
	bool operator()(const Type &left, const Type &right) const {
		(*counter)++;
		return left < right;
	}

	shared_ptr<CounterType> counter;
};

// A compare functor that returns whether the left value is greater than the right
struct Greater {
	template<typename Type>
	bool operator()(const Type &left, const Type &right) const {
		return left > right;
	}
};

int main() {
	// declare compare policies
	LeftmostPivotPolicy<TestContainerType::iterator> leftmostPolicy;
	RightmostPivotPolicy<TestContainerType::iterator> rightmostPolicy;
	RandomPivotPolicy<TestContainerType::iterator> randomPolicy;

	// ---------------------------------------------------------------------------------------------

	// Test QuickSort with different policies
	// This can be quite time consuming, if you find it too slow, just simply
	// make this part comment.
	TestQuickSortCorrectness(leftmostPolicy);
	TestQuickSortCorrectness(rightmostPolicy);
	TestQuickSortCorrectness(randomPolicy);
	cout << "QuickSort correctness check finished.\n\n";

	// ---------------------------------------------------------------------------------------------

	// Using iterators ensures the QuickSort method is more adaptive,
	// it can not only sort vector<> objects, but plain array as well.
	int* arrayLeftIter = dataSetArray;
	int* arrayRightIter = dataSetArray + dataSetArrayLength;
	cout << "array before sorting: \n";
	ShowArray(arrayLeftIter, arrayRightIter);
	// apply QuickSort
	QuickSort(arrayLeftIter, arrayRightIter);
	cout << "array after sorting: \n";
	ShowArray(arrayLeftIter, arrayRightIter);
	cout << "\n";

	// ---------------------------------------------------------------------------------------------

	// Testing the number of comparisons

	// first of all, get some copies for the test data set
	TestContainerType rightmostContainer, randomContainer;
	copy(dataSet.begin(), dataSet.end(), back_inserter(rightmostContainer));
	copy(dataSet.begin(), dataSet.end(), back_inserter(randomContainer));

	/*
	Secondly, decalare counters. I am using std::shared_ptr<> in CountCompare<> to
	manage resource. Feel free to change its design and use raw pointers instead.
	But be careful of memory leaks!
	*/
	shared_ptr<int> rightmostCounter = make_shared<int>(0); // initial value is 0
	shared_ptr<int> randomCounter = make_shared<int>(0);
	// declare comparison functors
	CountCompare<int> rightmostCompare{ rightmostCounter };
	CountCompare<int> randomCompare{ randomCounter };

	// apply QuickSort(rightmost policy)
	QuickSort(rightmostContainer.begin(), rightmostContainer.end(), rightmostCompare, rightmostPolicy);
	cout << "number of comparisons in rightmost pivoting: " << *rightmostCompare.counter << "\n";

	// apply QuickSort(random policy) 100 times and get the average number of comparison
	int numRuns = 100;
	int totalCompareCount = 0;
	cout << "running QuickSort with random pivoting for " << numRuns << " times.\n";
	for (auto i = 0; i < numRuns; ++i) {
		// apply QuickSort
		QuickSort(randomContainer.begin(), randomContainer.end(), randomCompare, randomPolicy);

		cout << "run" << i + 1 << ":" << *randomCompare.counter << "|";
		if ((i + 1) % 10 == 0) cout << "\n";

		// cumulate number of comparsions
		totalCompareCount += *randomCompare.counter;
		*randomCompare.counter = 0;

		// restore the randomContainter
		copy(dataSet.begin(), dataSet.end(), randomContainer.begin());
	}
	float averageNumComparison = (float)totalCompareCount / (float)numRuns;
	cout << "average number of comparisons in random pivoting: " << averageNumComparison << "\n\n";

	// ---------------------------------------------------------------------------------------------

	// sort even and odd items in different orders respectively

	TestContainerType evenItem;
	TestContainerType oddItem, oddItem2;
	for (const auto &item : dataSet) {
		if (item % 2 == 0)evenItem.push_back(item);
		else oddItem.push_back(item);
	}
	// get a copy of odd items
	copy(oddItem.begin(), oddItem.end(), back_inserter(oddItem2));

	QuickSort(evenItem.begin(), evenItem.end());

	// using the Greater comparison functor to get a descending array.
	// Alternatively, one can sort it with Less<> and apply std::reverse()
	// to that array (worse performance).
	QuickSort(oddItem.begin(), oddItem.end(), Greater{});
	QuickSort(oddItem2.begin(), oddItem2.end());
	reverse(oddItem2.begin(), oddItem2.end());

	cout << "odd numbers: \n";
	ShowArray(oddItem.begin(), oddItem.end());
	cout << "odd numbers (approach 2): \n";
	ShowArray(oddItem2.begin(), oddItem2.end());
	cout << "\n";
	cout << "even numbers: \n";
	ShowArray(evenItem.begin(), evenItem.end());
	cout << "\n";

	// ---------------------------------------------------------------------------------------------

	system("pause");

	return 0;
}