#include "utils.h"

std::vector<int> raffle(int lower_bound, int upper_bound, int amount )
{
	if(amount == 1)
		return raffle1(lower_bound, upper_bound);

	else if(amount == 2)
		return raffle2(lower_bound, upper_bound);

	std::vector<int> values;
	for (int i = lower_bound; i < upper_bound; i++)
		values.push_back(i);
	std::random_shuffle(values.begin(), values.end());

	std::vector<int> drawn;
	for (int i=0; i < amount; i++)
		drawn.push_back(values[i]);
	return drawn;
}

std::vector<int> raffle1(int lower_bound, int upper_bound )
{
	std::vector<int> res;
	res.push_back(lower_bound + rand()%(upper_bound - lower_bound));
	return res;
}

std::vector<int> raffle2(int lower_bound, int upper_bound )
{
	int n = upper_bound-lower_bound;

	int index_first = rand() % (n-1);
	int index_second = rand() % n;

	if(index_second == index_first)
		index_second = n-1;

	std::vector<int> res;
	res.push_back(index_first + lower_bound);
	res.push_back(index_second + lower_bound);

	return res;
}