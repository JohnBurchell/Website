#pragma once
#include "stdafx.h"
#include <vector>
#include <tuple>
#include <queue>
#include <unordered_map>
#include <map>
#include <string>
#include <functional>
#include <iostream>

//Pre-allocate whole block for the pathfinder at the start -> based on amount of nodes? + a bit extra? Prevents allocations during
//The running of the search.

namespace JPS
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


	int FindPath(const int nStartX, const int nStartY,
		const int nTargetX, const int nTargetY,
		const unsigned char* pMap, const int nMapWidth, const int nMapHeight,
		int* pOutBuffer, const int nOutBufferSize);

	struct Graph
	{
		Graph(const int width, const int height, const unsigned char* pMap) : m_width(width), m_height(height),
			neighbour_positions_4(), neighbour_positions_8(), m_pmap_size(height * width), m_pMap(pMap)
		{
			//N / E / S / W
			neighbour_positions_4 = { Node{ 0, -1 }, Node{ 1, 0 }, Node{ 0, 1 }, Node{ -1, 0 } };

			//N //NE //E //SE //S //SW //W //NW
			neighbour_positions_8 = { Node{ 0, -1 }, Node{ 1, 1 },  Node{ 1, 0 }, Node{ 1, -1 }, Node{ 0, 1 }, Node{ -1, -1 }, Node{ -1, 0 }, Node{ -1, 1 } };

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
			std::vector<Node> return_vector{};

			for (auto& pos : neighbour_positions_8)
			{
				int new_x = node.m_x + pos.m_x;
				int new_y = node.m_y + pos.m_y;

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

		//Get the neighbours of a given node in the map
		std::vector<Node> neighbours(const int x, const int y)
		{
			std::vector<Node> return_vector{};

			for (auto& pos : neighbour_positions_4)
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

		//BFS
		int BFS(const Node& start, const Node& target, int* pOutBuffer)
		{
			auto frontier = std::queue<Node>();
			bool target_found = false;
			frontier.push(start);

			node_paths[start] = start;

			Node current = start;

			while (!frontier.empty())
			{
				current = frontier.front();
				frontier.pop();

				auto nn = neighbours(current.m_x, current.m_y);

				if (current == target)
				{
					target_found = true;
					break;
				}

				for (auto& next : neighbours(current.m_x, current.m_y))
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


	private:
		int m_width;
		int m_height;
		int m_pmap_size;
		std::vector<Node> neighbour_positions_4;
		std::vector<Node> neighbour_positions_8;
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

	//Dijkstra
	void dijkstra(Graph& graph, const Node& start, const Node& target, const unsigned char* pMap, NodeMap& came_from, CostMap& current_cost)
	{
		PriorityQueue<Node> frontier;
		frontier.put(start, 0);

		came_from[start] = start;
		current_cost[start] = 0;

		auto test_search = graph.neighbours(target.m_x, target.m_y);

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
					came_from[next] = current;
					frontier.put(next, new_cost);
				}
			}
		}

		auto test = true;
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

			for (auto& next : graph.neighbours(current.m_x, current.m_y))
			{
				int new_cost = current_cost[current] + 1;

				if (!current_cost.count(next) || new_cost < current_cost[next])
				{
					current_cost[next] = new_cost;
					came_from[next] = current;
					frontier.put(next, new_cost);
					double priority = new_cost + octile_heuristic(next, target);
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
		//unsigned char pMap[] = { 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1 };
		//int pOutBuffer[12];
		//auto res = FindPath(0, 0, 1, 2, pMap, 4, 3, pOutBuffer, 12);

		//unsigned char pMap2[] = { 0, 0, 1, 0, 1, 1, 1, 0, 1 };
		//int pOutBuffer2[7];
		//auto res2 = FindPath(2, 0, 0, 2, pMap2, 3, 3, pOutBuffer2, 7);

		unsigned char pMap3[] = { 1,1,1,1,1,0,0,0,0,1,
			0,0,0,0,1,0,0,0,0,1,
			0,0,0,0,1,1,1,1,1,1,
			1,1,1,1,1,0,0,0,0,1,
			1,0,0,0,1,1,1,1,1,1
		};
		int pOutBuffer3[20];
		auto res3 = FindPath(0, 0, 9, 4, pMap3, 10, 5, pOutBuffer3, 20);

		//unsigned char pMap6[] = { 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1 };
		//int pOutBuffer6[12];
		////auto res6 = FindPath(0, 0, 1, 2, pMap6, 4, 3, pOutBuffer6, 12);

		unsigned char pMap7[] = {
			1, 1, 1, 1,
			1, 1, 0, 1,
			1, 1, 0, 1
		};
		int pOutBuffer7[12];
		auto res7 = FindPath(0, 1, 3, 2, pMap7, 4, 3, pOutBuffer7, 12);

		//Honey::honeycomb_walk();
		//Pizza::pizza();

		return 0;
	}

	void a_star_jps(Graph& graph, const Node& start, const Node& target, const unsigned char* pMap, NodeMap& came_from, CostMap& current_cost);

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

		a_star_jps(grid_graph, start, target, pMap, came_from, current_cost);
		auto res = reconstruct_path(start, target, came_from, nMapWidth, pOutBuffer, nOutBufferSize);

		print_map(pMap, nMapWidth, nMapHeight);

		return res.size() == 0 ? -1 : res.size();
	}

	//Need to prevent further searching if it's passed a certain threshold


	//If none of the non-dominated nodes are blocked, keep going in the "same"? direction until one is found
	//If one of these are found, it is a jump point and is assigned a the cost so far (g-value) of g(y) = g(x) + dist(x, y)
	//If it reaches an obstacle in this direction, then nothing is generated (hits a dead end or a wall)

	//Forced if not all neighbours can be pruned (i.e. an obstacle is found)

	//A jump point is either:
	//1 -> The goal node
	//2 -> Has at least one neighbour that is forced (see above definition -> i.e. adjacent to an obstacle)
	//3 -> Ignoring this for now as it concerns diagonal moves


	Node jump(const Node& current, const Node& parent, const Node& target, Graph& graph)
	{
		//Get the direction of the incomming node by removing the parent from it
		int dx = current.m_x - parent.m_x;
		int dy = current.m_y - parent.m_y;

		//If the current node is an obstacle or outside the grid
		if (!graph.reachable(current))
		{
			//Return null (aka Node{-1, -1}
			return Node{ -1, -1 };
		}

		//If the current node is the target, return the target node
		if (current == target)
		{
			return target;
		}

		if (dx != 0 && dy != 0)
		{
			if ((graph.reachable(Node{ current.m_x - dx,  current.m_y + dy }) && !graph.reachable(Node{ current.m_x - dx,  current.m_y })) ||
				(graph.reachable(Node{ current.m_x + dx,  current.m_y - dy }) && !graph.reachable(Node{ current.m_x ,  current.m_y - dy })))
			{
				return current;
			}
			// when moving diagonally, must check for vertical/horizontal jump points
			if (graph.reachable(jump(Node{ current.m_x + dx, current.m_y }, current, target, graph)) || 
				graph.reachable((jump(Node{ current.m_x, current.m_y + dy }, current, target, graph))))
			{
				return current;
			}
		}


		//Now check the neighbours to see if any of them are forced i.e
		//
		// x-?-?
		// x-C-?
		// P-x-x
		//
		// P = Parent
		// C = curent
		// ? = Possible next node if this is not a jump point
		//Search along x-axis
		if (dx != 0)
		{
			// x-!-!      !-!-x
			// P-C-?  OR  ?-C-P
			// x-!-!      !-!-x
			//Exclamation marks are checked

			auto t1 = graph.reachable(Node{ current.m_x, current.m_y - 1 });
			auto t2 = !graph.reachable(Node{ current.m_x - dx, current.m_y - 1 });
			auto t3 = graph.reachable(Node{ current.m_x, current.m_y + 1 });
			auto t4 = !graph.reachable(Node{ current.m_x - dx, current.m_y + 1 });

			if ((graph.reachable(Node{ current.m_x, current.m_y - 1 }) && (!graph.reachable(Node{ current.m_x - dx, current.m_y - 1 }))) ||
				(graph.reachable(Node{ current.m_x, current.m_y + 1 }) && (!graph.reachable(Node{ current.m_x - dx, current.m_y + 1 }))))
			{
				//This node is a jump node, return here
				return current;
			}
		}

		//Search along y axis
		else if (dy != 0)
		{
			// !-?-!	  x-P-x
			// !-C-!  OR  !-C-!
			// x-P-x      !-?-! 
			//Exclamation marks are checked
			auto t1 = graph.reachable(Node{ current.m_x - 1, current.m_y });
			auto t2 = !graph.reachable(Node{ current.m_x - 1, current.m_y - dy });
			auto t3 = graph.reachable(Node{ current.m_x + 1, current.m_y });
			auto t4 = !graph.reachable(Node{ current.m_x + 1, current.m_y - dy });

			if ((graph.reachable(Node{ current.m_x - 1, current.m_y }) && (!graph.reachable(Node{ current.m_x - 1, current.m_y - dy }))) ||
				(graph.reachable(Node{ current.m_x + 1, current.m_y }) && (!graph.reachable(Node{ current.m_x + 1, current.m_y - dy }))))
			{
				//This node is a jump node, return here
				return current;
			}

			//Do an extra horizontal check
			if (graph.valid_node(jump(Node{ current.m_x + 1, current.m_y }, current, target, graph)) || graph.valid_node(jump(Node{ current.m_x - 1, current.m_y }, current, target, graph)))
			{
				return current;
			}
		}

		//Recurse diagonally
		return jump(Node{ current.m_x + dx, current.m_y + dy }, current, target, graph);
	}
	std::vector<Node> prune_neighbours(const Node& node, const Node& parent, Graph& graph)
	{
		std::vector<Node> neighbours{};

		//Check if the current node has a parent, if it is the first node
		if (graph.valid_node(parent) && node != parent)
		{
			int dx = (node.m_x - parent.m_x);
			int dy = (node.m_y - parent.m_y);

			if (dx != 0)
			{
				Node n1{ node.m_x, node.m_y - 1 };
				Node n2{ node.m_x, node.m_y + 1 };
				Node n3{ node.m_x + dx, node.m_y };

				if (graph.reachable(n1))
				{
					neighbours.push_back(n1);
				}
				if (graph.reachable(n2))
				{
					neighbours.push_back(n2);
				}
				if (graph.reachable(n3))
				{
					neighbours.push_back(n3);
				}
			}
			else if (dy != 0)
			{
				Node n1{ node.m_x - 1, node.m_y };
				Node n2{ node.m_x + 1, node.m_y };
				Node n3{ node.m_x, node.m_y + dy };

				if (graph.reachable(n1))
				{
					neighbours.push_back(n1);
				}
				if (graph.reachable(n2))
				{
					neighbours.push_back(n2);
				}
				if (graph.reachable(n3))
				{
					neighbours.push_back(n3);
				}
			}
		}//if(parent)
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
		std::vector<Node> neighbours = prune_neighbours(current, parent, graph);

		std::vector<Node> neighbours1 = prune_neighbours({ 3, 1 }, { 3, 0 }, graph);
		std::vector<Node> neighbours2 = prune_neighbours({ 2, 0 }, { 1, 0 }, graph);

		for (auto& neighbour : neighbours)
		{
			Node jump_node = jump(neighbour, parent, target, graph);

			//If the node is valid, add to successors
			if (graph.valid_node(jump_node))
			{
				successors.push_back(jump_node);
			}
		}

		return successors;
	}

	//A* Search with JPS (Jump point search)
	void a_star_jps(Graph& graph, const Node& start, const Node& target, const unsigned char* pMap, NodeMap& came_from, CostMap& current_cost)
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

			auto successors = identify_successors(current, came_from[current], target, graph);

			for (auto& x : successors)
			{
				int new_cost = current_cost[current] + 1;

				current_cost[x] = new_cost;
				came_from[x] = current;
				frontier.put(x, new_cost);
				double priority = new_cost + octile_heuristic(x, target);
			}

			auto test = true;
		}
	}

}