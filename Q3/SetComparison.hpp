#ifndef DEF_SETCOMPARISON_HPP
#define DEF_SETCOMPARISON_HPP

#include <chrono>
#include <random>
#include <memory>
#include <algorithm>
#include <type_traits>

using namespace std;

/*
	Both containers should support operator[] element access
	and have size(), begin(), end() method and value_type trait. 
	Once the class is constructed, it is assumed that the size of both sets 
	stays fixed. If the size changes, call the UpdateSize() method.
*/
template<typename LeftCType, typename RightCType, typename RandomEngine = default_random_engine>
class SetComparison {
private:
	using DistributionType = uniform_int_distribution<size_t>;
	using DistributionPtrType = unique_ptr<DistributionType>;
public:
	using LeftContainerType = LeftCType;
	using RightContainerType = RightCType;

	static_assert(is_same<typename LeftContainerType::value_type, 
		typename RightContainerType::value_type>::value,
		"left and right value type mismatch");

	using ValueType = typename LeftContainerType::value_type;

	struct CompareResult {
		bool isSame = false;
		unique_ptr<ValueType> value{ nullptr };
	};
public:

	SetComparison(const LeftContainerType &left, const RightContainerType &right):
		leftContainer{ left }, rightContainer{ right } {
		this->UpdateSize();
		this->randomEngine.seed((int)chrono::system_clock::now().time_since_epoch().count());
	}

	void UpdateSize() {
		if(leftContainer.size() != rightContainer.size())throw runtime_error{ "the size of two set is different" };
		if (leftContainer.size() == 0)throw runtime_error{ "the sets are empty" };
		this->distribution.get_deleter()(this->distribution.get());
		this->distribution = move(make_unique<DistributionType>(0, leftContainer.size() - 1));
	}

	/*
		We pick one element from the left set to check whether it is in the
		right set, and then pick one element from the right set to check
		whether it is in the left set. If both statements are true, it indicates
		the two sets might be equal; otherwise, the two sets must not be equal. 
		So this Monte-Carlo method is false-biased.
	*/
	bool CompareOnce() {
		// leftTest indicates "whether the left set is a subset of the right set",
		// and the rightTest shares similar definition.
		bool leftTest, rightTest;

		// pick an element from the left
		auto leftPick = this->distribution->operator()(this->randomEngine);
		const auto &leftElement = leftContainer[leftPick];
		// using std::find to look for the left element in the right set
		auto rightFindIter = find(rightContainer.begin(), rightContainer.end(), leftElement);
		leftTest = (rightFindIter != rightContainer.end());

		// if leftTest is false, return false to avoid furthur process
		if (!leftTest)return false;

		auto rightPick = this->distribution->operator()(this->randomEngine);
		const auto &rightElement = rightContainer[rightPick];
		auto leftFindIter = find(leftContainer.begin(), leftContainer.end(), rightElement);
		rightTest = (leftFindIter != leftContainer.end());
		
		return rightTest;
	}

	CompareResult CompareOnceAndGetDifference() {
		CompareResult result;
		// leftTest indicates "whether the left set is a subset of the right set",
		// and the rightTest shares similar definition.
		bool leftTest, rightTest;

		// pick an element from the left
		auto leftPick = this->distribution->operator()(this->randomEngine);
		const auto &leftElement = leftContainer[leftPick];
		// using std::find to look for the left element in the right set
		auto rightFindIter = find(rightContainer.begin(), rightContainer.end(), leftElement);
		leftTest = (rightFindIter != rightContainer.end());

		// if leftTest is false, return false to avoid furthur process
		if (!leftTest) {
			result.isSame = false;
			result.value = move(make_unique<ValueType>(leftElement));
			return result;
		}

		auto rightPick = this->distribution->operator()(this->randomEngine);
		const auto &rightElement = rightContainer[rightPick];
		auto leftFindIter = find(leftContainer.begin(), leftContainer.end(), rightElement);
		rightTest = (leftFindIter != leftContainer.end());

		if (!rightTest) {
			result.isSame = false;
			result.value = move(make_unique<ValueType>(rightElement));
			return result;
		}

		result.isSame = true;
		return result;
	}
private:
	RandomEngine randomEngine;
	const LeftContainerType &leftContainer;
	const RightContainerType &rightContainer;
	DistributionPtrType distribution;
};

#endif