#pragma once
#include <vector>
#include <stdexcept>
#include <ostream>
#include <algorithm>
#include <utility>
#include <tuple>
#include "InputData.h"

template<typename T>
struct NextVertex
{
	int vertex;
	//int previousIndex;
	//int nextIndex;
	T distance;
};

template<typename T>
std::ostream& operator<<(std::ostream& os, const NextVertex<T>& nv)
{
	return os << "(V:" << nv.vertex /*<< ", PI:" << nv.previousIndex << ", NI:" << nv.nextIndex */<< ", D:" << nv.distance << ')';
}

template<typename T, typename S = T>
class SolutionPath
{
	std::vector<NextVertex<S>> path;
	mutable S calculatedPath{};
	mutable bool pathChanged{ false };
	void generateSimplePath(const InputData<T, S>*);
	inline void updateDistance(const InputData<T, S>*, NextVertex<S>& element, int index, bool updatePath = false);
	std::pair<int, int> getNearestIndexes(int index) const
	{
		return std::make_pair(
			(index == 0) ? path.size() - 1 : index - 1,
			(index + 1) % path.size()
		);
	}
public:
	SolutionPath() : SolutionPath{ false, nullptr } {}
	SolutionPath(bool generateStartPath, const InputData<T, S>* inputData = nullptr)
	{
		if (generateStartPath && !inputData)
			throw std::runtime_error{ "InputData pointer is null!" };

		if (generateStartPath)
		{
			path.resize(inputData->getVertexAmount());
			generateSimplePath(inputData);
		}
	}

	S getPathLength() const;
	auto begin(bool modified = false) -> decltype(path.begin())
	{
		if (modified)
			pathChanged = true;
		return path.begin();
	}
	auto end(bool modified = false) -> decltype(path.end())
	{
		if (modified)
			pathChanged = true;
		return path.end();
	}
	auto cbegin() -> decltype(path.cbegin()) const { return path.cbegin(); }
	auto cend() -> decltype(path.cend()) const { return path.cend(); }
	auto rbegin() -> decltype(path.rbegin()) { return path.rbegin(); }
	auto rend() -> decltype(path.rend()) { return path.rend(); }

	void swapElements(const InputData<T, S>* iData, int index1, int index2)
	{
		auto& element1 = path[index1];
		auto& element2 = path[index2];
		std::swap(element1, element2);
		updateDistance(iData, element1, index1, true);
		updateDistance(iData, element2, index2, true);
	}

	S getElement(int index) const { return path[index]; }
	S getLastElement() const { return path[path.size() - 1]; }
	S getFirstElement() const { return path[0]; }
};

template<typename T, typename S>
void SolutionPath<T, S>::generateSimplePath(const InputData<T, S>* inputData)
{
	auto vertexAmount = inputData->getVertexAmount();
	for (int i = 0; i < vertexAmount; ++i)
	{
		auto nextIndex = (i+1) % vertexAmount;
		auto previousIndex = (i == 0) ? vertexAmount - 1 : i - 1;
		path[i] = { i+1, inputData->getDistanceBetweenVertices(i+1, nextIndex + 1) };
	}
	pathChanged = true;
}

template<typename T, typename S>
S SolutionPath<T, S>::getPathLength() const
{
	if (pathChanged)
	{
		S sum{};
		for (auto nv : path)
			sum += nv.distance;
		calculatedPath = sum;
		pathChanged = false;
	}

	return calculatedPath;
}

template<typename T, typename S>
inline void SolutionPath<T, S>::updateDistance(const InputData<T, S>* inputData, NextVertex<S>& element, int index, bool updatePath)
{
	int previousIndex, nextIndex;
	std::tie(previousIndex, nextIndex) = getNearestIndexes(index);

	auto& previousElement = path[previousIndex];
	auto& nextElement = path[nextIndex];

	auto previousElementDiff = -previousElement.distance;
	previousElementDiff += (previousElement.distance = inputData->getDistanceBetweenVertices(previousElement.vertex, element.vertex));

	auto nextElementDiff = -element.distance;
	nextElementDiff += (element.distance = inputData->getDistanceBetweenVertices(nextElement.vertex, element.vertex));

	if (updatePath && !pathChanged)
	{
		calculatedPath += previousElementDiff;
		calculatedPath += nextElementDiff;
	}
	else
		pathChanged = true;
}
