#ifndef UTILS_H
#define UTILS_H

#include <algorithm> // random_shuffle()
#include <cstdlib> // rand()
#include <vector>

std::vector<int> raffle(int lower_bound, int upper_bound, int amount );

std::vector<int> raffle1(int lower_bound, int upper_bound );

std::vector<int> raffle2(int lower_bound, int upper_bound );

#endif