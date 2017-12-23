#include "Backtrack.hpp"

#include <iostream>
#include <vector>
#include <type_traits>
#include <iomanip>
#include <memory>

using namespace std;

using WeightType = int;
using PriceType = int;

// a special container which provides element access counter
template<typename Type>
class CounterVector : public vector<Type> {
public:
	using BaseType = vector<Type>;
	using value_type = typename BaseType::value_type;
	using size_type = typename BaseType::size_type;

	CounterVector() {
		this->CreateCounter();
	}
	CounterVector(const initializer_list<value_type> &list) :BaseType{list} {
		this->CreateCounter();
	}
	CounterVector(CounterVector &&other) {
		this->counter = move(other.counter);
		BaseType::operator=(move(other));
	}

	CounterVector &operator = (CounterVector &&other) {
		BaseType::operator=(move(other));
		return *this;
	}

	void ResetCounter() { *counter = 0; }

	int ReadCounter() const { return *counter; }

	value_type &operator[](size_type index) {
		(*counter)++;
		return BaseType::operator[](index);
	}

	const value_type &operator[](size_type index) const {
		(*counter)++;
		return BaseType::operator[](index);
	}

private:
	void CreateCounter() {
		this->counter = make_unique<int>(0);
	}

	unique_ptr<int> counter;
};

using ContainerType = CounterVector<Item<WeightType, PriceType>>;
using KnapsackType = Knapsack<WeightType, PriceType, ContainerType>;
using KnapsackSolverType = KnapsackType::KnapsackSolverType;

const vector<WeightType> weightArray{ 8,7,6,2,10,11,15,12 };
const vector<PriceType> priceArray{ 10,6,8,12,5,9,20,30 };
const WeightType maxWeight = 40;

template<typename WeightArrayType, typename PriceArrayType>
KnapsackType::ItemContainerType GetItemArray(const WeightArrayType &weightArray, const PriceArrayType &priceArray) {
	static_assert(is_same<typename WeightArrayType::value_type, WeightType>::value, "weight type mismatch");
	static_assert(is_same<typename PriceArrayType::value_type, PriceType>::value, "price type mismatch");

	using ItemType = typename KnapsackType::ItemType;
	KnapsackType::ItemContainerType result;

	if (weightArray.size() != priceArray.size())
		throw runtime_error{ "array size mismatch" };

	auto wIter = weightArray.begin();
	auto pIter = priceArray.begin();
	while (wIter != weightArray.end()) {
		ItemType item;
		item.weight = *wIter;
		item.price = *pIter;
		result.emplace_back(move(item));

		++wIter;
		++pIter;
	}

	return result;
}

template<typename ItemContainerType>
void ShowItem(const ItemContainerType &items) {
	auto itemCounter = 1;
	for (const auto &item : items) {
		cout << "item " << (itemCounter++) << " : $" << item.price;
		cout << ", " << item.weight << "kg, $";
		cout << fixed << setprecision(3) << item.GetPriceWeightRatio() << "/kg.\n";
	}
}

int main() {
	auto itemArray = move(GetItemArray(weightArray, priceArray));
	KnapsackType knapsack{ maxWeight };
	knapsack.AssignItems(move(itemArray));
	//knapsack.SortItems();
	ShowItem(knapsack.GetItems());

	cout << "\n";

	KnapsackSolverType solver{ knapsack };
	auto result = move(solver.DirectSolve());
	cout << knapsack.GetItems().ReadCounter() << "\n";
	ShowItem(result);

	system("pause");
	return 0;
}