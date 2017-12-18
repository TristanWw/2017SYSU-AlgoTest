#include "SetComparison.hpp"

#include <vector>
#include <iostream>
#include <stdlib.h>

using namespace std;


using TestContainerType = vector<char>;
using NestedTestContainerType = vector<TestContainerType>;

const NestedTestContainerType SSets{
	TestContainerType{ 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'Z', 'X', 'Y' },
	TestContainerType{ 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'Z', 'X', 'Y' },
	TestContainerType{ 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'Z', 'X', 'Y' }
};

const NestedTestContainerType TSets{
	TestContainerType{ 'A', 'C', 'D', 'E', 'F', 'G', 'B', 'H', 'I', 'X', 'Y', 'Z' },
	TestContainerType{ 'A', 'C', 'D', 'E', 'F', 'G', 'B', 'H', 'K', 'X', 'Y', 'Z' },
	TestContainerType{ 'A', 'C', 'D', 'E', 'F', 'G', 'B', 'J', 'K', 'X', 'Y', 'Z' }
};

// repeat the test 100 times
const int numOfTest = 100;

// for every pair of set, only compare 6 times
const int numOfComparisonLow = 6;
const int numOfComparisonHigh = 13;

template<typename Container>
void ShowArray(const Container &container) {
	for (const auto &ele : container) {
		cout << ele << " ";
	}
	cout << "\n";
}

/*
	Two sets S and T are equal if S is a subset of T and T
	is a subset of S. Now we build a algorithm based on
	Monte-Carlo method to test the equality.
*/
int main() {

	// stores whether two sets are the same
	vector<bool> isSTSame(3);

	// uses regular method to check whether the S and T sets are the same
	for (auto i = 0; i < 3; ++i) {
		TestContainerType s, t;
		copy(SSets[i].begin(), SSets[i].end(), back_inserter(s));
		copy(TSets[i].begin(), TSets[i].end(), back_inserter(t));

		if (s.size() != t.size())
			throw runtime_error{ "array size mismatch" };

		// sort them to see whether they are the same
		sort(s.begin(), s.end());
		sort(t.begin(), t.end());

		bool isSame = true;

		for (size_t j = 0; j < s.size(); ++j) {
			if (s[j] != t[j]) {
				isSame = false;
				break;
			}
		}

		isSTSame[i] = isSame;
	}

	for (auto numOfComparison = numOfComparisonLow; numOfComparison < numOfComparisonHigh; ++numOfComparison) {

		cout << "-----------------------------------\n";
		cout << "Monte-Carlo method configuration: " << numOfComparison << " comparisons between sets each time\n";
		cout << numOfTest << " repetitive tests to find success rate\n";

		for (auto i = 0; i < 3; ++i) {
			const auto &S = SSets[i];
			const auto &T = TSets[i];
			SetComparison<TestContainerType, TestContainerType> setComparison{ S,T };

			cout << "Running test case " << i + 1 << "\n";
			cout << "The S set is: ";
			ShowArray(SSets[i]);
			cout << "The T set is: ";
			ShowArray(TSets[i]);
			cout << "Checking equality with " << numOfTest << " repetitive tests...\n";

			int equalCounter = 0;
			int notEqualCounter = 0;

			for (auto j = 0; j < numOfTest; ++j) {
				bool isSame = true;
				for (auto k = 0; k < numOfComparison; ++k) {
					auto result = move(setComparison.CompareOnceAndGetDifference());
					if (!result.isSame) {
						isSame = false;
						notEqualCounter++;
						break;
					}
				}
				if (isSame)equalCounter++;
			}

			cout << "Equal counter: " << equalCounter << " | Not equal counter: " << notEqualCounter;
			cout << " | Success rate: ";
			if (isSTSame[i]) {
				cout << ((float)equalCounter / (float)numOfTest);
			}
			else {
				cout << ((float)notEqualCounter / (float)numOfTest);
			}
			cout << "\n";
			if (i < 2)cout << "\n";
		}
		cout << "-----------------------------------\n\n\n";
	}

	system("pause");

	return 0;
} 