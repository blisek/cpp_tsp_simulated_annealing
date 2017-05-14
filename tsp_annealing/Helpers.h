#pragma once
#include <random>
#include <stdexcept>
#include <ctime>
#include <functional>
#include <utility>
#include <iostream>
#include "SolutionPath.h"

#ifdef _DEBUG
#define DBPRINT(x) { std::cout << #x " = " << x << std::endl; }
#else
#define DBPRINT(x) {}
#endif

class DefaultRandomEngine
{
	std::random_device rd;
	std::mt19937 gen;
	std::uniform_int_distribution<int> dis;
public:
	DefaultRandomEngine(int minValue, int maxValue)
		: rd{}, gen{ rd() }, dis{ minValue, maxValue }
	{}

	int operator()() { return dis(gen); }
};

template<typename T, typename DistFunc, typename RandomNumberGenerator>
SolutionPath<T> constructRandomPath(const InputData<T, DistFunc>* inputData, RandomNumberGenerator& numGenerator, int numOfSwaps)
{
	if (numOfSwaps <= 0)
		throw std::runtime_error{ "numOfSwaps must be greater than 0!" };
	SolutionPath<T> solPath{ true, inputData };
	for (int i = 0; i < numOfSwaps; ++i)
	{
		auto firstIndex = numGenerator();
		auto secondIndex = numGenerator();
		if (secondIndex == firstIndex)
			secondIndex = (secondIndex + 1) % inputData->getVertexAmount();
		solPath.swapElements(inputData, firstIndex, secondIndex);
	}

	return solPath;
}

template<typename T, typename S, typename RandomNumberGenerator>
inline std::pair<int,int> randomSwitch(const InputData<S,T>* inputData, SolutionPath<T, S>& sol, RandomNumberGenerator& rg)
{
	auto firstIndex = rg();
	auto secondIndex = rg();

	if (secondIndex == firstIndex)
		secondIndex = (secondIndex > 1) ? secondIndex - 1 : secondIndex + 1;

	sol.swapElements(inputData, firstIndex, secondIndex);
	return std::make_pair(firstIndex, secondIndex);
}

class timeout_error : public std::runtime_error
{
public:
	timeout_error(const char* msg) : std::runtime_error{ msg } {}
};

class TimeRestrictor
{
	std::time_t startTime;
	int limit;
	bool started{ false };
public:
	TimeRestrictor(int secondsLimit)
		: limit{ secondsLimit }
	{
		if (limit <= 0)
			throw new std::runtime_error{ "Seconds limit must be greater than 0!" };
	}
	void start()
	{
		if (started)
			return;

		startTime = std::time(nullptr);
		started = true;
	}

	void stop()
	{
		started = false;
	}

	void check()
	{
		if (!started)
			return;

		auto secSinceStart = std::difftime(std::time(nullptr), startTime);
		if (secSinceStart >= limit)
			throw timeout_error{ "time limit" };
	}
};

struct AnnealingSettings
{
	std::function<float(float, float, float, float)> distanceFunction;
	std::function<float(float, float, float)> probabilityFunc;
	//std::function<float(int)> tempDecreaseFunc;
	float minTemp;
	float temperature;
	float tempMultiplyier;
	float bestSolutionImprovement;
	int bestSolutionImprovementMaxCounter;
	int generatingBetterSolutionTries;
};