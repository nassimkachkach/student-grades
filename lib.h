#ifndef LIB_H
#define LIB_H

#include <iostream>
#include <random>
#include <vector>

double calculateMedian(const std::vector<int>& values);
std::mt19937& randomEngine();
int generateRandomScore();
std::size_t generateRandomCount(std::size_t minCount, std::size_t maxCount);

#endif