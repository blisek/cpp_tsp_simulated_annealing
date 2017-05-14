#include <iostream>
#include <cstdlib>
#include <cmath>
#include <stdexcept>
#include <utility>
#include <memory>
#include <algorithm>
#include "InputData.h"
#include "SolutionPath.h"
#include "Helpers.h"
#include "AnnealingSolver.h"

std::unique_ptr<AnnealingSolver> solver;

template<typename T1, typename T2> std::ostream& operator<<(std::ostream&, const std::pair<T1, T2>&);
void printSolution(SolutionPath<float, float>& sol);
template<typename T1, typename T2> void printPathStartingWithSpecifiedVertex(std::ostream&, int startVertex, SolutionPath<T1, T2>& solution);
void printOutput();

int main(int argc, char **argv)
try
{
	auto euclidean_distance = [](float x1, float y1, float x2, float y2) -> float {
		double xDist = x1 - x2; double yDist = y1 - y2;
		return static_cast<float>(std::sqrt(xDist * xDist + yDist * yDist));
	};

	auto probabilityFunction = [](float newLength, float oldLength, float currentTemp) -> float {
		return std::exp(-((newLength - oldLength) / currentTemp));
	};

	AnnealingSettings settings{ 
		euclidean_distance, // distance function
		probabilityFunction, // probability function
		1e-30f, // min temp
		1e+16f, // t0
		.97f, // temp modifier
		0.05f, // best solution improvement
		10, // best solution improvement counter
		5
	};

	solver.reset(new AnnealingSolver(std::cin, settings));
	solver->solve();

	// print output
	printOutput();

	std::cout << std::endl;
	return EXIT_SUCCESS;
}
catch (timeout_error te)
{
	if (solver)
		printOutput();
	return EXIT_SUCCESS;
}
catch (std::runtime_error re)
{
	std::cerr << "Wystapil blad krytyczny w czasie wykonywania programu: " << re.what()
		<< std::endl << "Zamykanie programu..." << std::endl;
	return EXIT_FAILURE;
}
catch (...)
{
	std::cerr << "Wystapil blad krytyczny w czasie wykonywania programu! Program zostanie zamkniêty." << std::endl;
	return EXIT_FAILURE;
}


template<typename T1, typename T2>
std::ostream& operator<<(std::ostream& os, const std::pair<T1, T2>& p)
{
	return os << '[' << p.first << ',' << ' ' << p.second << ']';
}

void printSolution(SolutionPath<float, float>& sol)
{
	for(auto nv : sol)
		std::cout << nv << '\n';
}

template<class T1, class T2>
void printPathStartingWithSpecifiedVertex(std::ostream& os, int startVertex, SolutionPath<T1, T2>& solution)
{
	auto vertexPosition = std::find_if(solution.begin(),
		solution.end(), [startVertex](NextVertex<T2> v) { return v.vertex == startVertex; });
	auto itEnd = solution.end();
	auto vertexPositionCopy = vertexPosition;

	for (; vertexPosition != itEnd; ++vertexPosition)
		os << vertexPosition->vertex << '\n';
	for (auto it = solution.begin(); it != vertexPositionCopy; ++it)
		os << it->vertex << '\n';
}

void printOutput()
{
	std::cout << solver->getSolution().getPathLength() << std::endl;
	printPathStartingWithSpecifiedVertex(std::cerr, 1, solver->getModifiableSolution());
}