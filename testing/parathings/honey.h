#pragma once
#include <vector>
#include <iostream>

namespace Honey
{
	using std::cin;
	using std::cout;

	void honeycomb_walk()
	{
		//Should all fit in an int
		int all_steps[16][31][31]{ 0 };
		all_steps[0][15][15] = 1;

		//Instead of calculating everything on the fly per request, do upfront computation and
		//cache the reults -> We know we'll only ever need to check <= 14
		for (int i = 1; i < 16; ++i) {
			for (int j = 1; j < 31; ++j) {
				for (int k = 1; k < 31; ++k)
				{
					//Calculate the total step count of each step 
					all_steps[i][j][k] =
						all_steps[i - 1][j][k + 1] +
						all_steps[i - 1][j + 1][k] +
						all_steps[i - 1][j][k - 1] +
						all_steps[i - 1][j - 1][k] +
						all_steps[i - 1][j + 1][k + 1] +
						all_steps[i - 1][j - 1][k - 1];
				}
			}
		}

		int count = 0;
		int target = 0;
		cin >> count;
		std::vector<int> counts{};

		while (count != 0)
		{
			cin >> target;
			counts.push_back(target);
			--count;
		}

		for (int x : counts)
		{
			std::cout << all_steps[x][15][15] << "\n";
		}
	}
}