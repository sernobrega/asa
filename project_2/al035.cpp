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
	int flow;
};

/*
 * Class Graph - contains all secondary data structures and operations. Represents the graph.
 */
class Graph {
	int n_connections = 0;
	int n_supplier = 0;
	int n_provider = 0;

	std::vector<std::list<Edge*>> adjacency_list;
	
	vector<int> * nodes_cap;

public:
	/*
	 * Graph() - constructor, initialises all the vectors
	 */
	Graph(int provider, int supplier, int connections) {
		n_provider = provider;
		n_supplier = supplier;
		n_connections = connections;

		adjacency_list.resize(provider + supplier + 2, std::list<Edge*>());
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

	list<Edge*> getEdgeList(int origin) {
		return adjacency_list[origin];
	}

	/*
	 * setEdge(int, int, int, int) add an edge to the graph
	 */
	Edge * getEdgePtr(int origin, int destiny){
		list<Edge*> edge_list = adjacency_list[origin];

		for(list<Edge*>::iterator it = edge_list.begin(); it != edge_list.end(); ++it) {
			Edge * e = * it;
			if(e->destiny == destiny)
				return e;
		}
		return NULL;
	}

	/*
	 * setEdge(int, int, int, int) add an edge to the graph
	 */
	void setEdge(int origin, int destiny, int flow){
		Edge * e = new Edge();
		e->origin = origin;
		e->destiny = destiny;
		e->flow = flow;
		adjacency_list[origin].push_back(e);
	}

	/*
	 * setEdge(int, int, int, int) add an edge to the graph
	 */
	void addFlow(int origin, int destiny, int flow){
		Edge * e = getEdgePtr(origin, destiny);
		e->flow = e->flow + flow;
	}

	/*
	 *	getCapacityValue(int, int) get capacity value of a given connection
	 */
	int getCapacityValue(int origin, int destiny) {
		Edge * e = getEdgePtr(origin, destiny);
		return e->flow;
	}

	void setNodeCapacity(int i, int cap) {
		nodes_cap->at(i) = cap;
	}

	int getNodeCapacity(int i) {
		return nodes_cap->at(i);
	}

	int getProviderNo() {
		return n_provider;
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
		g->setEdge(0, i + 2, cap);
		g->setEdge(i + 2, 0, cap);
	}

	for(int i = 0; i < n_supplier; i++) {
		int cap;
		scanf("%d", &cap);
		g->setNodeCapacity(i + n_provider, cap);
	}

	for(int i = 0; i < n_connections; i++) {
		int orig, dest, cap;
		scanf("%d %d %d", &orig, &dest, &cap);
		g->setEdge(dest, orig, cap);
		g->setEdge(orig, dest, 0);
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

			list<Edge*> edge_list = g->getEdgeList(u);

			for(std::list<Edge*>::iterator it = edge_list.begin(); it != edge_list.end(); ++it) {
				Edge * e = *it;
				int v = e->destiny;
				if(((v > 1 && g->getNodeCapacity(v - 2) > 0) || v < 2) && e->flow > 0 && !visited->at(v)) {
					q->push_back(v);
					parent->at(v) = u;
					visited->at(v) = true;
					
				}

			}
		}

		return visited->at(t) == true;
	}

	static int fordfulkerson(Graph * g) {
		int max_capacity = 0;

		int u, v;
		int s = 1, t = 0;
		
		vector<int> * parent = new vector<int>(g->getNumberNodes(), 0);

		while(bfs(g, s, t, parent)) {

			int path_capacity = INT_MAX;

			for(v = t; v != s; v = parent->at(v)) {
				u = parent->at(v);
				
				if(u > 1) {
					path_capacity = min(path_capacity, g->getNodeCapacity(u - 2));
				}
				path_capacity = min(path_capacity, g->getCapacityValue(u, v));
			}

			/*
			 *  Build residual graph
			 */
			for(v = t; v != s; v = parent->at(v)) { 
				u = parent->at(v);
				if(u > 1) 
				 	g->setNodeCapacity(u - 2, g->getNodeCapacity(u - 2) - path_capacity);
				
				g->addFlow(u, v, - path_capacity);
				g->addFlow(v, u, path_capacity);

			}

			max_capacity += path_capacity;
		}
		return max_capacity;
	}
};

class DFS {
public:


	static void oi(Graph * g) {
		for(int v = 0; v < g->getNumberNodes(); v++) {
			list<Edge*> ed = g->getEdgeList(v);
		  	for(list<Edge*>::iterator it = ed.begin(); it != ed.end(); ++it) {
		  		Edge * e = *it;
		  		int u = e->destiny;
				if(v > g->getProviderNo() + 1 && g->getNodeCapacity(v - 2) == 0 && g->getEdgePtr(v, u)->flow > 0) {
		  				printf("%d\n", v);	
		  		}
		  	}
		}

		

	}

	static void findFaultsGraph(Graph * g) {

		for(int i = 2; i < g->getProviderNo() + 2; i++) {
			vector<bool> * visited = new vector<bool>(g->getNumberNodes(), false);
			deque<int> * path = new deque<int>();

			//printf("New DFS is about to start\n");

			dfs_visit(g, 1, i, visited, path);

			while(!path->empty()) {
				printf("%d ", path->front());
				path->pop_front();
			}
			printf("\n");

			// for(int x = 0; x < g->getNumberNodes(); x++) {
			// 	if(visited->at(x) == true)
			// 	{
			// 		//printf("%d\n", x);
			// 	}
			// }

			delete path;
			delete visited;
		}
		

		
	}


	static void dfs_visit(Graph * g, int v, int t, vector<bool> * visited, deque<int> * path) {
	    visited->at(v) = true;
	    path->push_back(v);

	  	list<Edge*> edge_list = g->getEdgeList(v);
	  	for(list<Edge*>::iterator it = edge_list.begin(); it != edge_list.end(); ++it) {
	  		Edge * e = *it;
	  		int u = e->destiny;
	  		if(!visited->at(u) && e->flow > 0 && e->destiny != 0 && e->origin != 0) {


	  			//printf("%d %d %d\n", e->origin, e->destiny, e->flow);
	  			dfs_visit(g, u, t, visited, path); 
	  		}

	  // 		//IDEA: Get a DFS path and if it finds, run this for each of the vertices
	  // 		if(v > g->getProviderNo() + 1 && g->getNodeCapacity(v - 2) == 0 && g->getEdgePtr(v, u)->flow > 0) {
			// 	printf("%d\n", v);	
			// }
		}

		

		if(visited->at(t) != true) {

			path->pop_back();
		}
	}
};

/*
 * Main
 */
int main() {
	/*
	 * Read input and create Graph g
	 */
	Graph * g = readInput();
	printf("%d\n", EdmondsKarp::fordfulkerson(g));
	//DFS::oi(g);
	DFS::findFaultsGraph(g);

	delete g;

	return 0;
}
