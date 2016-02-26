#include <vector>
#include <queue>
#include <functional>
#include <algorithm>
#include <memory>
#include <map>

struct Node
{
	Node() : m_x(-1), m_y(-1), m_px(-1), m_py(-1), m_closed(false), m_opened(false), parent_index(-1) {}
	Node(const int x, const int y, const int px = -1, const int py = -1) : 
		m_x(x), m_y(y), m_px(px), m_py(py), m_closed(false), m_opened(false), parent_index(-1) {}
	int m_x;
	int m_y;

	int m_px;
	int m_py;
	
	int parent_index;

	bool m_closed;
	bool m_opened;

	friend bool operator<(const Node& lhs, const Node& rhs)
	{
		return (lhs.m_x < rhs.m_x) && (lhs.m_y < rhs.m_x);
	}
};

typedef std::pair<Node, int> Node_Pair;
typedef std::vector<Node> Neighbour_Vector;

struct Compare_Nodes
{
	bool operator()(const Node_Pair& lhs, const Node_Pair& rhs)
	{
		return lhs.second > rhs.second;
	}
};

int manhattan_heuristic(const int x1, const int y1, const int x2, const int y2)
{
	return abs(x1 - x2) + abs(y1 - y2);
}

int octile_heuristic(const int x1, const int y1, const int x2, const int y2)
{
	auto F = std::sqrt(2 - 1);
	auto dx = std::abs(x1 - x2);
	auto dy = std::abs(y1 - y2);
	return (dx < dy) ? F * dx + dy : F * dy + dx;
}

int FindPath(const int nStartX, const int nStartY,
	const int nTargetX, const int nTargetY,
	const unsigned char* pMap, const int nMapWidth, const int nMapHeight,
	int* pOutBuffer, const int nOutBufferSize);

class Graph
{
	Graph() {};

public:

	Graph(const int width, const int height, const unsigned char* pmap) :
		m_width(width),
		m_height(height),
		m_pmap_size(width * height),
		m_neighbour_positions{ Node{ 0, -1 }, Node{ 1, 0 }, Node{ 0, 1 }, Node{ -1, 0 } },
		m_pMap(pmap),
		m_bad_node(new Node{}),
		cost_map_vec(m_pmap_size, 0)
	{
		m_neighbour_vector.reserve(4);
		m_neighbour_vector = { { -1, -1 },{ -1, -1 },{ -1, -1 },{ -1, -1 } };
		m_prune_neighbours.reserve(4);
		m_successors.reserve(10);
		m_map_rep = std::vector<Node>(m_pmap_size, Node{ -1, -1 });
	}

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

	//Get the neighbours of a given node in the map
	void neighbours(const int x, const int y)
	{
		//Consider having this vector exist as a member, return refernce to it
		//Pre-allocate on creation?
		auto neighbour_iter = m_neighbour_vector.begin();

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
					neighbour_iter->m_x = new_x;
					neighbour_iter->m_y = new_y;
					neighbour_iter->m_px = x;
					neighbour_iter->m_py = y;

					++neighbour_iter;
				}
			}
		}
	}
	void add_to_vec(const Node& current, const Node& parent)
	{
		//Put the node into its correct place in the vector
		int index = current.m_x + (current.m_y * m_width);
		m_map_rep[index] = Node{ current.m_x, current.m_y, parent.m_x, parent.m_y };
	}

private:
	int m_width;
	int m_height;
	int m_pmap_size;
	std::vector<Node> m_neighbour_positions;
	const unsigned char* m_pMap;

public:
	std::priority_queue<Node_Pair, std::vector<Node_Pair>, Compare_Nodes> m_frontier;
	Neighbour_Vector m_neighbour_vector;
	Neighbour_Vector m_prune_neighbours;

	std::vector<Node> m_successors;

	Node* m_bad_node;

	std::vector<Node> m_map_rep;
	std::vector<int> cost_map_vec;
};

std::vector<int> reconstruct_path(const Node& start, const Node& target, const int mapWidth,
	int* pOutBuffer, const std::vector<Node>& pmap_vec, const int maxmiumPath)
{
	std::vector<int> return_path{};

	Node current = pmap_vec[target.m_x + (target.m_y * mapWidth)];

	//Push the target onto the end
	return_path.push_back(current.m_x + (current.m_y * mapWidth));

	while (true)
	{
		if (current.m_x == start.m_x && current.m_y == start.m_y)
		{
			break;
		}

		if (current.m_x != current.m_px)
		{
			//Need to move either south or down for the path
			int count = current.m_px - current.m_x;
			if (count < 0)
			{
				//Going left
				for (int i = 1; i <= std::abs(count); ++i)
				{
					auto new_x = current.m_x - i;
					auto new_y = current.m_y;
					auto test_index = (new_x + (new_y * mapWidth));
					return_path.push_back(test_index);
				}
			}
			else
			{
				//Going right
				for (int i = 1; i <= count; ++i)
				{
					auto new_x = current.m_x + i;
					auto new_y = current.m_y;
					auto test_index = (new_x + (new_y * mapWidth));
					return_path.push_back(test_index);
				}
			}
		}

		else if (current.m_y != current.m_py)
		{
			//Need to move either south or north for rebuilding the path
			int count = current.m_py - current.m_y;
			if (count < 0)
			{
				//Going up
				for (int i = 1; i <= std::abs(count); ++i)
				{
					auto new_x = current.m_x;
					auto new_y = current.m_y - i;
					auto test_index = (new_x + (new_y * mapWidth));
					return_path.push_back(test_index);
				}
			}
			else
			{
				//Going down
				for (int i = 1; i <= count; ++i)
				{
					auto new_x = current.m_x;
					auto new_y = current.m_y + i;
					auto test_index = (new_x + (new_y * mapWidth));
					return_path.push_back(test_index);
				}
			}
		}

		current = pmap_vec[current.m_px + (mapWidth * current.m_py)];
	}

	//Always pop the top -> If the node is the target then it's also the start node
	return_path.pop_back();
	std::reverse(return_path.begin(), return_path.end());

	int count = 0;
	for (auto& x : return_path)
	{
		*pOutBuffer = x;
		++pOutBuffer;
		++count;

		if (count == maxmiumPath)
		{
			return return_path;
		}
	}

	return return_path;
}

Node a_star_jps(Graph& graph, const Node& start, const Node& target);
Node jump_north(const Node& node, const Node& parent, const Node& target, Graph& graph);
Node jump_east(const Node& node, const Node& parent, const Node& target, Graph& graph);
Node jump_south(const Node& node, const Node& parent, const Node& target, Graph& graph);
Node jump_west(const Node& node, const Node& parent, const Node& target, Graph& graph);

int FindPath(const int nStartX, const int nStartY,
	const int nTargetX, const int nTargetY,
	const unsigned char* pMap, const int nMapWidth, const int nMapHeight,
	int* pOutBuffer, const int nOutBufferSize)
{
	Node start = { nStartX, nStartY };
	Node target = { nTargetX, nTargetY };

	Graph graph = Graph(nMapWidth, nMapHeight, pMap);
	auto target_found = a_star_jps(graph, start, target);

	if (target_found.m_x == -1 && target_found.m_y == -1)
	{
		return -1;
	}

	auto res = reconstruct_path(start, target_found, nMapWidth, pOutBuffer, graph.m_map_rep, nOutBufferSize);
	return res.size();
}

Node jump_north(const Node& node, const Node& parent, const Node& target, Graph& graph)
{
	auto current = Node{ node.m_x, node.m_y, parent.m_x, parent.m_y };

	while (true)
	{
		if (!graph.reachable(current.m_x, current.m_y))
		{
			//Not reachable
			return *graph.m_bad_node;
		}

		if (current.m_x == target.m_x && current.m_y == target.m_y)
		{
			return Node{ current.m_x, current.m_y, current.m_px, current.m_py };
		}

		if ((graph.reachable(current.m_x + 1, current.m_y) && (!graph.reachable(current.m_x + 1, current.m_y - (-1)))) ||
			(graph.reachable(current.m_x - 1, current.m_y) && (!graph.reachable(current.m_x - 1, current.m_y - (-1)))))
		{
			return current;
		}

		auto east = jump_east(Node{ current.m_x + 1, current.m_y }, Node{ current.m_x, current.m_y }, target, graph);
		auto west = jump_west(Node{ current.m_x - 1, current.m_y }, Node{ current.m_x, current.m_y }, target, graph);

		if (graph.reachable(east.m_x, east.m_y) || graph.reachable(west.m_x, west.m_y))
		{
			return current;
		}

		current = Node{ current.m_x, current.m_y - 1, current.m_x, current.m_y };
	}

	return *graph.m_bad_node;
}

Node jump_east(const Node& node, const Node& parent, const Node& target, Graph& graph)
{
	auto current = Node{ node.m_x, node.m_y, parent.m_x, parent.m_y };

	while (true)
	{
		if (!graph.reachable(current.m_x, current.m_y))
		{
			return *graph.m_bad_node;
		}

		if (current.m_x == target.m_x && current.m_y == target.m_y)
		{
			return Node{ current.m_x, current.m_y, current.m_px, current.m_py };
		}

		if ((graph.reachable(current.m_x, current.m_y + 1) && (!graph.reachable(current.m_x - 1, current.m_y + 1))) ||
			(graph.reachable(current.m_x, current.m_y - 1) && (!graph.reachable(current.m_x - 1, current.m_y - 1))))
		{
			return current;
		}
		current = Node{ current.m_x + 1, current.m_y, current.m_x, current.m_y };

	}

	return *graph.m_bad_node;
}

Node jump_south(const Node& node, const Node& parent, const Node& target, Graph& graph)
{
	auto current = Node{ node.m_x, node.m_y, parent.m_x, parent.m_y };
	while (true)
	{
		if (!graph.reachable(current.m_x, current.m_y))
		{
			return *graph.m_bad_node;
		}

		if (current.m_x == target.m_x && current.m_y == target.m_y)
		{
			return Node{ current.m_x, current.m_y, current.m_px, current.m_py };
		}


		if ((graph.reachable(current.m_x + 1, current.m_y) && (!graph.reachable(current.m_x + 1, current.m_y - 1))) ||
			(graph.reachable(current.m_x - 1, current.m_y) && (!graph.reachable(current.m_x - 1, current.m_y - 1))))
		{
			return current;
		}

		auto east = jump_east(Node{ current.m_x + 1, current.m_y }, Node{ current.m_x, current.m_y }, target, graph);
		auto west = jump_west(Node{ current.m_x - 1, current.m_y }, Node{ current.m_x, current.m_y }, target, graph);

		if (graph.reachable(east.m_x, east.m_y) || graph.reachable(west.m_x, west.m_y))
		{
			return current;
		}

		current = Node{ current.m_x, current.m_y + 1, current.m_x, current.m_y };
	}

	return *graph.m_bad_node;
}


Node jump_west(const Node& node, const Node& parent, const Node& target, Graph& graph)
{
	auto current = Node{ node.m_x, node.m_y, parent.m_x, parent.m_y };
	while (true)
	{

		if (!graph.reachable(current.m_x, current.m_y))
		{
			return *graph.m_bad_node;
		}

		if (current.m_x == target.m_x && current.m_y == target.m_y)
		{
			return Node{ current.m_x, current.m_y, current.m_px, current.m_py };
		}

		if ((graph.reachable(current.m_x, current.m_y + 1) && (!graph.reachable(current.m_x + 1, current.m_y + 1))) ||
			(graph.reachable(current.m_x, current.m_y - 1) && (!graph.reachable(current.m_x + 1, current.m_y - 1))))
		{
			return current;
		}

		current = Node{ current.m_x - 1, current.m_y, current.m_x, current.m_y };

	}

	return *graph.m_bad_node;
}

Node jump(const Node& current, const Node& parent, const Node& target, Graph& graph)
{
	//Get the direction of the incomming node by removing the parent from it
	int dx = current.m_x - parent.m_x;
	int dy = current.m_y - parent.m_y;

	if (!graph.reachable(current.m_x, current.m_y))
	{
		//Not reachable
		return *graph.m_bad_node;
	}

	Node new_node{};

	//North
	if (dx == 0 && dy == -1)
	{
		new_node = jump_north(current, parent, target, graph);
	}
	//East
	else if (dx == 1 && dy == 0)
	{
		new_node = jump_east(current, parent, target, graph);
	}
	//South
	else if (dx == 0 && dy == 1)
	{
		new_node = jump_south(current, parent, target, graph);
	}
	//West
	else if (dx == -1 && dy == 0)
	{
		new_node = jump_west(current, parent, target, graph);
	}

	//A jump point or a null node has been found
	return new_node;
}

void prune_neighbours(const Node& node, const Node& parent, Graph& graph)
{
	graph.m_prune_neighbours.clear();
	//Check if the current node has a parent, if it is the first node
	if (graph.valid_node(parent.m_x, parent.m_y) &&
		!(node.m_x == parent.m_x && node.m_y == parent.m_y))
	{
		int dx = (node.m_x - parent.m_x);
		int dy = (node.m_y - parent.m_y);

		if (dx != 0)
		{
			Node n1{ node.m_x, node.m_y - 1 };
			Node n2{ node.m_x, node.m_y + 1 };
			Node n3{ node.m_x + dx, node.m_y };

			if (graph.reachable(n1.m_x, n1.m_y))
			{
				graph.m_prune_neighbours.push_back(n1);
			}
			if (graph.reachable(n2.m_x, n2.m_y))
			{
				graph.m_prune_neighbours.push_back(n2);
			}
			if (graph.reachable(n3.m_x, n3.m_y))
			{
				graph.m_prune_neighbours.push_back(n3);
			}
		}
		else if (dy != 0)
		{
			Node n1{ node.m_x - 1, node.m_y };
			Node n2{ node.m_x + 1, node.m_y };
			Node n3{ node.m_x, node.m_y + dy };


			if (graph.reachable(n1.m_x, n1.m_y))
			{
				graph.m_prune_neighbours.push_back(n1);
			}
			if (graph.reachable(n2.m_x, n2.m_y))
			{
				graph.m_prune_neighbours.push_back(n2);
			}
			if (graph.reachable(n3.m_x, n3.m_y))
			{
				graph.m_prune_neighbours.push_back(n3);
			}
		}

	}//if parent is not valid
	else
	{
		//Else just get all the neighbours
		graph.neighbours(node.m_x, node.m_y);
		graph.m_prune_neighbours = graph.m_neighbour_vector;
	}
}

void identify_successors(const Node& current, const Node& parent, const Node& target, Graph& graph)
{
	prune_neighbours(current, parent, graph);
	graph.m_successors.clear();

	for (auto& neighbour : graph.m_prune_neighbours)
	{
		Node jump_node = jump(neighbour, current, target, graph);

		//If the node is valid, add to successors
		if (graph.valid_node(jump_node.m_x, jump_node.m_y))
		{
			graph.m_successors.push_back(jump_node);
		}
	}
}

//A* Search with JPS (Jump point search)
Node a_star_jps(Graph& graph, const Node& start, const Node& target)
{
	auto correct_start = Node{ start.m_x, start.m_y, start.m_x, start.m_y };
	correct_start.m_opened = true;
	graph.m_frontier.emplace(correct_start, 0);

	//Set the start to the correct values
	graph.m_map_rep[graph.get_index_value(correct_start.m_x, correct_start.m_y)] = correct_start;
	graph.cost_map_vec[graph.get_index_value(correct_start.m_x, correct_start.m_y)] = 0;

	while (!graph.m_frontier.empty())
	{
		auto current = graph.m_frontier.top().first;
		current.m_closed = true;
		graph.m_frontier.pop();

		if (current.m_x == target.m_x && current.m_y == target.m_y)
		{
			//target acquired
			return current;
		}

		//Update successors
		identify_successors(current, Node{ current.m_px, current.m_py }, target, graph);

		for (auto& jump_point : graph.m_successors)
		{
			auto jump_point_index = graph.get_index_value(jump_point.m_x, jump_point.m_y);
			auto current_index = graph.get_index_value(current.m_x, current.m_y);

			if (graph.m_map_rep[jump_point_index].m_closed)
			{
				continue;
			}

			//adds the distance from the last node, plus one to make the cost
			int distance = manhattan_heuristic(jump_point.m_x, jump_point.m_y, current.m_x, current.m_y);
			int cost = graph.cost_map_vec[current_index] + distance;
			//int cost = graph.cost_map_vec[index];

			if (!graph.m_map_rep[jump_point_index].m_opened ||
				cost < graph.cost_map_vec[jump_point_index])
			{
				graph.cost_map_vec[jump_point_index] = cost;
				auto man_to_target = manhattan_heuristic(jump_point.m_x, jump_point.m_y, target.m_x, target.m_y);
				int new_f_value = cost + manhattan_heuristic(jump_point.m_x, jump_point.m_y, target.m_x, target.m_y);

				if (!graph.m_map_rep[jump_point_index].m_opened)
				{
					graph.m_frontier.emplace(jump_point, new_f_value);
					graph.m_map_rep[jump_point_index].m_opened = true;
					graph.add_to_vec(jump_point, current);
					jump_point.parent_index = current_index;
				}
				else
				{
					graph.m_frontier.emplace(jump_point, new_f_value);
					//Update the current value
				}
			}
		}
	}

	//No target found
	return Node{ -1, -1 };
}

