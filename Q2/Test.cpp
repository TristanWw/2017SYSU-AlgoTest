// release build is preferred because this program needs a lot of calculations.

#include "Birthday.hpp"

#include <limits>
#include <iostream>
#include <iomanip>
#include <stdlib.h>
#include <math.h>

using namespace std;

// indicates how many times should the randomized algorithm run to get a result
constexpr const IntType numOfPass = 8000;
static_assert(numOfPass <= numeric_limits<IntType>::max(), "the number of runs exceeds the maximum value of its type");

// indicates how many number of people we are to test
constexpr const IntType numOfPeople = 25;
static_assert(numOfPeople > 0 , "there should be at least one people");

// indicates how many times we repeat the probability calculation
constexpr const IntType numOfAttempt = 300;

// indicates the precision when printing floating point numbers
constexpr const IntType floatingPointPrecision = 8;

using BUtil = BirthdayUtility<numOfPeople, minstd_rand>;

FPType GetProbabilityOfPairsMoreThan(IntType numPair, BUtil &util) {
	IntType success = 0;
	for (IntType i = 0; i < numOfPass; ++i) {
		util.GenerateRandomBirthday();
		auto numOfPairs = util.GetNumOfSameBirthdayPairs();
		if (numOfPairs > numPair)success++;
	}
	return (FPType)success / (FPType)numOfPass;
}

FPType GetProbabilityOfPairsEqual(IntType numPair, BUtil &util) {
	IntType success = 0;
	for (IntType i = 0; i < numOfPass; ++i) {
		util.GenerateRandomBirthday();
		auto numOfPairs = util.GetNumOfSameBirthdayPairs();
		if (numOfPairs == numPair)success++;
	}
	return (FPType)success / (FPType)numOfPass;
}

struct Statistic {
	FPType mean = 0;
	FPType variance = 0;
	FPType standardDeviation = 0;
};

template<typename Iter>
Statistic GetStatistic(Iter begin, Iter end) {
	static_assert(is_same<Iter::value_type,FPType>::value, "incompatible value type");

	if (begin == end)throw runtime_error{ "no element received" };

	IntType counter = 0;
	FPType sum = 0;
	for (auto iter = begin; iter != end; ++iter) {
		sum += *iter;
		counter++;
	}

	FPType mean = sum / (FPType)counter;

	FPType variance = 0;

	for (auto iter = begin; iter != end; ++iter) {
		FPType difference = *iter - mean;
		variance += (difference * difference);
	}

	variance /= (FPType)counter;

	Statistic result;
	result.mean = mean;
	result.variance = variance;
	result.standardDeviation = sqrt(variance);

	return result;
}

void ShowStatistic(const Statistic &stat) {
	cout << "mean: " << fixed << setprecision(floatingPointPrecision) << stat.mean << "\n";
	cout << "mean (scientific): " << scientific << setprecision(floatingPointPrecision) << stat.mean << "\n";

	cout << "variance: " << fixed << setprecision(floatingPointPrecision) << stat.variance << "\n";
	cout << "variance (scientific): " << scientific << setprecision(floatingPointPrecision) << stat.variance << "\n";

	cout << "standard deviation: " << fixed << setprecision(floatingPointPrecision) << stat.standardDeviation << "\n";
	cout << "standard deviation (scientific): " << scientific << setprecision(floatingPointPrecision) << stat.standardDeviation << "\n";
}


/*
    It can be seen that the probability of having at least one
	pair of people sharing the same birthday within 25 people is
	p = 1 - P(356, 25)/365^25 = 1 - 365!/(340! * 365 ^ 25)

	We use a randomized algorithm to pick 25 arbitrary dates in a year
	and count the number of identical date pairs. By determining whether
	there are indentical date pairs, we can find an approximation of p,
	and thus calculate 365!/(340! * 365 ^ 25).
*/

int main() {

	BUtil util;

	vector<FPType> moreThan0{};

	cout << "calculating the probability by doing " << numOfAttempt << " experiments...\n";
	for (IntType i = 0; i < numOfAttempt; ++i) {
		moreThan0.push_back(GetProbabilityOfPairsMoreThan(0, util));
	}
	auto stat = GetStatistic(moreThan0.begin(), moreThan0.end());
	ShowStatistic(stat);
	cout << "\n";

	cout << "365!/(340! * 365 ^ 25) = " << fixed << setprecision(floatingPointPrecision) << ((FPType)1 - stat.mean) << "\n";

	system("pause");
	return 0;
}