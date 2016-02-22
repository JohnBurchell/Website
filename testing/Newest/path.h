#include <vector>
#include <tuple>
#include <queue>
#include <unordered_map>
#include <map>
#include <string>
#include <functional>
#include <algorithm>
#include <iostream>

//Idea
//Make the graph static to prevent multiple calls -> Not so well versed in multi-threaded environments so I think this might be why it's timing out
//If this is not why - then I don't know, the map being passed to it must be huuuuuuuuuuuuuuuuuuuuge!

struct Node
{
	Node() : m_x(-1), m_y(-1), m_px(-1), m_py(-1), m_closed(false) {}
	Node(const int x, const int y, const int px = -1, const int py = -1) : m_x(x), m_y(y), m_px(px), m_py(py), m_closed(false) {}
	int m_x;
	int m_y;

	int m_px;
	int m_py;

	bool m_closed;

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
typedef std::unordered_map<Node, double, NodeHasher> CostMap;
typedef std::pair<Node, double> Node_Pair;
typedef std::vector<Node> Neighbour_Vector;

int FindPath(const int nStartX, const int nStartY,
	const int nTargetX, const int nTargetY,
	const unsigned char* pMap, const int nMapWidth, const int nMapHeight,
	int* pOutBuffer, const int nOutBufferSize);

class Graph
{
	Graph() {};

public:

	//Delete these instead of making them private -C++11 woohooo
	Graph(Graph const&) = delete;
	void operator=(Graph const&) = delete;

	void init_graph(const int width, const int height, const unsigned char* pMap)
	{
		m_width = width;
		m_height = height;
		m_pmap_size = width * height;
		m_pMap = pMap;
		m_neighbour_positions = { Node{ 0, -1 }, Node{ 1, 0 }, Node{ 0, 1 }, Node{ -1, 0 } };

		//Pre-allocate the buckets in the map -> Don't need to do this now as it's not really needed? At least it's not for the m_came_from
		//Arbritrary number..for now
		m_came_from.reserve(10);
		m_current_cost.reserve(10);

		//Reserve space for four possible neighbours
		m_neighbour_vector.reserve(4);
		m_neighbour_vector = { { -1, -1 },{ -1, -1 },{ -1, -1 },{ -1, -1 } };
		m_prune_neighbours.reserve(4);

		//I don't think i'll ever find more than 4 but reserving 10 to be safe
		m_successors.reserve(10);

		//Reserve the vector to hold nodes once they have been discovered
		m_map_rep.reserve(m_pmap_size);
		m_map_rep = std::vector<Node>(m_pmap_size, Node{ -1, -1 });
	}

	//gets the current instance.
	//As per Meyers suggestion, returning ref instead of a pointer
	static Graph& get_instance()
	{
		static Graph instance;
		return instance;
	}

	void clear_graph()
	{
		//Incase this is called twice - Clear the old graph data
		m_map_rep.clear();
		m_map_rep.reserve(0);
		m_prune_neighbours.clear();
		m_prune_neighbours.reserve(0);
		m_successors.clear();
		m_successors.reserve(0);
		m_pMap = nullptr;
		m_width = 0;
		m_height = 0;
		m_pmap_size = 0;
		//Erase the maps and reserve them back down to 0 for good measure
		m_came_from.erase(m_came_from.begin(), m_came_from.end());
		m_current_cost.erase(m_current_cost.begin(), m_current_cost.end());
		m_came_from.reserve(0);
		m_current_cost.reserve(0);
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
					//Consider using the vector here instead of creating a new object each time
					neighbour_iter->m_x = new_x;
					neighbour_iter->m_y = new_y;
					++neighbour_iter;
				}
			}
		}
	}

	void add_to_vec(const Node& current, const Node& parent)
	{
		//Put the node into its correct place in the vector
		int index = current.m_x + (current.m_y * m_width);
		auto node_to_test = m_map_rep[index];
		if (!node_to_test.m_closed)
		{
			m_map_rep[index] = Node{ current.m_x, current.m_y, parent.m_x, parent.m_y };;
			m_map_rep[index].m_closed = true;
		}
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
	Neighbour_Vector m_prune_neighbours;

	std::vector<Node> m_successors;

	Node m_bad_node;

	std::vector<Node_Pair> m_frontier_vec;

	std::vector<Node> m_map_rep;

	//Testing
	bool in_use;

};
std::vector<int> reconstruct_path(const Node& start, const Node& target, NodeMap& came_from, const int mapWidth, int* pOutBuffer, int nOutBufferSize, const std::vector<Node>& pmap_vec)
{

	if (target.m_x == -1 && target.m_y == -1)
	{
		return{};
	}
	std::vector<int> return_path{};

	Node current = pmap_vec[target.m_px + (mapWidth * target.m_py)];
	return_path.push_back((target.m_x + (target.m_y * mapWidth)));
	return_path.push_back((current.m_x + (current.m_y * mapWidth)));

	bool start_reached = false;

	while (!start_reached)
	{
		if (current.m_x < 0 || current.m_y < 0)
		{
			return{};
		}

		if (current.m_x == start.m_x && current.m_y == start.m_y)
		{
			//This is being weird in the loop above..
			break;
		}

		current = pmap_vec[current.m_px + (mapWidth * current.m_py)];
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


Node a_star_jps(Graph& graph, const Node& start, const Node& target, const unsigned char* pMap);


int FindPath(const int nStartX, const int nStartY,
	const int nTargetX, const int nTargetY,
	const unsigned char* pMap, const int nMapWidth, const int nMapHeight,
	int* pOutBuffer, const int nOutBufferSize)
{
	Node start = { nStartX, nStartY };
	Node target = { nTargetX, nTargetY };

	Graph::get_instance();

	if (!Graph::get_instance().in_use)
	{
		Graph::get_instance().init_graph(nMapWidth, nMapHeight, pMap);
		Graph::get_instance().in_use = true;

		auto target_found = a_star_jps(Graph::get_instance(), start, target, pMap);
		auto res = reconstruct_path(start, target_found, Graph::get_instance().m_came_from, nMapWidth, pOutBuffer, nOutBufferSize, Graph::get_instance().m_map_rep);

		//Not using it anymore and clear the graph of stuff
		Graph::get_instance().in_use = false;
		Graph::get_instance().clear_graph();

		return res.size() == 0 ? -1 : res.size();
	}

	return -1;
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
		return graph.m_bad_node;
	}

	graph.add_to_vec(current, parent);
	//If the current node is the target, return the target node
	if (current == target)
	{
		graph.m_came_from[target] = parent;
		return Node{ target.m_x, target.m_y, parent.m_x, parent.m_y };
	}

	if (dx != 0)
	{
		if ((graph.reachable(current.m_x, current.m_y + 1) && (!graph.reachable(current.m_x - dx, current.m_y + 1))) ||
			(graph.reachable(current.m_x, current.m_y - 1) && (!graph.reachable(current.m_x - dx, current.m_y - 1))))
		{
			//This node is a jump node, return here
			return Node{ current.m_x, current.m_y, parent.m_x, parent.m_y };
		}
	}

	//Search along y axis
	else if (dy != 0)
	{
		if ((graph.reachable(current.m_x + 1, current.m_y) && (!graph.reachable(current.m_x + 1, current.m_y - dy))) ||
			(graph.reachable(current.m_x - 1, current.m_y) && (!graph.reachable(current.m_x - 1, current.m_y - dy))))
		{
			//This node is a jump node, return here
			return Node{ current.m_x, current.m_y, parent.m_x, parent.m_y };

		}

		//Make an extra check when moving vertically for horizontal jump points
		//Refactor to just take ints?
		auto right = jump(Node{ current.m_x + 1, current.m_y }, current, target, graph);
		auto left = jump(Node{ current.m_x - 1, current.m_y }, current, target, graph);
		if (graph.valid_node(right.m_x, right.m_y) || graph.valid_node(left.m_x, left.m_y))
		{
			return Node{ current.m_x, current.m_y, parent.m_x, parent.m_y };
		}
	}

	else
	{
		// Should never happen
		return graph.m_bad_node;
	}

	//Recurse diagonally
	return jump(Node{ current.m_x + dx, current.m_y + dy }, current, target, graph);
}
void prune_neighbours(const Node& node, const Node& parent, Graph& graph)
{
	graph.m_prune_neighbours.clear();
	//Check if the current node has a parent, if it is the first node
	if (graph.valid_node(parent.m_x, parent.m_y) && node != parent)
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

	}//if !(parent)
	else
	{
		//Else just get all the neighbours
		graph.neighbours(node.m_x, node.m_y);
		graph.m_prune_neighbours = graph.m_neighbour_vector;
	}
}

void identify_successors(const Node& current, const Node& parent, const Node& target, Graph& graph)
{
	prune_neighbours(current, graph.m_came_from[parent], graph);
	graph.m_successors.clear();

	for (auto& neighbour : graph.m_prune_neighbours)
	{
		Node jump_node = jump(neighbour, current, target, graph);

		//If the node is valid, add to successors
		if (graph.valid_node(jump_node.m_x, jump_node.m_y))
		{
			graph.m_successors.push_back(jump_node);
		}

		if (!graph.m_came_from.count(neighbour))
		{
			graph.m_came_from[neighbour] = current;

			if (graph.valid_node(neighbour.m_x, neighbour.m_y))
			{
				graph.add_to_vec(neighbour, parent);
			}
		}
	}
}

//A* Search with JPS (Jump point search)
Node a_star_jps(Graph& graph, const Node& start, const Node& target, const unsigned char* pMap)
{
	graph.m_frontier.emplace(start, 0);

	graph.m_came_from[start] = start;

	//Start is index 0 
	graph.m_map_rep[0] = start;
	graph.m_current_cost[start] = 0;

	while (!graph.m_frontier.empty())
	{
		auto current = graph.m_frontier.top().first;
		graph.m_frontier.pop();

		if (current == target)
		{
			//target acquired
			return current;
		}

		//Update successors
		identify_successors(current, graph.m_came_from[current], target, graph);

		for (auto& x : graph.m_successors)
		{
			//Weights are always one
			double new_cost = graph.m_current_cost[current] + 1;
			if (!graph.m_current_cost.count(x) || new_cost < graph.m_current_cost[x])
			{
				//Don't need to add to the "came_from" path here, that's done in the jump / by the neighbours
				double priority = new_cost + manhattan_heuristic(x, target);
				graph.m_current_cost[x] = new_cost;
				graph.m_frontier.emplace(x, priority);
			}
		}
	}

	return Node{ -1, -1 };
}
