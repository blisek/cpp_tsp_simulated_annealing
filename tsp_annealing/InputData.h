#pragma once
#include <istream>
#include <functional>
#include <stdexcept>
#include <cstdio>
#include <string>
#include <memory>
#include <utility>

template<typename T, typename S = T>
class InputData final
{
	using CoordType = T;
	using DistanceFunctionReturnType = S;
	int vertexAmount;
	int maxTime;
	std::unique_ptr<std::pair<CoordType, CoordType>[]> coordinates{};
	std::function<DistanceFunctionReturnType(CoordType, CoordType, CoordType, CoordType)> distanceFunc;
public:
	InputData(std::istream& inputStream, std::function<DistanceFunctionReturnType(CoordType, CoordType, CoordType, CoordType)> distanceFunction)
		: distanceFunc{distanceFunction}
	{
		if (!inputStream)
			throw std::runtime_error{ "Input stream has invalid state!" };

		int vertexAmount;
		inputStream >> vertexAmount;
		
		if (vertexAmount <= 0)
			throw std::runtime_error{ "Invalid vertices amount!" };
		
		std::unique_ptr<std::pair<CoordType, CoordType>[]> coords(new std::pair<CoordType, CoordType>[vertexAmount]);

		int lineCounter;
		for (lineCounter = 0; inputStream && lineCounter < vertexAmount; ++lineCounter)
		{
			int v; 
			CoordType x, y;

			if (inputStream.eof())
				break;
			inputStream >> v >> x >> y;

			coords[v - 1].first = static_cast<CoordType>(x);
			coords[v - 1].second = static_cast<CoordType>(y);
		}

		if (lineCounter < vertexAmount)
			throw std::runtime_error{ "Invalid line amount - less than declared in first line." };

		int maxTime;
		inputStream >> maxTime;
		if (maxTime <= 0)
			throw std::runtime_error{ "Invalid max time - le than zero." };
		
		coordinates.swap(coords);
		this->vertexAmount = vertexAmount;
		this->maxTime = maxTime;
	}

	InputData(const InputData&) = delete;
	InputData(InputData&&) = default;
	~InputData() = default;

	CoordType vertexX(int vertex) const noexcept
	{
		return coordinates[vertex-1].first;
	}

	void vertexX(int vertex, CoordType val) noexcept
	{
		coordinates[vertex-1].first = val;
	}

	CoordType vertexY(int vertex) const noexcept
	{
		return coordinates[vertex-1].second;
	}

	void vertexY(int vertex, CoordType val) noexcept
	{
		coordinates[vertex-1].second = val;
	}

	std::pair<CoordType, CoordType> getVertexCoordinates(int vertex) const noexcept
	{
		return coordinates[vertex-1];
	}

	DistanceFunctionReturnType getDistanceBetweenVertices(int vertex1, int vertex2) const
	{
		auto v1 = coordinates[vertex1-1];
		auto v2 = coordinates[vertex2-1];

		return distanceFunc(
			// vertex1 x & y
			v1.first, v1.second,
			// vertex2 x & y
			v2.first, v2.second
		);
	}

	int getVertexAmount() const noexcept { return vertexAmount; }
	int getMaxTime() const noexcept { return maxTime; }
};

