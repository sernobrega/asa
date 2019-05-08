/*
 * 2nd project of Analysis and Synthesis of Algorithms 2018/2019
 * Instituto Superior Tecnico - ULisboa
 *
 * Project realised by Sergio Nobrega 86806
 * More about this project can be found at Github @sernobrega
 * 
 * The main goals of the project are to:
 * 1. Calculate the maximum flow of a network
 * 2. Find connections that might need augmenting
 * 3. Find nodes that might need augmenting the capacity of flow
 *
 */

#include <cstdio>
#include <list>
#include <vector>
#include <algorithm>
#include <stdlib.h>
#include <climits>
#include <queue>
#include <list>

using namespace std; 

struct Edge {
	int origin;
	int destiny;
	int capacity;
	int flow;
};

/*
 * Class Graph - contains all secondary data structures and operations. Represents the graph.
 */
class Graph {
	int n_connections = 0;

	int n_supplier = 0;
	int n_provider = 0;

	deque<int> * nodes_aug = new deque<int>();

	list<Edge> * conn_aug;

	std::vector<std::list<Edge>> adjacency_list;
	vector<vector<int>> graph_rep;
	vector<int> * nodes_cap;

public:
	/*
	 * Graph() - constructor, initialises all the vectors
	 */
	Graph(int provider, int supplier, int connections) {
		n_provider = provider;
		n_supplier = supplier;
		n_connections = connections;

		/* index 0 - source
		 * index 1 - sink
		 * other index - providers (2 to n_provider + 2) and suppliers (2 + n_providers + 1 to n_supplier + n_provider + 2)
		 */
		graph_rep = vector<vector<int>>(supplier + provider + 2, vector<int>(supplier + provider + 2, 0));
		adjacency_list.resize(provider + supplier + 2, std::list<Edge>());
		nodes_cap = new vector<int>(supplier + provider, -1);
	}

	~Graph() {
		delete nodes_cap;
	}

	/*
	 * getNumberNodes() get the number of nodes
	 */
	int getNumberNodes() {
		return n_provider + n_supplier + 2;
	}

	/*
	 * getNumberProviders() get the number of providers
	 */
	int getNumberProviders() {
		return n_provider;
	}

	/*
	 * getNumberSuppliers() get the number of suppliers
	 */
	int getNumberSuppliers() {
		return n_supplier;
	}

	/*
	 * getNumberConnections() get the number of connections
	 */
	int getNumberConnections() {
		return n_connections;
	}

	/*
	 * setEdge(int, int, int, int) add an edge to the graph
	 */
	Edge getEdge(int origin, int destiny){
		list<Edge> edge_list =adjacency_list[origin];

		for(list<Edge>::iterator it = edge_list.begin(); it != edge_list.end(); ++it) {
			Edge e = * it;
			if(e.destiny == destiny)
				return e;
		}
	}

	/*
	 * setEdge(int, int, int, int) add an edge to the graph
	 */
	void setEdge(int origin, int destiny, int flow, int cap){
		graph_rep[origin][destiny] = cap;
		Edge e = {origin, destiny, flow, cap};
		adjacency_list[origin].push_back(e);
	}
	
	/*
	 * setEdge(int, int) add an edge to the graph FIXME
	 */
	void setEdge(int origin, int destiny, int cap){
		graph_rep[origin][destiny] = cap;
	}

	/*
	 *	getCapacityValue(int, int) get capacity value of a given connection
	 */
	int getCapacityValue(int origin, int destiny) {
		return graph_rep[origin][destiny];
	}

	void setNodeCapacity(int i, int cap) {
		nodes_cap->at(i) = cap;
	}

	int getNodeCapacity(int i) {
		return nodes_cap->at(i);
	}

	void addNodeAug(int i) {
		nodes_aug->push_back(i);
	}

	void listNodeAug() {
		// for(int i = 0; i < nodes_aug->size(); i++) {
		// 	printf("%d ", nodes_aug->front());
		// 	nodes_aug->pop_front();
		// }
		// printf("\n");
	}
};

/*
 *  readInput() - reads input and creates graph g
 */
Graph * readInput() {
	int n_provider, n_supplier, n_connections;
	scanf("%d", &n_provider);
	scanf("%d", &n_supplier);
	scanf("%d", &n_connections);

	Graph* g = new Graph(n_provider, n_supplier, n_connections);

	for(int i = 0; i < n_provider; i++) {
		int cap;
		scanf("%d", &cap);
		g->setNodeCapacity(i, cap);
		g->setEdge(0, i + 2, INT_MAX);
		g->setEdge(0, i + 2, 0, INT_MAX);
	}

	for(int i = 0; i < n_supplier; i++) {
		int cap;
		scanf("%d", &cap);
		g->setNodeCapacity(i + n_provider, cap);
	}

	for(int i = 0; i < n_connections; i++) {
		int orig, dest, cap;
		scanf("%d %d %d", &orig, &dest, &cap);
		g->setEdge(orig, dest, cap);
		g->setEdge(orig, dest, 0, cap);
	}

	return g;
}


class EdmondsKarp {
public:
	static bool bfs(Graph * g, int s, int t, vector<int> * parent) {
		vector<bool> * visited = new vector<bool>(g->getNumberNodes(), false);

		deque<int> * q = new deque<int>();
		q->push_back(s);
		visited->at(s) = true;
		parent->at(s) = -1;

		while(!q->empty()) {
			int u = q->front();
			q->pop_front();

			for(int v = 1; v < g->getNumberNodes(); v++) {
				if(((v > 1 && g->getNodeCapacity(v - 2) > 0) || v < 2) && g->getCapacityValue(u, v) > 0 && !visited->at(v)) {
					q->push_back(v);
					parent->at(v) = u;
					visited->at(v) = true;
				}
			}
		}

		return visited->at(1) == true;
	}

	static int fordfulkerson(Graph * g) {
		int max_capacity = 0;

		int u, v;
		
		vector<int> * parent = new vector<int>(g->getNumberNodes() + 1, 0);

		while(bfs(g, 0, 1, parent)) {

			int path_capacity = INT_MAX;
			/*
			 *  Calculate bottleneck
			 */
			for(v = 1; v != 0; v = parent->at(v)) {
				u = parent->at(v);
				if(u > 0) {
					path_capacity = min(path_capacity, g->getNodeCapacity(u - 2));
				}
				path_capacity = min(path_capacity, g->getCapacityValue(u, v));
			}

			/*
			 *  Build residual graph
			 */
			for(v = 1; v != 0; v = parent->at(v)) { 
				u = parent->at(v);

				if(u > 0) 
				 	g->setNodeCapacity(u - 2, g->getNodeCapacity(u - 2) - path_capacity);
				
				g->setEdge(u, v, g->getCapacityValue(u, v) - path_capacity);
				g->setEdge(v, u, g->getCapacityValue(v, u) + path_capacity);
			}

			// 	for(int x = 0; x < g->getNumberNodes(); x++) {
			// 	for(int y = 0; y < g->getNumberNodes(); y++) {
			// 		printf("%d ", g->getCapacityValue(x, y));
			// 	}
			// 	printf("\n");
			// }
			max_capacity += path_capacity;
		}
		return max_capacity;
	}
};

void transposeGraph(Graph * g) {
	// int u, v;
	// for(u = 0; u < g->getNumberNodes(); u++) {
	// 	for(v = 0; u < g->getNumberNodes(); u++) {
	// 		// g->getCapacityValue(u, v)
	// 		// g->getCapacityValue()
	// 	}
	// }
}

/*
 * Main
 */
int main() {
	/*
	 * Read input and create Graph g
	 */
	Graph * g = readInput();
	printf("%d\n", EdmondsKarp::fordfulkerson(g));
	g->listNodeAug();


	// for(int x = 0; x < g->getNumberNodes(); x++) {
	// 		for(int y = 0; y < g->getNumberNodes(); y++) {
	// 			printf("%d ", g->getCapacityValue(x, y));
	// 		}
	// 		printf("\n");
	// 	}

	delete g;

	return 0;
}
