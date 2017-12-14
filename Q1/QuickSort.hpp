#include <assert.h>
#include <type_traits>
#include <algorithm>
#include <random>
#include <chrono>

using namespace std;

/*
	Quick sort implementation for index accessible containers

	coded by Ziyue Xiang
*/


// select left as pivot in the range [left, right]
template<typename RAIter>
struct LeftmostPivotPolicy {
	RAIter operator()(RAIter left, RAIter right) {
		return left;
	}
};

// select right as pivot in the range [left, right]
template<typename RAIter>
struct RightmostPivotPolicy {
	RAIter operator()(RAIter left, RAIter right) {
		return right;
	}
};

// select a random pivot in the range [left, right]
template<typename RAIter, typename RandomEngine = minstd_rand>
struct RandomPivotPolicy {
	RandomPivotPolicy() {
		randomEngine.seed((int)chrono::system_clock::now().time_since_epoch().count());
	}
	RAIter operator()(RAIter left, RAIter right) {
		auto iterDistance = distance(left, right);
		uniform_int_distribution<> distribution(0, iterDistance);
		auto offset = distribution(randomEngine);
		return left + offset;
	}
	RandomEngine randomEngine;
};


// partition the region [begin, end]
template<typename RAIter, typename Less>
inline RAIter Partition(RAIter begin, RAIter end, RAIter pivotIter, Less &&less)
{
	// end + 1 will not go out of range
	auto left = begin, right = end + 1;

	bool isBeginPivotIterSame = (begin == pivotIter);
	bool isEndPivotIterSame = (end == pivotIter);

	auto pivotValue = *pivotIter;

	/*
	if the begin iterator and the pivot iterator is not the same,
	swap their values, just to preserve the pivotValue at the 
	begin iterator position. It will not be swapped throughout 
	the entire process.
	*/
	if (!isBeginPivotIterSame) {
		// using std::swap
		swap(*begin, *pivotIter);
	}

	// finding the left and right pairs in order to swap according to the pivot
	while (true) {
		// find an element on the left portion that is larger than the pivot
		// note that (++left) means we deal with the next element of the begin iterator,
		// which stores the pivot value by now.
		while (less(*(++left), pivotValue) && left < end);

		// find an element on the right portion that is not greater than the pivot
		// this loop is guaranteed to exit at the pivot value itself
		while (less(pivotValue, *(--right)));
		
		if (left >= right)break;

		// using std::swap to swap values
		swap(*left, *right);
	}

	/*
	If the old pivot iterator is the end iterator, and new pivot 
	iterator we find is also the end iterator, to avoid dead loop,	
	we select the second to last element as the new pivot and repartition. 
	Other ways of selection is feasible as well.
	*/
	if (isEndPivotIterSame && right == end) {
		// note that end > begin, so this will not cause out of range error
		return Partition(begin, end, end - 1, less);
	}
	
	/*
	now it is guaranteed that (*right) <= pivotValue, we can now swap the
	values between the begin iterator and the right iterator to get the range
	we desired. Now the value at right iterator is just the pivotValue.
	*/
	swap(*begin, *right);
	
	return right;
}


// sort the region [begin, end)
template<typename RAIter, typename Less = less<>, typename PivotPolicy = LeftmostPivotPolicy<RAIter>>
inline void QuickSort(RAIter begin, RAIter end,
	Less &&less = Less{}, PivotPolicy &&policy = PivotPolicy{})
{
	// if there is no element in the range
	if (begin == end)return;
	// if just one element in the range
	if (end - 1 <=  begin)return;
	
	// partition the region [begin, end - 1]
	auto oldPivotIter = policy(begin, end - 1);
	auto newPivotIter = Partition(begin, end - 1, oldPivotIter, less);
	QuickSort(begin, newPivotIter, less, policy);
	QuickSort(newPivotIter + 1, end, less, policy);
}
