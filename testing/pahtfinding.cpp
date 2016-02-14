#include "stdafx.h"
#include <vector>
#include <tuple>
#include <queue>
#include <unordered_map>
#include <string>

//Pre-allocate whole block for the pathfinder at the start -> based on amount of nodes? + a bit extra? Prevents allocations during
//The running of the search.


int FindPath(const int nStartX, const int nStartY,
	const int nTargetX, const int nTargetY,
	const unsigned char* pMap, const int nMapWidth, const int nMapHeight,
	int* pOutBuffer, const int nOutBufferSize);

struct Node
{
	Node() : m_x(-1), m_y(-1) {}
	Node(const int x, const int y) : m_x(x), m_y(y) {}
	int m_x;
	int m_y;

	bool operator==(const Node& other) const
	{
		return (m_x == other.m_x
			 && m_y == other.m_y);
	}
};

struct NodeHasher
{
	std::size_t operator()(const Node& node) const
	{
		return ((std::hash<int>()(node.m_x) ^ std::hash<int>()(node.m_y << 1)));
	}
};

struct Graph
{
	Graph(const int width, const int height) : m_width(width), m_height(height), neighbour_positions(), m_pmap_size(height * width)
	{
										//N / E / S / W
		 neighbour_positions = { Node{ 0, -1 }, Node{ 1, 0 }, Node{ 0, 1 }, Node{ -1, 0 } };
	}

	int get_index_value(const int x, const int y) const
	{
		return x + (y * m_width);
	}

	//Get the neighbours of a given node in the map
	std::vector<Node> neighbours(const int x, const int y, const unsigned char* pMap)
	{
		std::vector<Node> return_vector{};

		for (auto& pos : neighbour_positions)
		{
			int new_x = x + pos.m_x;
			int new_y = y + pos.m_y;

			//If it's in bounds
			if ((new_x >= 0 && new_x <= m_width && new_y >= 0 && new_y <= m_height))
			{
				//Convert the combined position to get the real index value from pMap
				int real_index = get_index_value(new_x, new_y);

				//If the neighbour is valid
				if (real_index <= m_pmap_size && pMap[real_index] == 1)
				{
					Node neighbour{ new_x, new_y };
					return_vector.emplace_back(neighbour);
				}
			}
		}

		return return_vector;
	}

	//BFS
	int BFS(const int start_x, const int start_y, const int target_x ,const int target_y, 
		const unsigned char* pMap, int* pOutBuffer)
	{
		auto frontier = std::queue<Node>();
		Node start{ start_x, start_y };
		Node target{ target_x, target_y };
		bool target_found = false;
		frontier.push(start);

		node_paths[start] = start;

		Node current = start;

		while (!frontier.empty()) 
		{
			current = frontier.front();
			frontier.pop();

			auto nn = neighbours(current.m_x, current.m_y, pMap);

			if (current == target)
			{
				target_found = true;
				break;
			}

			for (auto& next : neighbours(current.m_x, current.m_y, pMap))
			{
				if (!node_paths.count(next))
				{
					frontier.push(next);
					node_paths[next] = current;
				}
			}
		}

		std::vector<int> indecies{};

		while (!(current == start))
		{
			indecies.push_back(get_index_value(current.m_x, current.m_y));
			current = node_paths[current];
		}

		for (int i = indecies.size() - 1; i != -1; --i)
		{
			*pOutBuffer = indecies[i];
			++pOutBuffer;
		}

		return target_found ? indecies.size() : -1;
	}

	//Dijkstra

	//A*

	//A* with JPS

	//A* with JRA

private:
	int m_width;
	int m_height;
	int m_pmap_size;
	std::vector<Node> neighbour_positions;
	std::unordered_map<Node, Node, NodeHasher> node_paths;
};


int main()
{
	unsigned char pMap[] = { 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1 };
	int pOutBuffer[12];
	auto res = FindPath(0, 0, 1, 2, pMap, 4, 3, pOutBuffer, 12);

	unsigned char pMap2[] = { 0, 0, 1, 0, 1, 1, 1, 0, 1 };
	int pOutBuffer2[7];
	auto res2 = FindPath(2, 0, 0, 2, pMap2, 3, 3, pOutBuffer2, 7);

	unsigned char pMap3[] = { 1,1,1,1,1,0,0,0,0,1,
		0,0,0,0,1,0,0,0,0,1,
		0,0,0,0,1,1,1,1,1,1,
		1,1,1,1,1,0,0,0,0,1,
		1,0,0,0,1,1,1,1,1,1
	};
	int pOutBuffer3[20];
	auto res3 = FindPath(0, 0, 9, 4, pMap3, 10, 5, pOutBuffer3, 20);

	return 0;
}


int FindPath(const int nStartX, const int nStartY,
	const int nTargetX, const int nTargetY,
	const unsigned char* pMap, const int nMapWidth, const int nMapHeight,
	int* pOutBuffer, const int nOutBufferSize)
{
	Graph grid_graph{ nMapWidth, nMapHeight };
	auto res = grid_graph.BFS(nStartX, nStartY, nTargetX, nTargetY, pMap, pOutBuffer);

	//case 1
	auto test_1 = grid_graph.neighbours(1, 1, pMap); // should be 1,0 and 1,2
	auto test_2 = grid_graph.neighbours(0, 0, pMap); // should be 1,0
	auto test_3 = grid_graph.neighbours(3, 2, pMap); // should be 2,2 and 3,1

	//Case 2
	auto test_4 = grid_graph.neighbours(1, 1, pMap); // should be 2,0 and 2,1
	auto test_5 = grid_graph.neighbours(0, 0, pMap); // should be nothing
	auto test_6 = grid_graph.neighbours(2, 2, pMap); // should be 2,1
	auto test_7 = grid_graph.neighbours(0, 2, pMap); // should be nothing
	auto test_8 = grid_graph.neighbours(2, 0, pMap); // should be 2,1


	//Keep within the bounds by using % when iterating

	return res;
}