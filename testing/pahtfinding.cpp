#include "stdafx.h"
#include <vector>
#include <tuple>
#include <queue>
#include <unordered_map>
#include <string>
#include <functional>

//Pre-allocate whole block for the pathfinder at the start -> based on amount of nodes? + a bit extra? Prevents allocations during
//The running of the search.

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

	int heuristic(const Node& a, const Node& b)
	{
		//Using Manhattan distance for now
		return abs(a.m_x - b.m_x) + abs(a.m_y - b.m_y);
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
	int BFS(const int start_x, const int start_y, const int target_x, const int target_y,
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

		while (current != start)
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

	//A*

	//A* with JPS

	//A* with JRA

private:
	int m_width;
	int m_height;
	int m_pmap_size;
	std::vector<Node> neighbour_positions;
	NodeMap node_paths;
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

//Dijkstra
void dijkstra(Graph& graph, const Node& start, const Node& target, const unsigned char* pMap, NodeMap& came_from, CostMap& current_cost)
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

		for (auto& next : graph.neighbours(current.m_x, current.m_y, pMap))
		{
			int new_cost = current_cost[current] + 1;

			if (!current_cost.count(next) || new_cost < current_cost[next])
			{
				current_cost[next] = new_cost;
				came_from[next] = current;
				frontier.put(next, new_cost);
			}
		}
	}
}

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

		for (auto& next : graph.neighbours(current.m_x, current.m_y, pMap))
		{
			int new_cost = current_cost[current] + 1;

			if (!current_cost.count(next) || new_cost < current_cost[next])
			{
				current_cost[next] = new_cost;
				came_from[next] = current;
				frontier.put(next, new_cost);
				int priority = new_cost + heuristic(next, target);
			}
		}
	}
}

std::vector<int> reconstruct_path(const Node& start, const Node& target, NodeMap& came_from, const int mapWidth, int* pOutBuffer, int nOutBufferSize)
{
	std::vector<int> return_path;
	Node current = target;
	return_path.push_back((current.m_x + (current.m_y * mapWidth)));

	while (current != start)
	{
		current = came_from[current];
		if (Node{ -1, -1 } == came_from[current] || static_cast<int>(return_path.size()) > nOutBufferSize)
		{
			return {};
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
	//auto res = grid_graph.BFS(nStartX, nStartY, nTargetX, nTargetY, pMap, pOutBuffer);

	Node start = { nStartX, nStartY };
	Node target = { nTargetX, nTargetY };

	NodeMap came_from;
	CostMap current_cost;

	//Store the pMap pointer inside the graph instead?
	dijkstra(grid_graph, start, target, pMap, came_from, current_cost);
	auto res = reconstruct_path(start, target, came_from, nMapWidth, pOutBuffer, nOutBufferSize);

	return res.size() == 0 ? -1 : res.size();
}

//Need to prevent further searching if it's passed a certain threshold
