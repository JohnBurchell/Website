#pragma once
#include "stdafx.h"
#include <iostream>
#include <map>
#include <vector>

namespace Pizza
{
	int manhattan(const int x1, const int y1, const int x2, const int y2)
	{
		return abs(x1 - x2) + abs(y1 - y2);
	}

	int find_best_location(const int x, const int y, int** map, const int rowCount, const int colCount)
	{
		//Store the weights etc here?
		std::map<std::pair<std::pair<int, int>, std::pair<int, int>>, int> calcs;


		int total_count = 0;

		for (int i = 0; i < colCount; ++i)
		{
			for (int j = 0; j < rowCount; ++j)
			{
				std::pair<std::pair<int, int>, std::pair<int, int>> pair{ { x, y },{ j, i } };
				calcs[pair] = manhattan(x, y, j, i) * map[j][i];
			}
		}

		int count = 0;

		for (auto& x : calcs)
		{
			count += x.second;
		}

		return count;
	}

	void pizza()
	{
		//Read in total test cases
		int** map;
		int cases = 0;
		std::cin >> cases;

		std::vector<int> results;

		while (cases != 0)
		{
			int rowCount = 0;
			int colCount = 0;

			std::cin >> colCount;
			std::cin >> rowCount;

			//Init array
			map = new int *[rowCount];
			for (int i = 0; i < rowCount; ++i)
			{
				map[i] = new int[colCount];
			}

			for (int i = 0; i < rowCount; ++i)
			{
				for (int j = 0; j < colCount; ++j)
				{
					std::cin >> map[i][j];
				}
			}

			int min = std::numeric_limits<int>::max();

			for (int i = 0; i < rowCount; ++i)
			{
				for (int j = 0; j < colCount; ++j)
				{
					auto res = find_best_location(j, i, map, rowCount, colCount);
					if (res < min)
					{
						min = res;
					}
				}
			}

			results.push_back(min);

			//Create 2d array of the values -> Simply find the smallest?
			//Same can be done with a simple vector / array
			--cases;
		}

		for (auto& x : results)
		{
			std::cout << x << " blocks\n";
		}
	}
}