#pragma once
#include "stdafx.h"
#include <vector>
#include <tuple>
#include <queue>
#include <unordered_map>
#include <map>
#include <string>
#include <functional>
#include <algorithm>
#include <iostream>

//Pre-allocate whole block for the pathfinder at the start -> based on amount of nodes? + a bit extra? Prevents allocations during
//The running of the search.

//IDEA! Remove as many allocations as possible

namespace JPS_NoDiag
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
		//auto D = 1;
		//auto D2 = sqrt(2);

		auto dx = std::abs(a.m_x - b.m_x);
		auto dy = std::abs(a.m_y - b.m_y);

		return std::max(dx, dy) + 0.41 * std::min(dx, dy); //Courtesy of GameAIPro (Rabin)
														   //return D * (dx + dy) + (D2 - 2 * D) * std::min(dx, dy);
	}

	typedef std::unordered_map<Node, Node, NodeHasher> NodeMap;
	typedef std::unordered_map<Node, int, NodeHasher> CostMap;
	typedef std::pair<Node, int> Node_Pair;
	typedef std::vector<Node> Neighbour_Vector;

	int FindPath(const int nStartX, const int nStartY,
		const int nTargetX, const int nTargetY,
		const unsigned char* pMap, const int nMapWidth, const int nMapHeight,
		int* pOutBuffer, const int nOutBufferSize);

	struct Graph
	{
		Graph(const int width, const int height, const unsigned char* pMap) :
			m_width(width),
			m_height(height),
			m_pmap_size(height * width),
			//N / E / S / W
			m_neighbour_positions{ Node{ 0, -1 }, Node{ 1, 0 }, Node{ 0, 1 }, Node{ -1, 0 } },
			m_pMap(pMap),
			m_frontier(),
			m_came_from(),
			m_current_cost()
		{
			//Pre-allocate the buckets in the map
			m_came_from.reserve(width * height);
			m_current_cost.reserve(width * height);

			//Reserve space for four possible neighbours
			m_neighbour_vector.reserve(4);
		};

		int get_index_value(const int x, const int y) const
		{
			return x + (y * m_width);
		}

		bool is_in_bounds(const int x, const int y)
		{
			return x >= 0 && x < m_width && y >= 0 && y < m_height;
		}

		bool is_obstacle(const int x, const int y)
		{
			return m_pMap[x + (y * m_width)] == 0;
		}

		bool reachable(const int x, const int y)
		{
			return is_in_bounds(x, y) && !is_obstacle(x, y);
		}

		bool valid_node(const int x, const int y)
		{
			return x >= 0 && y >= 0;
		}

		std::vector<Node> neighbours(const Node& node)
		{
			return neighbours(node.m_x, node.m_y);
		}

		//Get the neighbours of a given node in the map
		const std::vector<Node>& neighbours(const int x, const int y)
		{
			//Consider having this vector exist as a member, return refernce to it
			//Pre-allocate on creation?
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
						//Consider using the vector here instead of creating a new object each time
						Node neighbour{ new_x, new_y };
						m_neighbour_vector.emplace_back(neighbour);
					}
				}
			}

			return m_neighbour_vector;
		}

	private:
		int m_width;
		int m_height;
		int m_pmap_size;
		std::vector<Node> m_neighbour_positions;
		const unsigned char* m_pMap;

	public:
		std::priority_queue<Node_Pair, std::vector<Node_Pair>, std::greater<Node_Pair>> m_frontier;
		NodeMap m_came_from;
		CostMap m_current_cost;
		Neighbour_Vector m_neighbour_vector;

	};

	void print_map(const unsigned char* pmap, const int width, const int height, const NodeMap& from)
	{
		std::cout << "\n=====================================================\n";

		for (int i = 0; i != height; ++i)
		{
			for (int j = 0; j != width; ++j)
			{
				if (from.count(Node{ j, i }))
				{
					std::cout << "X";
				}
				else
				{
					std::cout << (pmap[j + (i * width)] == 1 ? 1 : 0);
				}
			}

			std::cout << "\n";
		}

		std::cout << "\n=====================================================\n";
	}

	std::vector<int> reconstruct_path(const Node& start, const Node& target, NodeMap& came_from, const int mapWidth, int* pOutBuffer, int nOutBufferSize)
	{
		std::vector<int> return_path;
		std::vector<int> return_path_test;

		Node current = target;
		return_path.push_back((current.m_x + (current.m_y * mapWidth)));

		while (current != start)
		{
			current = came_from[current];
			if (Node{ -1, -1 } == came_from[current] || static_cast<int>(return_path.size()) > nOutBufferSize)
			{
				if (target == Node{ 99, 99 })
				{
					return return_path;
				}

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

	void a_star_jps(Graph& graph, const Node& start, const Node& target, const unsigned char* pMap);

	int FindPath(const int nStartX, const int nStartY,
		const int nTargetX, const int nTargetY,
		const unsigned char* pMap, const int nMapWidth, const int nMapHeight,
		int* pOutBuffer, const int nOutBufferSize)
	{
		Graph grid_graph{ nMapWidth, nMapHeight, pMap };

		Node start = { nStartX, nStartY };
		Node target = { nTargetX, nTargetY };

		a_star_jps(grid_graph, start, target, pMap);
		auto res = reconstruct_path(start, target, grid_graph.m_came_from, nMapWidth, pOutBuffer, nOutBufferSize);

		//print_map(pMap, nMapWidth, nMapHeight, grid_graph.m_came_from);

		return res.size() == 0 ? -1 : res.size();
	}


	Node jump(const Node& current, const Node& parent, const Node& target, Graph& graph)
	{
		//Get the direction of the incomming node by removing the parent from it
		int dx = current.m_x - parent.m_x;
		int dy = current.m_y - parent.m_y;

		//If the current node is an obstacle or outside the grid
		if (!graph.reachable(current.m_x, current.m_y))
		{
			//Return null (aka Node{-1, -1})
			return Node{ -1, -1 };
		}

		//If the current node is the target, return the target node
		if (current == target)
		{
			graph.m_came_from[target] = parent;
			return target;
		}

		if (!graph.m_came_from.count(current))
		{
			graph.m_came_from[current] = parent;
		}

		if (dx != 0)
		{
			if ((graph.reachable(current.m_x, current.m_y + 1) && (!graph.reachable(current.m_x - dx, current.m_y + 1))) ||
				(graph.reachable(current.m_x, current.m_y - 1) && (!graph.reachable(current.m_x - dx, current.m_y - 1))))
			{
				//This node is a jump node, return here
				return current;
			}
		}

		//Search along y axis
		else if (dy != 0)
		{
			if ((graph.reachable(current.m_x + 1, current.m_y) && (!graph.reachable(current.m_x + 1, current.m_y - dy))) ||
				(graph.reachable(current.m_x - 1, current.m_y) && (!graph.reachable(current.m_x - 1, current.m_y - dy))))
			{
				//This node is a jump node, return here
				return current;
			}

			//Make an extra check when moving vertically for horizontal jump points
			//Refactor to just take ints?
			auto right = jump(Node{ current.m_x + 1, current.m_y }, current, target, graph);
			auto left = jump(Node{ current.m_x - 1, current.m_y }, current, target, graph);
			if (graph.valid_node(right.m_x, right.m_y) || graph.valid_node(left.m_x, left.m_y))
			{
				return current;
			}
		}

		else
		{
			// Should never happen
			return Node{ -1, -1 };
		}

		//Recurse diagonally
		return jump(Node{ current.m_x + dx, current.m_y + dy }, current, target, graph);
	}
	std::vector<Node> prune_neighbours(const Node& node, const Node& parent, Graph& graph)
	{
		std::vector<Node> neighbours{};

		//Check if the current node has a parent, if it is the first node
		if (graph.valid_node(parent.m_x, parent.m_y) && node != parent)
		{
			int dx = (node.m_x - parent.m_x); /// std::max(std::abs(node.m_x - parent.m_x), 1);
			int dy = (node.m_y - parent.m_y); /// std::max(std::abs(node.m_y - parent.m_y), 1);

			if (dx != 0)
			{
				Node n1{ node.m_x, node.m_y - 1 };
				Node n2{ node.m_x, node.m_y + 1 };
				Node n3{ node.m_x + dx, node.m_y };

				if (graph.reachable(n1.m_x, n1.m_y))
				{
					neighbours.push_back(n1);
				}
				if (graph.reachable(n2.m_x, n2.m_y))
				{
					neighbours.push_back(n2);
				}
				if (graph.reachable(n3.m_x, n3.m_y))
				{
					neighbours.push_back(n3);
				}
			}
			else if (dy != 0)
			{
				Node n1{ node.m_x - 1, node.m_y };
				Node n2{ node.m_x + 1, node.m_y };
				Node n3{ node.m_x, node.m_y + dy };

				if (graph.reachable(n1.m_x, n1.m_y))
				{
					neighbours.push_back(n1);
				}
				if (graph.reachable(n2.m_x, n2.m_y))
				{
					neighbours.push_back(n2);
				}
				if (graph.reachable(n3.m_x, n3.m_y))
				{
					neighbours.push_back(n3);
				}
			}

		}//if !(parent)
		else
		{
			//Else just get all the neighbours
			return graph.neighbours(node);
		}

		return neighbours;
	}

	std::vector<Node> identify_successors(const Node& current, const Node& parent, const Node& target, Graph& graph)
	{
		std::vector<Node> successors{};

		std::vector<Node> neighbours = prune_neighbours(current, graph.m_came_from[parent], graph);

		for (auto& neighbour : neighbours)
		{
			Node jump_node = jump(neighbour, current, target, graph);

			//If the node is valid, add to successors
			if (graph.valid_node(jump_node.m_x, jump_node.m_y))
			{
				successors.push_back(jump_node);
			}

			if (!graph.m_came_from.count(neighbour))
			{
				graph.m_came_from[neighbour] = current;
			}
		}

		return successors;
	}

	//A* Search with JPS (Jump point search)
	void a_star_jps(Graph& graph, const Node& start, const Node& target, const unsigned char* pMap)
	{
		graph.m_frontier.emplace(start, 0);

		graph.m_came_from[start] = start;
		graph.m_current_cost[start] = 0;

		while (!graph.m_frontier.empty())
		{
			auto current = graph.m_frontier.top().first;
			graph.m_frontier.pop();

			if (current == target)
			{
				//target acquired
				break;
			}

			auto successors = identify_successors(current, graph.m_came_from[current], target, graph);

			for (auto& x : successors)
			{
				//Weights are always one
				int new_cost = graph.m_current_cost[current] + 1;
				if (!graph.m_current_cost.count(x) || new_cost < graph.m_current_cost[x])
				{
					//Don't need to add to the "came_from" path here, that's done in the jump / by the neighbours
					double priority = new_cost + manhattan_heuristic(x, target);
					graph.m_current_cost[x] = new_cost;
					graph.m_frontier.emplace(x, priority);
				}
			}
		}
	}
}