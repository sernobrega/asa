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
#include <unordered_set>

using namespace std; 

/*
 *  Edge - edge of a graph
 */
struct Edge {
	int destiny;
	int origin;
	int capacity;
	int orig_capacity;
};

/*
 * Class Graph - contains all secondary data structures and operations. Represents the graph.
 */
class Graph {
private:
	int nonodes = 0;
	int provider = 0;

	vector<list<Edge*>> residual_graph;
	vector<int> * nodes_cap;
	vector<int> * nodes_cut;

public:
	/*
	 * Graph() - constructor, initialises all the vectors
	 */
	Graph(int provider, int supplier, int connections) {
		nonodes = provider + supplier + 2;
		provider = provider;

		residual_graph.resize(nonodes, list<Edge*>());
		nodes_cap = new vector<int>(nonodes);
	}

	~Graph() {
		delete nodes_cap;
		delete nodes_cut;
	}

	int getNumberNodes() {
		return nonodes;
	}

	int getNumberProvider() {
		return provider;
	}

	vector<int> * getNodesCut() {
		return nodes_cut;
	}

	void setNodesCut(vector<int> * nodes) {
		nodes_cut = nodes;
	}

	list<Edge*> getEdgeList(int origin) {
		return residual_graph[origin];
	}

	Edge * getEdgePtr(int origin, int destiny){
		list<Edge*> edge_list = residual_graph[origin];

		for(list<Edge*>::iterator it = edge_list.begin(); it != edge_list.end(); ++it) {
			Edge * e = * it;
			if(e->destiny == destiny)
				return e;
		}
		return NULL;
	}

	void setEdge(int origin, int destiny, int capacity){
		Edge * e = new Edge();
		e->origin = origin;
		e->destiny = destiny;
		e->capacity = capacity;
		e->orig_capacity = capacity;
		residual_graph[origin].push_back(e);
		

		Edge * r = new Edge();
		r->origin = destiny;
		r->destiny = origin;
		r->capacity = 0;
		r->orig_capacity = 0;
		residual_graph[destiny].push_back(r);
	}

	void addFlow(int origin, int destiny, int capacity){
		Edge * e = getEdgePtr(origin, destiny);
		e->capacity = e->capacity + capacity;
	}

	int getCapacityValue(int origin, int destiny) {
		Edge * e = getEdgePtr(origin, destiny);
		return e->capacity;
	}

	void setNodeCapacity(int i, int cap) {
		nodes_cap->at(i) = cap;
	}


	int getNodeCapacity(int i) {
		return nodes_cap->at(i);
	}

	static Graph * createGraphFromStdin();

	/* Maximum flow  */
	bool bfs(int s, int t, vector<int> * parent);
	void fordFulkerson(int s, int t);

	/* Find limiting */
	void findFaultsGraph();
	void findEdgesCut();
	void dfs_EdgesCut(int v, vector<bool> * visited);
	void dfs_visit(int v, vector<bool> * visited);

};

/*
 *  createGraphFromStdin() - reads input and creates graph g
 */
Graph * Graph::createGraphFromStdin() {
	int n_provider, n_supplier, n_connections;
	int offset = 2;
	scanf("%d", &n_provider);
	scanf("%d", &n_supplier);
	scanf("%d", &n_connections);

	Graph* g = new Graph(n_provider, n_supplier, n_connections);

	g->setNodeCapacity(0, INT_MAX);
	g->setNodeCapacity(1, INT_MAX);

	for(int i = offset; i < n_provider + offset; i++) {
		int cap;
		scanf("%d", &cap);
		g->setNodeCapacity(i, cap);
		g->setEdge(i, 0, cap);
	}

	for(int i = n_provider + offset; i < n_supplier + n_provider + offset; i++) {
		int cap;
		scanf("%d", &cap);
		g->setNodeCapacity(i, cap);
	}

	for(int i = 0; i < n_connections; i++) {
		int orig, dest, cap;
		scanf("%d %d %d", &orig, &dest, &cap);
		g->setEdge(dest, orig, cap);
	}

	return g;
}

bool Graph::bfs(int s, int t, vector<int> * parent) {

	vector<bool> * visited = new vector<bool>(getNumberNodes(), false);

	deque<int> * q = new deque<int>();
	q->push_back(s);
	visited->at(s) = true;
	parent->at(s) = -1;
	
	while(!q->empty()) {
		int u = q->front();
		q->pop_front();

		list<Edge*> edge_list = getEdgeList(u);

		for(list<Edge*>::iterator it = edge_list.begin(); it != edge_list.end(); ++it) {
			Edge * e = *it;
			int v = e->destiny;
			if(getNodeCapacity(v) > 0 && e->capacity > 0 && !visited->at(v)) {
				q->push_back(v);
				parent->at(v) = u;
				visited->at(v) = true;
			}
		}
	}

	return visited->at(t);
}

void Graph::fordFulkerson(int s, int t) {
	int max_capacity = 0;
	

	vector<int> * parent = new vector<int>(getNumberNodes(), 0);

	while(bfs(s, t, parent)) {
		int u, v;
		int path_capacity = INT_MAX;

		for(v = t; v != s; v = parent->at(v)) {
			u = parent->at(v);

			path_capacity = min(path_capacity, getNodeCapacity(u));
			path_capacity = min(path_capacity, getCapacityValue(u, v));
		}

		/*
		 *  Build residual graph
		 */
		for(v = t; v != s; v = parent->at(v)) { 
			u = parent->at(v);

			setNodeCapacity(u, getNodeCapacity(u) - path_capacity);
			addFlow(u, v, -path_capacity);
			addFlow(v, u, path_capacity);
		}

		max_capacity += path_capacity;
	}

	printf("%d\n", max_capacity);
}

void Graph::findFaultsGraph() {
	vector<bool> * visited = new vector<bool>(getNumberNodes(), false);
	unordered_set<int> * nodes_cut_tmp = new unordered_set<int>();

	dfs_visit(1, visited);

	for(int i = 2; i < getNumberNodes(); i++) {
		if(visited->at(i)) {
			list<Edge*> edge_list = getEdgeList(i);
			for(list<Edge*>::iterator it = edge_list.begin(); it != edge_list.end(); ++it) {
				Edge * e = *it;
				int u = e->destiny;
				if(u != 0 && !visited->at(u) &&  getNodeCapacity(u) == 0 && getNodeCapacity(i) == 0 && e->orig_capacity > e->capacity) {
					nodes_cut_tmp->insert(i);
				}
			}
		}
	}

	vector<int> * ordered_nodes = new vector<int>(nodes_cut_tmp->begin(), nodes_cut_tmp->end());
	sort(ordered_nodes->begin(), ordered_nodes->end());

	/*
	 * Print nodes that need to be augmented
	 */
	for(unsigned i = 0; i< ordered_nodes->size(); i++) {
        printf("%d", ordered_nodes->at(i));
        if(i != ordered_nodes->size() - 1)printf(" ");
    }
    printf("\n");

    setNodesCut(ordered_nodes);

	delete visited;
	delete nodes_cut_tmp;
}

void Graph::dfs_visit(int v, vector<bool> * visited) {
    visited->at(v) = true; 

  	list<Edge*> edge_list = getEdgeList(v);
  	for(list<Edge*>::iterator it = edge_list.begin(); it != edge_list.end(); ++it) {
  		Edge * e = *it;
  		int u = e->destiny;

        if (u > 0 && getNodeCapacity(v) != 0 && !visited->at(u) && e->capacity > 0) {
            dfs_visit(u, visited);
        }
    }
}

void Graph::findEdgesCut() {
	vector<bool> * visited = new vector<bool>(getNumberNodes(), false);
	unordered_set<Edge*> * edges_cut = new unordered_set<Edge*>();

	dfs_EdgesCut(1, visited);

	for(int i = 1; i < getNumberNodes(); i++) {
		if(visited->at(i)) {

			list<Edge*> edge_list = getEdgeList(i);
			for(list<Edge*>::iterator it = edge_list.begin(); it != edge_list.end(); ++it) {
				Edge * e = *it;
				int u = e->destiny;
				Edge * r = getEdgePtr(i, u);
				if(u != 0 && !visited->at(u) && e->orig_capacity > 0) {
					vector<int> * nodes_cut = getNodesCut();
					if (!(find(nodes_cut->begin(), nodes_cut->end(), i) != nodes_cut->end())) {
						edges_cut->insert(r);
					}

				}
			}
		}
		
	}

	vector<Edge*> * ordered_edges = new vector<Edge*>(edges_cut->begin(), edges_cut->end());
	sort(ordered_edges->begin(), ordered_edges->end(), 
		[](const Edge * item1, const Edge * item2) {
			return item1->destiny < item2->destiny || (item1->destiny == item2->destiny && item1->origin < item2->origin );
		}
	);

	for(unsigned i = 0; i< ordered_edges->size(); i++) {
		Edge * e = ordered_edges->at(i);
		printf("%d %d\n", e->destiny, e->origin);
    }

    delete visited;
    delete edges_cut;
    delete ordered_edges;
}

void Graph::dfs_EdgesCut(int v, vector<bool> * visited) {
	visited->at(v) = true; 

  	list<Edge*> edge_list = getEdgeList(v);
  	for(list<Edge*>::iterator it = edge_list.begin(); it != edge_list.end(); ++it) {
  		Edge * e = * it;
  		int u = e->destiny;
        if (u > 0 && !visited->at(u) && e->capacity > 0) {
            dfs_EdgesCut(u, visited);
        }
    }
}


/*
 * Main
 */
int main() {
	/*
	 * Read input and create Graph g
	 */
	Graph * g = Graph::createGraphFromStdin();
	g->fordFulkerson(1, 0);
	g->findFaultsGraph();
	g->findEdgesCut();

	delete g;

	return 0;
}
