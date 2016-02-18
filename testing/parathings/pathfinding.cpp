#include "stdafx.h"
#include <vector>
#include <tuple>
#include <queue>
#include <unordered_map>
#include <map>
#include <string>
#include <functional>
#include <iostream>

#include "honey.h"

/*

	Simple A* implementation.

	Tried to get JPS working but failed, perhaps this will be sufficient to pass the tests!

	Ways to optimise:

	Pre-allocate whole block of memory to prevent further allocations
	Use an optimisation of A* (I've tried JPS but it's not that suitable for only 4 directions)
	RSR might work?
	
	*/

namespace
{
	struct Node
	{
		Node() : m_x(-1), m_y(-1) {}
		Node(const int x, const int y) : m_x(x), m_y(y) {}
		int m_x;
		int m_y;

		friend bool operator==(const Node& lhs, const Node& rhs)
		{
			return (lhs.m_x == rhs.m_x
				&& lhs.m_y == rhs.m_y);
		}

		friend bool operator!=(const Node& lhs, const Node& rhs)
		{
			return !(lhs == rhs);
		}

		friend bool operator<(const Node& lhs, const Node& rhs)
		{
			return std::tie(lhs.m_x, lhs.m_y) < std::tie(rhs.m_x, rhs.m_y);
		}
	};


	struct NodeHasher
	{
		std::size_t operator()(const Node& node) const
		{
			return ((std::hash<int>()(node.m_x) ^ std::hash<int>()(node.m_y << 1)));
		}
	};

	int manhattan_heuristic(const int x1, const int y1, const int x2, const int y2)
	{
		return abs(x1 - x2) + abs(y1 - y2);
	}

	int manhattan_heuristic(const Node& a, const Node& b)
	{
		//Using Manhattan distance for now
		return 1 * abs(a.m_x - b.m_x) + abs(a.m_y - b.m_y);
	}

	double octile_heuristic(const Node& a, const Node& b)
	{
		auto D = 1;
		auto D2 = sqrt(2);

		auto dx = abs(a.m_x - b.m_x);
		auto dy = abs(a.m_y - b.m_y);

		return std::max(dx, dy) + 0.41 * std::min(dx, dy); //Courtesy of GameAIPro (Rabin)
      //return D * (dx + dy) + (D2 - 2 * D) * std::min(dx, dy);
	}

	typedef std::unordered_map<Node, Node, NodeHasher> NodeMap;
	typedef std::unordered_map<Node, int, NodeHasher> CostMap;
}

int FindPath(const int nStartX, const int nStartY,
	const int nTargetX, const int nTargetY,
	const unsigned char* pMap, const int nMapWidth, const int nMapHeight,
	int* pOutBuffer, const int nOutBufferSize);

struct Graph
{
	Graph(const int width, const int height, const unsigned char* pMap) : m_width(width), m_height(height),
		neighbour_positions(), m_pmap_size(height * width), m_pMap(pMap)
	{
		//N / E / S / W
		neighbour_positions = { Node{ 0, -1 }, Node{ 1, 0 }, Node{ 0, 1 }, Node{ -1, 0 } };
	}

	int get_index_value(const int x, const int y) const
	{
		return x + (y * m_width);
	}

	bool is_in_bounds(const Node& node)
	{
		return node.m_x >= 0 && node.m_x < m_width && node.m_y >= 0 && node.m_y < m_height;
	}

	bool is_obstacle(const Node& node)
	{
		return m_pMap[node.m_x + (node.m_y * m_width)] == 0;
	}

	bool reachable(const Node& node)
	{
		return is_in_bounds(node) && !is_obstacle(node);
	}

	bool valid_node(const Node& node)
	{
		return node.m_x >= 0 && node.m_y >= 0;
	}

	//Get the neighbours of a given node in the map
	std::vector<Node> neighbours(const Node& node)
	{
		return neighbours(node.m_x, node.m_y);
	}

	//Get the neighbours of a given node in the map
	std::vector<Node> neighbours(const int x, const int y)
	{
		std::vector<Node> return_vector{};

		for (auto& pos : neighbour_positions)
		{
			int new_x = x + pos.m_x;
			int new_y = y + pos.m_y;

			//If it's in bounds
			if ((new_x >= 0 && new_x < m_width && new_y >= 0 && new_y < m_height))
			{
				//Convert the combined position to get the real index value from pMap
				int real_index = get_index_value(new_x, new_y);

				//If the neighbour is valid
				if (real_index <= m_pmap_size && m_pMap[real_index] == 1)
				{
					Node neighbour{ new_x, new_y };
					return_vector.emplace_back(neighbour);
				}
			}
		}

		return return_vector;
	}


private:
	int m_width;
	int m_height;
	int m_pmap_size;
	std::vector<Node> neighbour_positions;
	NodeMap node_paths;
	const unsigned char* m_pMap;
};

//PQ
template<typename T, typename Number = int>
struct PriorityQueue
{
	typedef std::pair<Number, T> PQElement;
	std::priority_queue<PQElement, std::vector<PQElement>, std::greater<PQElement>> elements;

	inline bool empty() const
	{
		return elements.empty();
	}

	inline void put(const T& item, const Number priority)
	{
		elements.emplace(priority, item);
	}

	inline T get()
	{
		T best_item = elements.top().second;
		elements.pop();

		return best_item;
	}
};

//A* Search
void a_star(Graph& graph, const Node& start, const Node& target, const unsigned char* pMap, NodeMap& came_from, CostMap& current_cost)
{
	PriorityQueue<Node> frontier;
	frontier.put(start, 0);

	came_from[start] = start;
	current_cost[start] = 0;

	while (!frontier.empty())
	{
		auto current = frontier.get();

		if (current == target)
		{
			break;
		}

		for (auto& next : graph.neighbours(current.m_x, current.m_y))
		{
			int new_cost = current_cost[current] + 1;

			if (!current_cost.count(next) || new_cost < current_cost[next])
			{
				current_cost[next] = new_cost;
				double priority = new_cost + octile_heuristic(next, target);
				frontier.put(next, new_cost);
				came_from[next] = current;
			}
		}
	}
}

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

std::vector<int> reconstruct_path(const Node& start, const Node& target, NodeMap& came_from, const int mapWidth, int* pOutBuffer, int nOutBufferSize)
{
	std::vector<int> return_path;
	Node current = target;
	return_path.push_back((current.m_x + (current.m_y * mapWidth)));
	int count = 0;

	while (current != start)
	{
		current = came_from[current];
		if (Node{ -1, -1 } == came_from[current] || static_cast<int>(return_path.size()) > nOutBufferSize)
		{
			return{};
		}

		return_path.push_back((current.m_x + (current.m_y * mapWidth)));
	}

	//Get rid of the last entry -> it's the start
	return_path.pop_back();
	std::reverse(return_path.begin(), return_path.end());
	for (auto& x : return_path)
	{
		*pOutBuffer = x;
		++pOutBuffer;
	}
	return return_path;
}

int main()
{
	unsigned char pMap[] = { 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1 };
	int pOutBuffer[12];
	auto res = FindPath(0, 0, 1, 2, pMap, 4, 3, pOutBuffer, 12);

	unsigned char pMap2[] = { 0, 0, 1, 0, 1, 1, 1, 0, 1 };
	int pOutBuffer2[7];
	auto res2 = FindPath(2, 0, 0, 2, pMap2, 3, 3, pOutBuffer2, 7);

	unsigned char pMap3[] = { 
		1,1,1,1,1,0,0,0,0,1,
		0,0,0,0,1,0,0,0,0,1,
		0,0,0,0,1,1,1,1,1,1,
		1,1,1,1,1,0,0,0,0,1,
		1,0,0,0,1,1,1,1,1,1
	};
	int pOutBuffer3[20];
	auto res3 = FindPath(0, 0, 9, 4, pMap3, 10, 5, pOutBuffer3, 20);

	unsigned char pMap6[] = { 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1 };
	int pOutBuffer6[12];
	auto res6 = FindPath(0, 0, 1, 2, pMap6, 4, 3, pOutBuffer6, 12);

	Honey::honeycomb_walk();


	return 0;
}

int FindPath(const int nStartX, const int nStartY,
	const int nTargetX, const int nTargetY,
	const unsigned char* pMap, const int nMapWidth, const int nMapHeight,
	int* pOutBuffer, const int nOutBufferSize)
{
	Graph grid_graph{ nMapWidth, nMapHeight, pMap };

	Node start = { nStartX, nStartY };
	Node target = { nTargetX, nTargetY };

	NodeMap came_from;
	CostMap current_cost;

	a_star(grid_graph, start, target, pMap, came_from, current_cost);
	auto res = reconstruct_path(start, target, came_from, nMapWidth, pOutBuffer, nOutBufferSize);

	print_map(pMap, nMapWidth, nMapHeight);

	return res.size() == 0 ? -1 : res.size();
}
