#include "AnnealingSolver.h"
#include <random>
#include <utility>


AnnealingSolver::AnnealingSolver(std::istream& input, const AnnealingSettings& settings)
	: annealingSettings{settings}
	, inputData{input, settings.distanceFunction}
	, currentSolution{}
	, randomEngine{1, inputData.getVertexAmount()-1}
	, timeRestrictor{inputData.getMaxTime()}
{
	auto vertexAmount = inputData.getVertexAmount();
	currentSolution = constructRandomPath(&inputData, randomEngine, (vertexAmount * 2) / 3);
}

void AnnealingSolver::solve()
{
	auto vertexAmount = inputData.getVertexAmount();
	auto minTemp = annealingSettings.minTemp;
	auto currTemp = annealingSettings.temperature;
	auto tempMod = annealingSettings.tempMultiplyier;
	auto bestSolutionImprovementLimit = annealingSettings.bestSolutionImprovement;
	auto bestSolutionImprovementLimitCounter = annealingSettings.bestSolutionImprovementMaxCounter;

	std::random_device rd;
	std::mt19937 gen{ rd() };
	std::uniform_real_distribution<> urd;
	
	SolutionPath<float> workingSolution = currentSolution;
	timeRestrictor.start();
	
	int loopCounter = 0;
	float lastPathLength = workingSolution.getPathLength();
	while (currTemp > minTemp)
	{
		if (bestSolutionImprovementLimitCounter <= 0)
		{
			for (int counter = 0; counter < annealingSettings.generatingBetterSolutionTries; ++counter)
			{
				// wylosowanie innej drogi
				auto tempWorkingSolution = constructRandomPath(&inputData, randomEngine, (vertexAmount * 2) / 3);
				if (tempWorkingSolution.getPathLength() < currentSolution.getPathLength())
				{
					workingSolution = std::move(tempWorkingSolution);
					break;
				}
			}
			bestSolutionImprovementLimitCounter = annealingSettings.bestSolutionImprovementMaxCounter;
		}

		auto changedIndices = randomSwitch(&inputData, workingSolution, randomEngine);
		auto newPathLength = workingSolution.getPathLength();
		auto oldPathLength = currentSolution.getPathLength();
		if (newPathLength < oldPathLength)
		{
			currentSolution = workingSolution;
			if (std::abs((lastPathLength - newPathLength) / lastPathLength) < bestSolutionImprovementLimit)
				--bestSolutionImprovementLimitCounter;
			lastPathLength = newPathLength;
		}
		else if (urd(gen) < annealingSettings.probabilityFunc(newPathLength, oldPathLength, currTemp))
		{
			currentSolution = workingSolution;
			if (std::abs((lastPathLength - newPathLength) / lastPathLength) < bestSolutionImprovementLimit)
				--bestSolutionImprovementLimitCounter;
			lastPathLength = newPathLength;
		}
		else
		{
			// przywrócenie poprzedniego stanu
			workingSolution.swapElements(&inputData, changedIndices.first, changedIndices.second);
			lastPathLength = oldPathLength;
			--bestSolutionImprovementLimitCounter;
		}

		currTemp = currTemp * tempMod;
		++loopCounter;
		timeRestrictor.check();
		DBPRINT(currTemp);
	}

	DBPRINT(loopCounter);
}