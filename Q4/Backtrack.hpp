#ifndef DEF_BACKTRACE_HPP
#define DEF_BACKTRACE_HPP

#include <algorithm>
#include <vector>
#include <limits>
#include <bitset>

using namespace std;

template<typename WeightT, typename PriceT, typename FPT = double>
struct Item {
	using WeightType = WeightT;
	using PriceType = PriceT;
	using FPType = FPT;
	WeightType weight;
	PriceType price;

	FPType GetPriceWeightRatio() const { return (FPType)this->price / (FPType)this->weight; }
};

template<typename WeightT, typename PriceT, typename ItemContainerT>
class KnapsackSolver;

template<typename WeightT, typename PriceT, typename ItemContainerT = vector<Item<WeightT, PriceT>>>
class Knapsack {
public:
	using WeightType = WeightT;
	using PriceType = PriceT;
	using ItemType = Item<WeightType, PriceType>;
	using ItemContainerType = ItemContainerT;
	using ItemContainerConstIter = typename ItemContainerType::const_iterator;
	using KnapsackSolverType = KnapsackSolver<WeightType, PriceType, ItemContainerType>;

private:

	struct ItemSortingPredicate {
		bool operator()(const ItemType &left, const ItemType &right) const {
			return left.GetPriceWeightRatio() > right.GetPriceWeightRatio();
		}
	};

public:

	Knapsack() {}
	Knapsack(WeightType _maxWeight):maxWeight{_maxWeight}{}
	Knapsack(ItemContainerConstIter begin,
		ItemContainerConstIter end,
		WeightType _maxWeight) :maxWeight{ _maxWeight } {
		copy(begin, end, back_inserter(this->item));
	}

	const ItemContainerType & GetItems() const { return this->item; }
	ItemContainerType & GetItems() { return this->item; }
	void AssignItems(ItemContainerType &&items) { this->item = move(items); }

	WeightType GetMaxWeight() const { return this->maxWeight; }
	void SetMaxWeight(WeightType weight) { this->maxWeight = weight; }

	// sort the items according to the cost/performance ratio
	void SortItems() {
		sort(this->item.begin(), this->item.end(), ItemSortingPredicate{});
	}

	ItemContainerType GetSortedItems() const {
		ItemContainerType result;
		result.reserve(this->item.size());
		copy(this->item.begin(), this->item.end(), back_inserter(result));
		sort(result.begin(), result.end(), ItemSortingPredicate{});
		return result;
	}

	ItemContainerType GetOptimalChoice() const {

	}

private:
	ItemContainerType item;
	WeightType maxWeight{ 0 };


};

template<typename WeightT, typename PriceT, typename ItemContainerT>
class KnapsackSolver {
public:
	using KnapsackType = Knapsack<WeightT, PriceT, ItemContainerT>;
	using WeightType = WeightT;
	using PriceType = PriceT;
	using ItemType = Item<WeightType, PriceType>;
	using ItemContainerType = typename KnapsackType::ItemContainerType;

	KnapsackSolver(const KnapsackType &_knapsack) :knapsack{ _knapsack } {}

	/*
	Unlike general backtracking algorithms, which generate feasible solutions,
	the knapsack problem requires an optimal solution. Thus a special algorithm
	which traverses through the entire solution space is needed.
	*/
	ItemContainerType SortedSolve() {
		this->Init();
		this->choice.resize(this->knapsack.GetItems().size(), false);
		this->bestChoice.resize(this->knapsack.GetItems().size(), false);

		auto sortedItem = this->knapsack.GetSortedItems();
		this->sortedItem = move(sortedItem);
	}

	ItemContainerType DirectSolve() {
		this->Init();
		this->choice.resize(this->knapsack.GetItems().size(), false);
		this->bestChoice.resize(this->knapsack.GetItems().size(), false);

		this->BacktrackDirect(0);

		ItemContainerType result;
		for (auto i = 0U; i < this->bestChoice.size(); ++i) {
			if(this->bestChoice[i])
				result.push_back(this->knapsack.GetItems()[i]);
		}

		return result;
	}

private:

	void Init() {
		this->currentPrice = 0;
		this->currentWeight = 0;
		this->bestPrice = 0;

		this->sortedItem.clear();
		this->choice.clear();
		this->bestChoice.clear();
	}

	void BacktrackDirect(size_t depth) {
		if (depth >= this->choice.size()) {
			return;
		}

		const auto &currentItem = this->knapsack.GetItems()[depth];

		if (this->currentWeight + currentItem.weight <= this->knapsack.GetMaxWeight()) {
			// select the current item
			this->currentWeight += currentItem.weight;
			this->currentPrice += currentItem.price; 
			this->choice[depth] = true;

			if (currentPrice > bestPrice) {
				bestPrice = currentPrice;
				copy(this->choice.begin(), this->choice.end(), this->bestChoice.begin());
			}

			// enter next layer
			this->BacktrackDirect(depth + 1);

			this->currentWeight -= currentItem.weight;
			this->currentPrice -= currentItem.price;
			this->choice[depth] = false;
		}

		this->BacktrackDirect(depth + 1);
	}

	PriceType GetPriceUpperBound() {

	}

private:
	PriceType currentPrice{ 0 };
	PriceType bestPrice{ 0 };
	WeightType currentWeight{ 0 };
	ItemContainerType sortedItem;
	vector<bool> choice;
	vector<bool> bestChoice;

private:
	const KnapsackType &knapsack;
};


#endif