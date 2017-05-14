#pragma once
#include <istream>
#include <functional>
#include <random>
#include "InputData.h"
#include "SolutionPath.h"
#include "Helpers.h"

class AnnealingSolver
{
	//std::function<float(float, float, float, float)> distanceFunction;
	InputData<float> inputData;
	SolutionPath<float> currentSolution;
	DefaultRandomEngine randomEngine;
	TimeRestrictor timeRestrictor;
	AnnealingSettings annealingSettings;
public:
	AnnealingSolver(std::istream& input, const AnnealingSettings& settings);

	const SolutionPath<float>& getSolution() const { return currentSolution; }
	SolutionPath<float>& getModifiableSolution() { return currentSolution; }
	void solve();
};

