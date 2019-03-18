#include <cstdlib>
#include <cstdio>
#include <list>
#include <vector>
#include <stack>
#include <algorithm>

/*
FILLME
*/
class Graph {
	int n_routers = 0;
	int n_connections = 0;

	std::vector<std::list<int>> adjacency_list;
	std::vector<int> * d;
	std::vector<int> * low;
	std::vector<bool> * inStack;
	std::vector<bool> * importantRouters;
	int importantRouter;
	std::vector<std::vector<int>> * forests;

public:
	//Graph constructor
	Graph() {

	}

	int getNumberImportant() {
		return importantRouter;
	}

	void addNewScc(std::vector<int> scc) {
		forests->push_back(scc);
	}

	std::vector<std::vector<int>> getForests() {
		return *forests;
	}

	void addRouters(int r) {
		n_routers = r;
	}

	void addConnections(int c) {
		n_connections = c;
	}

	int getRouters() {
		return n_routers;
	}

	int getNumberConnections() {
		return n_connections;
	}

	std::vector<std::list<int>> getAdjacencyList() {
		return adjacency_list;
	}

	void init() {
		adjacency_list.resize(n_routers, std::list<int>());
		d = new std::vector<int>(getRouters(), -1);
		low = new std::vector<int>(getRouters(), -1);
		importantRouters = new std::vector<bool>(getRouters(), false);
		importantRouter = 0;
		inStack = new std::vector<bool>(getRouters(), false);
		forests = new std::vector<std::vector<int>>();
	}

	std::list<int> getEdgeListOfNode(int i) {
		return adjacency_list[i];
	}

	void addEdge(int i, int j){
		adjacency_list[i-1].push_back(j-1);
		adjacency_list[j-1].push_back(i-1);
	}
 
	void removeEdge(int i, int j) {
		adjacency_list[i-1].remove(j-1);
		adjacency_list[j-1].remove(i-1);
	}

	void tarjan() {
		int counter = 0;
		std::list<int> * stk = new std::list<int>();

		for(int u = getRouters() - 1; u > -1; u--) {
			if(d->at(u) == -1) {
				tarjanVisit(-1, u, &counter, stk);
			}
		}

		delete stk;
	}

	void tarjanVisit(int p, int u, int * counter, std::list<int> * stk) {
		int childcount = 0;
		d->at(u) = *counter;
		low->at(u) = *counter;
		(*counter)++;

		stk->push_back(u);
		inStack->at(u) = true;

		for(int adj: getEdgeListOfNode(u)) {
			if(d->at(adj) == -1) {
				childcount++;
				tarjanVisit(u, adj, counter, stk);
				low->at(u) = std::min(low->at(u), low->at(adj));

				if(((low->at(adj) >= d->at(u) && p != -1) || ((p == -1) && (childcount > 1))) && importantRouters->at(u)!=true) {
					importantRouters->at(u) = true;
					importantRouter++;
				}	
			}
			else if(inStack->at(adj) == true) {
				low->at(u) = std::min(low->at(u), d->at(adj));
			}
		}	

		if(d->at(u) == low->at(u)) {
			std::vector<int> * new_scc = new std::vector<int>();
			int tmp = -1;
			do {
				tmp = stk->back();
				stk->pop_back();
				new_scc->push_back(tmp);
			} while(tmp != u);

			addNewScc(*new_scc);
			delete new_scc;
		}
	}

	void fundamentalVisit(int u, std::vector<bool> * traverse, int * tmp) {
		traverse->at(u) = true;

		for(int j : getEdgeListOfNode(u))
			 if(!traverse->at(j)) {
			 	(*tmp)++;
				fundamentalVisit(j, traverse, tmp);
			}
	}

	int fundamentalDFS() {
		int max = 0;
		std::vector<bool> * traverse = new std::vector<bool>(getRouters(), false);
		
		for(int i = 0; i < (int) traverse->size(); i++)
			if(importantRouters->at(i) == true)
				traverse->at(i) = true;

		for(int u = 0; u < getRouters(); u++) {
			int tmp = 0;
			if(!traverse->at(u)) {
				tmp++;
				fundamentalVisit(u, traverse, &tmp);
			}
			max = std::max(tmp, max);
		}

		return max;
	}
};


// Main
int main() {
	Graph* g = new Graph();
 
	int n_routers, n_connections;
	scanf("%d", &n_routers);
	scanf("%d", &n_connections);
	g->addRouters(n_routers);
	g->addConnections(n_connections);
	g->init();
	for(int i = 0; i < n_connections; i++) {
		int n1, n2;
		scanf("%d %d", &n1, &n2);
		g->addEdge(n1, n2);
	}

	g->tarjan();

	//Number of forests
	std::vector<std::vector<int>> forests = g->getForests();
	printf("%lu\n", forests.size());
	
	//IDs of forests
	for(int i = (int) forests.size() - 1; i > -1; i--) {
		int m = 0;
		for(int j: forests.at(i)) {
			if(j > m)
				m = j;
		}
		printf("%d", m+1);
		i == 0 ? printf("\n") : printf(" ");
	}

	//Get number of routers that break the network
	printf("%d\n", g->getNumberImportant());

	printf("%d\n", g->fundamentalDFS());

	return 0;
}
