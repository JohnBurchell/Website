#include <iostream>
#include <fstream>
#include <sstream>
#include <random>
#include <assert.h>
#include "path.h"

void TestEdgeCases() {
	unsigned char pMap[] = { 1 }; // Single node
	int pOutBuffer[10];

	int nRes = FindPath(0, 0, 0, 0, pMap, 1, 1, pOutBuffer, 10);
	assert(nRes == 0); // Question of definition

	unsigned char pMap2[] = {
		1, 1, 1,
		1, 1, 1
	};
	int pOutBuffer2[2]; // Too small buffer

	nRes = FindPath(0, 0, 2, 1, pMap2, 3, 2, pOutBuffer2, 2);
	assert(nRes == 3);

	assert(pOutBuffer2[0] == 1 || pOutBuffer2[0] == 3);
	assert(pOutBuffer2[1] == 2 || pOutBuffer2[1] == 4);
}

void TestInvalidLongPath() {
	unsigned char pMap[] = {
		1, 0, 0, 1, 1, 1,
		1, 0, 1, 1, 1, 1,
		1, 0, 0, 1, 1, 0,
		1, 0, 1, 1, 0, 1,
		1, 1, 1, 0, 1, 1,
		0, 0, 0, 0, 0, 1
	};
	int pOutBuffer[100];

	int nRes = FindPath(0, 0, 5, 5, pMap, 6, 6, pOutBuffer, 100);
	assert(nRes == -1);
}

void TestInvalidPath() {
	unsigned char pMap[] = {
		0, 0, 1,
		0, 1, 1,
		1, 0, 1
	};
	int pOutBuffer[7];

	int nRes = FindPath(2, 0, 0, 2, pMap, 3, 3, pOutBuffer, 7);
	assert(nRes == -1);
}

void TestValidLongPath() {
	unsigned char pMap[] = {
		1, 1, 0, 1, 1, 1,
		1, 1, 1, 1, 0, 1,
		1, 1, 0, 1, 0, 1,
		1, 0, 1, 1, 1, 1,
		1, 1, 1, 0, 1, 1,
		0, 0, 0, 0, 1, 1
	};
	int pOutBuffer[100];

	int nRes = FindPath(0, 0, 5, 5, pMap, 6, 6, pOutBuffer, 100);
	assert(nRes == 10);
}

void TestValidOnePath() {
	unsigned char pMap[] = {
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 1, 1, 1, 0, 0,
		0, 0, 0, 0, 0, 1, 0, 1, 0, 0,
		0, 0, 0, 0, 1, 1, 0, 1, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 1, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 1, 0, 0,
		1, 1, 1, 1, 1, 1, 1, 1, 0, 0,
		1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		1, 1, 1, 1, 0, 0, 0, 0, 0, 0,
	};
	int pOutBuffer[100];

	int nRes = FindPath(4, 4, 3, 9, pMap, 10, 10, pOutBuffer, 100);
	assert(nRes == 22);

	assert(pOutBuffer[0] == 45);
	assert(pOutBuffer[9] == 77);
	assert(pOutBuffer[21] == 93);
}

void TestValidPath() {
	unsigned char pMap[] = {
		1, 1, 1, 1,
		0, 1, 0, 1,
		0, 1, 1, 1
	};
	int pOutBuffer[12];

	int nRes = FindPath(0, 0, 1, 2, pMap, 4, 3, pOutBuffer, 12);
	assert(nRes == 3);

	assert(pOutBuffer[0] == 1);
	assert(pOutBuffer[1] == 5);
	assert(pOutBuffer[2] == 9);
}

void print_map(const unsigned char* pmap, const int width, const int height, int target_y, int target_x, int start_x, int start_y)
{
	std::cout << "\n=====================================================\n";

	for (int i = 0; i != height; ++i)
	{
		for (int j = 0; j != width; ++j)
		{
			if (j == target_y && i == target_x)
			{
				std::cout << "T";
			}
			else if (j == start_y && i == start_x)
			{
				std::cout << "S";
			}
			else
			{
				std::cout << (pmap[j + (i * width)] == 1 ? 1 : 0);
			}
		}

		std::cout << "\n";
	}
}

bool in_buffer(int* buffer, const int val, const int size)
{
	for (int i = 0; i < size; ++i)
	{
		if (buffer[i] == val)
		{
			return true;
		}
	}

	return false;
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
		1,1,1,1,1,1,1,1,1,1,
		0,0,0,0,0,0,0,0,0,1,
		1,1,1,1,1,1,1,1,1,1,
		1,0,0,0,0,0,0,0,0,0,
		1,1,1,1,1,1,1,1,1,1
	};
	int pOutBuffer3[50];
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

	unsigned char cross[] = {
		1, 1, 0, 1, 0,
		1, 1, 0, 0, 1,
		0, 1, 0, 0, 0,
		1, 1, 0, 1, 1,
		0, 1, 1, 1, 1
	};
	int cross_out[10];

	unsigned char pattern_one[] = {
		1, 1, 1, 1, 1, 1, 1,
		1, 0, 0, 0, 0, 0, 1, 
		1, 0, 1, 1, 1, 0, 1, 
		1, 0, 1, 0, 1, 0, 1, 
		1, 0, 1, 0, 0, 0, 1, 
		1, 0, 1, 1, 1, 1, 1, 
		1, 0, 0, 0, 0, 0, 0
	};
	int pattern_one_out[50];



	//unsigned char huge_map_500[500*500];
	//load_map("500_500.txt", huge_map_500);
	//int pOutBuffer_500[2000];

	//unsigned char huge_map_750[750 * 750];
	//load_map("750_750.txt", huge_map_750);
	//int pOutBuffer_750[2000];

	//unsigned char huge_map_1000[1000 * 1000];
	//load_map("1000_1000.txt", huge_map_1000);
	//int pOutBuffer_1000[2000];

	//auto res_500  = FindPath(10, 10, 24, 24, huge_map_500, 25, 25, pOutBuffer_500, 2000);
	//auto res_750  = FindPath(0, 0, 749, 749, huge_map_750, 750, 750, pOutBuffer_750, 3000);
	//auto res_1000 = FindPath(0, 0, 999, 999, huge_map_1000, 1000, 1000, pOutBuffer_1000, 2000);

	constexpr int width = 500;
	constexpr int height = 500;

	int size = width * height;

	int pOutBuffer_X[10000];

	unsigned char map_X[width * height];

	std::fill_n(map_X, size, 0x01);

	std::random_device rd;     // only used once to initialise (seed) engine
	std::mt19937 rng(rd());    // random-number engine used (Mersenne-Twister in this case)
	std::uniform_int_distribution<int> uni(0, height - 1); // guaranteed unbiased

	auto rand_x = uni(rng);
	auto rand_y = uni(rng);
	auto target_x = uni(rng);
	auto target_y = uni(rng);

	auto res_x = FindPath(0, 0, width - 1, height - 1, map_X, width, height, pOutBuffer_X, 10000);

	//for (int i = 0; i < 10; ++i)
	//{
	//	rand_x = uni(rng);
	//	rand_y = uni(rng);
	//	target_x = uni(rng);
	//	target_y = uni(rng);

	//	res_x = FindPath(rand_x, rand_y, target_x, target_y, map_X, width, height, pOutBuffer_X, 10000);

	//	std::cout << "Rand " << rand_x << " " << rand_y << " Target >> " << target_x << " " << target_y;

	//	if (res_x != -1)
	//	{
	//		std::cout << " Found!" << std::endl;
	//	}
	//	else
	//	{
	//		std::cout << " No path!" << std::endl;
	//	}
	//}

	unsigned char forum_map[] = {
		0,0,0,0,0,1,0,0,0,
		0,0,0,0,0,1,0,0,0,
		0,1,1,1,1,1,1,1,0,
		0,1,0,0,0,0,0,1,0,
		0,1,1,1,1,1,1,1,0,
		0,0,0,0,0,0,0,0,0
	};

	unsigned char forum_map_mirrored[] = {
		0, 0, 0, 1, 0, 0, 0, 0, 0,
		0, 0, 0, 1, 0, 0, 0, 0, 0,
		0, 1, 1, 1, 1, 1, 1, 1, 0,
		0, 1, 0, 0, 0, 0, 0, 1, 0,
		0, 1, 1, 1, 1, 1, 1, 1, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0
	};	

	int forum_normal[20];
	int forum_mirrored[20];

	auto forum_normal_res = FindPath(4, 4, 5, 0, forum_map, 9, 6, forum_normal, 20);
	auto forum_mirrrored_res = FindPath(4, 4, 3, 0, forum_map_mirrored, 9, 6, forum_mirrored, 20);

	auto cross_res = FindPath(0, 0, 4, 4, cross, 5, 5, cross_out, 20);
	auto pattern_res = FindPath(0, 6, 4, 3, pattern_one, 7, 7, pattern_one_out, 50);

	unsigned char turn_test[] =
	{
		1,1,1,1,1,0,1,1,
		0,1,0,0,1,0,1,0,
		1,1,0,1,1,0,1,1,
		1,0,0,1,0,0,0,1,
		1,1,0,1,1,0,1,1,
		0,1,0,0,1,0,1,0,
		1,1,0,1,1,0,1,1,
		1,0,0,1,0,0,0,1,
		1,1,0,1,1,1,1,1
	};
	int turn_test_buffer[50];
	auto turn_test_res = FindPath(3, 4, 1, 8, turn_test, 8, 9, turn_test_buffer, 50);

	unsigned char fail_test[] =
	{
		0,1,1,1,0,
		0,1,0,1,0,
		0,0,1,1,0,
		1,1,1,0,1
	};

	int shit_buffer[10];
	int shit_buffer2[10];
	auto fail_test_2 = FindPath(0, 3, 1, 1, fail_test, 5, 4, shit_buffer2, 10);
	auto fail_test_1 = FindPath(0, 3, 1, 1, fail_test, 5, 4, shit_buffer, 5);

	TestEdgeCases();
	TestInvalidLongPath();
	TestInvalidPath();
	TestValidLongPath();
	TestValidOnePath();
	TestValidPath();

	unsigned char same_length_test[] = {
		1, 1, 0, 1, 1, 1,
		1, 1, 1, 1, 0, 1,
		1, 1, 0, 1, 0, 1,
		1, 0, 1, 1, 1, 1,
		1, 1, 1, 0, 1, 1,
		0, 0, 0, 0, 1, 1
	};
	int same_length_buffer[100];
	int same_length_buffer_two[100];
	int same_length_buffer_three[100];
	int same_length_buffer_four[100];

	auto res6 = FindPath(0, 0, 1, 2, pMap5, 4, 3, pOutBuffer5, 12);
	assert(res6 == 3);

	auto res7 = FindPath(3, 2, 0, 0, pMap7, 4, 3, pOutBuffer7, 12);
	assert(res7 == 5);

	auto res8 = FindPath(2, 0, 0, 2, pMap2, 3, 3, pOutBuffer2, 12);
	assert(res8 == -1);

	auto res9 = FindPath(9, 4, 0, 0, pMap3, 10, 5, pOutBuffer3, 5);
	assert(res9 == 31);


	int same_length_res = FindPath(0, 0, 5, 5, same_length_test, 6, 6, same_length_buffer, 100);
	int same_length_res_opposite = FindPath(5, 5, 0, 0, same_length_test, 6, 6, same_length_buffer_two, 100);

	assert(same_length_res == same_length_res_opposite);

	unsigned char git_test[] =
	{
		1,1,1,1,1,0,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
		1,1,1,1,1,0,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
		1,0,0,0,1,0,1,1,1,1,0,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
		1,1,1,1,1,0,1,1,1,1,0,1,1,1,0,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,
		1,1,0,0,0,0,1,1,1,1,0,1,1,1,0,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
		1,1,1,1,1,1,1,1,1,1,0,1,1,1,0,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, // 4
		0,0,0,0,0,0,0,0,1,1,0,1,1,1,0,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
		1,1,1,1,1,1,1,1,1,1,0,1,1,1,0,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
		1,1,1,1,0,0,0,0,0,0,0,0,0,1,0,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
		1,1,1,1,1,1,1,1,1,1,1,1,0,1,0,1,1,1,1,1,0,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,1,1,
		0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,1,1,1,1,0,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1, // 10
		1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,0,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,
		1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,1,1,1,1,1,0,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,
		1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,
		1,1,1,1,1,1,0,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,
		1,1,1,1,1,1,0,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1, // 15
		1,1,1,1,1,1,0,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,
		1,1,1,1,1,1,0,1,1,1,1,1,0,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,
		1,1,1,1,1,1,0,1,1,1,1,1,0,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,
		1,1,1,1,1,1,0,1,1,1,1,1,0,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,
		1,1,1,1,1,1,0,1,1,1,1,1,0,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1, // 20
		1,1,1,1,1,1,0,1,1,1,1,1,0,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,
		1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
		1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
		1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 // 24
	};
	print_map(git_test, 39, 25, 21, 20, 0, 0);
	int git_buffer[200];
	int git_buffer_2[200];

	auto git_buffer_res = FindPath(0, 0, 20, 20, git_test, 39, 25, git_buffer, 200);
	auto git_buffer_res_1 = FindPath(20, 20, 0, 0, git_test, 39, 25, git_buffer_2, 200);
	
	for (int i = 0; i != 25; ++i)
	{
		for (int j = 0; j != 39; ++j)
		{
			if (in_buffer(git_buffer, (j + (i * 25)), git_buffer_res))
			{
				std::cout << "P";
			}
			else
			{
				std::cout << (git_test[j + (i * width)] == 1 ? 1 : 0);
			}
		}

		std::cout << "\n";
	}

	//Shouldn't the paths be the same -> According to the online version, no
	/*
		1,0
		2,0
		3,0
		4,0
		5,0
		5,1
		5,2
		5,3
		5,4
		5,5
		5,6
		5,7
		5,8
		4,8
		3,8
		2,8
		1,8
		1,9
		1,10
		2,10
		3,10
		4,10
		5,10
		6,10
		7,10
		7,11
		7,12
		8,12
		9,12
		10,12
		11,12
		10,12
		9,12
		8,12
		7,12
		6,12
		5,12
		5,13
		TILL
		5,21
		6,21
		7,21
		8,21
		8,20
		TILL
		8,13
		TILL
		15,13
		TILL
		15,2
		TILL
		34,2
		TILL
		34,8
		35,8
		36,8
		37,8
		38,8
		TILL
		38,21
		TILL
		17,21
		TILL
		17,16
		18,16
		19,16


	
	*/

	int short_test_buffer_north[15];
	int short_test_buffer_east[15];
	int short_test_buffer_south[15];
	int short_test_buffer_west[15];

	unsigned char short_test[] =
	{
		1,1,1,1,1,
		1,0,0,0,1,
		1,0,0,0,1,
		1,0,0,0,1,
		1,0,0,0,1,
		1,0,0,0,1,
		1,0,0,1,1,
		1,0,0,1,0,
		1,0,0,1,1,
		1,0,0,0,1,
		1,1,1,1,1
	};

	//5 width 10
	auto final_test_north = FindPath(2, 0, 2, 10, short_test, 5, 11, short_test_buffer_north, 50);
	auto final_test_east = FindPath(4, 5, 0, 5, short_test, 5, 11, short_test_buffer_east, 50);
	auto final_test_south = FindPath(2, 10, 2, 0, short_test, 5, 11, short_test_buffer_south, 50);
	auto final_test_west = FindPath(0, 5, 4, 5, short_test, 5, 11, short_test_buffer_west, 50);
	assert(final_test_north == 14);
	assert(final_test_south == 14);
	assert(final_test_east == 14);
	assert(final_test_west == 14);

	const unsigned char filkry_map[] = {
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, //starts at 0
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, //10
		1, 1, 1, 0, 0, 0, 0, 1, 1, 1, //20
		1, 1, 1, 0, 1, 1, 0, 1, 1, 1, //30
		1, 1, 1, 0, 1, 1, 0, 1, 1, 1, //40
		1, 1, 1, 0, 1, 1, 0, 1, 1, 1, //50
		1, 1, 1, 0, 1, 1, 0, 1, 1, 1, //60
		1, 1, 1, 0, 1, 0, 0, 1, 1, 1, //70
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, //80
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, //90
	};

	int filkry_buffer_one[20];
	int filkry_buffer_two[20];

	//Should be 11
	auto filkry_res = FindPath(4, 8, 4, 1, filkry_map, 10, 10, filkry_buffer_one, 20);

	//should be 17 (Mine takes the wrong route -- again)
	auto f_2 = FindPath(5, 3, 4, 1, filkry_map, 10, 10, filkry_buffer_two, 20);

	return 0;
}




