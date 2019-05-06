/*
 * 1st project of Analysis and Synthesis of Algorithms 2018/2019
 * Instituto Superior Tecnico - ULisboa
 *
 * Project realised by Sergio Nobrega 86806
 * More about this project can be found at Github @sernobrega
 * 
 * Main goals of this project were to find SCCs and cut vertices in a graph. Algorithms used were Trajan and DFS.
 *
 */

#include <cstdio>
#include <list>
#include <vector>
#include <algorithm>

/*
 * Class Graph - contains all secondary data structures for Trajan and DFS, stores the SCCs and cut vertices of the graph.
 */
class Graph {
	int n_routers = 0;
	int n_connections = 0;
	int n_cut = 0;

	std::vector<std::list<int>> adjacency_list;
	std::vector<int> * d;
	std::vector<int> * low;
	std::vector<bool> * visited;
	std::vector<bool> * cutRouters;
	std::vector<std::vector<int>> * v_sccs;

public:
	/*
	 * Graph() - constructor, initialises all the vectors
	 */
	Graph(int routers, int connections) {
		n_routers = routers;
		n_connections = connections;

		adjacency_list.resize(n_routers, std::list<int>());
		d = new std::vector<int>(getRouters(), -1);
		low = new std::vector<int>(getRouters(), -1);
		cutRouters = new std::vector<bool>(getRouters(), false);
		visited = new std::vector<bool>(getRouters(), false);
		v_sccs = new std::vector<std::vector<int>>();
	}

	~Graph() {
		delete visited;
		delete d;
		delete v_sccs;
		delete cutRouters;
		delete low;
	}

	/*
	 * getRouters() get the number of vertices
	 */
	int getRouters() {
		return n_routers;
	}

	/*
	 * getNumberConnections() get the number of connections
	 */
	int getNumberConnections() {
		return n_connections;
	}

	/*
	 * getAdjacencyList() get adjacency list
	 */
	std::vector<std::list<int>> getAdjacencyList() {
		return adjacency_list;
	}

	/*
	 * getNumberCut() get number of cut vertices
	 */
	int getNumberCut() {
		return n_cut;
	}

	/*
	 * getSCCs() gets SCCs of graph
	 */
	std::vector<std::vector<int>> getSCCs() {
		return *v_sccs;
	}

	/*
	 * getEdgeListOfNode(int) get edges of a vertice
	 */
	std::list<int> getEdgeListOfNode(int i) {
		return adjacency_list[i];
	}

	/*
	 * addEdge(int, int) add an edge to the graph
	 */
	void addEdge(int i, int j){
		adjacency_list[i-1].push_back(j-1);
		adjacency_list[j-1].push_back(i-1);
	}

	/*
	 * tarjan() executes Tarjan algorithm
	 */
	void tarjan() {
		int counter = 0;
		std::list<int> * stk = new std::list<int>();

		for(int u = getRouters() - 1; u >= 0; u--) {
			if(d->at(u) == -1) {
				tarjanVisit(u, true, &counter, stk);
			}
		}

		delete stk;
	}

	/*
	 * tarjanVisit(int, int, int *, list<int> *) auxiliary method, recursive
	 * Calculates the cut vertices and creates the SCCs
	 */
	void tarjanVisit(int u, bool p, int * counter, std::list<int> * stk) {
		int childcount = 0;
		d->at(u) = *counter;
		low->at(u) = *counter;
		(*counter)++;

		stk->push_back(u);
		visited->at(u) = true;

		std::list<int> edge_list = getEdgeListOfNode(u);
		for(std::list<int>::iterator it = edge_list.begin(); it != edge_list.end(); ++it) {
			int adj = *it;
			if(d->at(adj) == -1) {
				childcount++;
				tarjanVisit(adj, false, counter, stk);
				low->at(u) = std::min(low->at(u), low->at(adj));

				//Conditions to be a cut vertex:
				//1. Root of a tree and has more than 1 child, or
				//2. The son vertex has a higher low[] than the father
				if(((low->at(adj) >= d->at(u) && !p) || (p && (childcount > 1))) && cutRouters->at(u)!=true) {
					cutRouters->at(u) = true;
					n_cut++;
				}	
			}
			else if(visited->at(adj) == true) {
				low->at(u) = std::min(low->at(u), d->at(adj));
			}
		}	

		if(d->at(u) == low->at(u) && stk->size() > 0) {
			std::vector<int> * new_scc = new std::vector<int>();
			int tmp = -1;
			do {
				tmp = stk->back();
				stk->pop_back();
				new_scc->push_back(tmp);
			} while(tmp != u && stk->size() > 0);

			v_sccs->push_back(*new_scc);
			delete new_scc;
		}
	}

	/*
	 * fundamentalDFS() - calculates the number of nodes of the biggest SCC when removing all the cut vertices
	 */
	int fundamentalDFS() {
		int max = 0;
		std::vector<bool> * traverse = new std::vector<bool>(getRouters(), false);
		
		//Marking cut vertices as already visited so that they will be ignored by the algorithm
		for(int i = 0; i < (int) traverse->size(); i++)
			if(cutRouters->at(i) == true)
				traverse->at(i) = true;

		for(int u = 0; u < getRouters(); u++) {
			int tmp = 0;
			if(!traverse->at(u)) {
				tmp++;
				fundamentalVisit(u, traverse, &tmp);
			}
			max = std::max(tmp, max);
		}

		delete traverse;

		return max;
	}

	/*
	 * fundamentalVisit(int, std::vector<bool>*, int *) - auxiliary method for fundamentalDFS
	 */
	void fundamentalVisit(int u, std::vector<bool> * traverse, int * tmp) {
		traverse->at(u) = true;

		for(int j : getEdgeListOfNode(u))
			 if(!traverse->at(j)) {
			 	(*tmp)++;
				fundamentalVisit(j, traverse, tmp);
			}
	}
};


/*
 * Main
 */
int main() {
	int n_routers, n_connections;

	/*
	 * Read input and create Graph g
	 */
	scanf("%d", &n_routers);
	scanf("%d", &n_connections);
	Graph* g = new Graph(n_routers, n_connections);
	for(int i = 0; i < n_connections; i++) {
		int n1, n2;
		scanf("%d %d", &n1, &n2);
		g->addEdge(n1, n2);
	}

	/*
	 * Execute Tarjan
	 */
	g->tarjan();

	/*
	 * Get the SCCs vector and print the number of SCCs
	 */
	std::vector<std::vector<int>> v_sccs = g->getSCCs();
	printf("%lu\n", v_sccs.size());
	
	/*
	 * Get maximum of each SCC and put in an auxiliary vector
	 */
	for(int i = (int) v_sccs.size() - 1; i > -1; i--) {
		int m = 0;
		for(int j: v_sccs.at(i)) {
			if(j > m)
				m = j;
		}
		printf("%d", m+1);
		i == 0 ? printf("\n") : printf(" ");
	}

	/*
	 * Print number of cut vertices
	 */
	printf("%d\n", g->getNumberCut());

	/*
	 * Execute DFS and print number of nodes of biggest SCC when excluding all SCCs
	 */
	printf("%d\n", g->fundamentalDFS());


	delete g;


	return 0;
}
