#ifndef DEF_BIRTHDAY_HPP
#define DEF_BIRTHDAY_HPP

#include <assert.h>
#include <vector>
#include <random>
#include <chrono>

using namespace std;

using IntType = int32_t;
using FPType = double;

constexpr const IntType daysPerYear = 365;
static_assert(daysPerYear > 0, "there should be at least one day in a year");

// this class is definitely NOT thread safe
// this class avoids to create new objects just to boost performance
template<IntType numOfPeople, typename RandomEngine = default_random_engine>
struct BirthdayUtility {
	using ContainerType = vector<IntType>;

	static_assert(numOfPeople > 0, "there should be at least one people");

	BirthdayUtility() {
		// use the time now as the random seed
		this->randomEngine.seed((int)chrono::system_clock::now().time_since_epoch().count());
		this->days.resize(daysPerYear);
		this->birthdays.resize(numOfPeople);
	}

	// given a container of birthdays, returns the number 
	// of pairs of people having same birthdays
	IntType GetNumOfSameBirthdayPairs() {
		// creating a vector storing the number of people
		// with birthday on particular days
		for (auto &num : this->days)num = 0;

		for (const auto &day : this->birthdays) {
			this->days[day]++;
		}

		IntType result = 0;

		for (const auto &num : days) {
			if (num > 1)result += this->GetNumberOfPairs(num);
		}

		return result;
	}

	void GenerateRandomBirthday() {
		for (IntType i = 0; i < numOfPeople; ++i) {
			this->birthdays[i] = this->distribution(randomEngine);
		}
	}

private:
	// given the number of people having the same birthday,
	// returns the number of pairs of people
	IntType GetNumberOfPairs(IntType num) const {
		assert(num > 1);
		return num * (num - 1) / 2;
	}

	RandomEngine randomEngine;
	vector<IntType> days;
	vector<IntType> birthdays;
	uniform_int_distribution<IntType> distribution{0, daysPerYear - 1};
};


#endif