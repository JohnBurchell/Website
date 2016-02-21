#ifndef __PATHFINDING_H_
#define __PATHFINDING_H_

#include <functional>
#include <vector>
#include <queue>
#include <unordered_map>
#include <cmath>
#include <algorithm>

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
		//Required for storing nodes in an unordered map
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
		//auto D = 1;
		//auto D2 = std::sqrt(2);

		auto dx = std::abs(a.m_x - b.m_x);
		auto dy = std::abs(a.m_y - b.m_y);

		return std::max(dx, dy) + 0.41 * std::min(dx, dy); //Courtesy of GameAIPro (Rabin)
														   //return D * (dx + dy) + (D2 - 2 * D) * std::min(dx, dy);
	}

	typedef std::unordered_map<Node, Node, NodeHasher> NodeMap;
	typedef std::unordered_map<Node, int, NodeHasher> CostMap;
	typedef std::pair<Node, int> Node_Pair;
}

int FindPath(const int nStartX, const int nStartY,
	const int nTargetX, const int nTargetY,
	const unsigned char* pMap, const int nMapWidth, const int nMapHeight,
	int* pOutBuffer, const int nOutBufferSize);

struct Graph
{
	Graph(const int width, const int height, const unsigned char* pMap) :
		m_width(width),
		m_height(height),
		//N / E / S / W
		m_neighbour_positions{ Node{ 0, -1 }, Node{ 1, 0 }, Node{ 0, 1 }, Node{ -1, 0 } },
		m_pmap_size(height * width),
		m_pMap(pMap),
		m_frontier(),
		m_came_from(),
		m_current_cost()
	{}

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

		for (auto& pos : m_neighbour_positions)
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
	std::vector<Node> m_neighbour_positions;
	int m_pmap_size;
	const unsigned char* m_pMap;

public:
	std::priority_queue<Node_Pair, std::vector<Node_Pair>, std::greater<Node_Pair>> m_frontier;
	NodeMap m_came_from;
	CostMap m_current_cost;
};

//A* Search
void a_star(Graph& graph, const Node& start, const Node& target, const unsigned char* pMap)
{
	//Set start node and start cost
	graph.m_frontier.emplace(start, 0);
	graph.m_came_from[start] = start;
	graph.m_current_cost[start] = 0;

	while (!graph.m_frontier.empty())
	{
		//Pop the top node off the queue
		auto current = graph.m_frontier.top().first;
		graph.m_frontier.pop();

		if (current == target)
		{
			break;
		}

		for (auto& next : graph.neighbours(current.m_x, current.m_y))
		{
			//Get new cost - Weight added is always 1
			int new_cost = graph.m_current_cost[current] + 1;

			if (!graph.m_current_cost.count(next) || new_cost < graph.m_current_cost[next])
			{
				graph.m_current_cost[next] = new_cost;

				//Get the new priority by adding the new cost to the chosen heuristic
				double priority = new_cost + octile_heuristic(next, target);

				//Push back into the queue and update came_from map
				graph.m_frontier.emplace(next, priority);
				graph.m_came_from[next] = current;
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

int FindPath(const int nStartX, const int nStartY,
	const int nTargetX, const int nTargetY,
	const unsigned char* pMap, const int nMapWidth, const int nMapHeight,
	int* pOutBuffer, const int nOutBufferSize)
{
	Graph grid_graph{ nMapWidth, nMapHeight, pMap };

	Node start = { nStartX, nStartY };
	Node target = { nTargetX, nTargetY };

	a_star(grid_graph, start, target, pMap);
	auto res = reconstruct_path(start, target, grid_graph.m_came_from, nMapWidth, pOutBuffer, nOutBufferSize);

	return res.size() == 0 ? -1 : res.size();
}



#endif // __PATHFINDING_H_