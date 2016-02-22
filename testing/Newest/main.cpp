#include <vector>
#include <tuple>
#include <queue>
#include <unordered_map>
#include <map>
#include <string>
#include <functional>
#include <iostream>
#include <fstream>
#include <sstream>

#include "path.h"


/*

Simple A* implementation.

Tried to get JPS working but failed, perhaps this will be sufficient to pass the tests!

Ways to optimise:

Pre-allocate whole block of memory to prevent further allocations
Use an optimisation of A* (I've tried JPS but it's not that suitable for only 4 directions)
RSR might work?

*/

void print_map(const unsigned char* pmap, const int width, const int height)
{
	std::cout << "\n=====================================================\n";

	for (int i = 0; i != height; ++i)
	{
		for (int j = 0; j != width; ++j)
		{
			std::cout << (pmap[j + (i * width)] == 1 ? 1 : 0);
		}

		std::cout << "\n";
	}

	std::cout << "\n=====================================================\n";
}

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
	std::stringstream ss(s);
	std::string item;
	while (std::getline(ss, item, delim)) {
		elems.push_back(item);
	}
	return elems;
}


std::vector<std::string> split(const std::string &s, char delim) {
	std::vector<std::string> elems;
	split(s, delim, elems);
	return elems;
}

void load_map(const char* name, unsigned char* pMap)
{
	std::ifstream map_file{};
	map_file.open(name);
	std::string line;

	unsigned char* p_pmap = pMap;
	int count = 0;

	while (std::getline(map_file, line))
	{
		auto test = true;

		for (unsigned char x : line)
		{
			*p_pmap = '\x1';
			++p_pmap;
			++count;
		}
	}
}

int main()
{
	unsigned char pMap[] = {
		1, 1, 1, 1,
		0, 1, 0, 1,
		0, 1, 1, 1
	};
	int pOutBuffer[12];
	//auto res = FindPath(0, 0, 1, 2, pMap, 4, 3, pOutBuffer, 12);

	unsigned char pMap2[] = {
		0, 0, 1,
		0, 1, 1,
		1, 0, 1 };
	int pOutBuffer2[7];
	//auto res2 = FindPath(2, 0, 0, 2, pMap2, 3, 3, pOutBuffer2, 7);

	unsigned char pMap3[] = {
		1,1,1,1,1,0,0,0,0,1,
		0,0,0,0,1,0,0,0,0,1,
		0,0,0,0,1,1,1,1,1,1,
		1,1,1,1,1,0,0,0,0,1,
		1,0,0,0,1,1,1,1,1,1
	};
	int pOutBuffer3[20];
	//auto res3 = FindPath(0, 0, 9, 4, pMap3, 10, 5, pOutBuffer3, 20);

	unsigned char pMap5[] = {
		1, 1, 1, 1,
		0, 1, 0, 1,
		0, 1, 1, 1 };

	int pOutBuffer5[12];
	//auto res5 = FindPath(0, 0, 1, 2, pMap5, 4, 3, pOutBuffer5, 12);

	unsigned char pMap7[] = {
		1, 1, 1, 1,
		1, 1, 0, 1,
		1, 1, 0, 1
	};
	int pOutBuffer7[12];

	//unsigned char pMap4[] = {

	//};

	unsigned char huge_map[500*500];
	load_map("500_500.txt", huge_map);
	int pOutBuffer4[1000];

	auto res4 = FindPath(0, 0, 499, 499, huge_map, 500, 500, pOutBuffer4, 1000);

	auto res6 = FindPath(0, 0, 1, 2, pMap5, 4, 3, pOutBuffer5, 12);
	auto res7 = FindPath(0, 0, 3, 2, pMap7, 4, 3, pOutBuffer7, 12);
	auto res8 = FindPath(2, 0, 0, 2, pMap2, 3, 3, pOutBuffer2, 12);
	auto res9 = FindPath(0, 0, 9, 4, pMap3, 10, 5, pOutBuffer3, 20);

	return 0;
}
