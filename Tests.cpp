#include "Tests.h"
#include "CertGraph.h"
#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <string>

using namespace std;

int Tests::countDuplicateEdges(const CertGraph &g)
{
    int m = g.numEdges();
    if(m <= 1)
	return 0;

    int dupCount = 0;

    unordered_map<int, unordered_map<int, unordered_map<string, unordered_set<time_t> > > > edgeMap;
    for(int e=1; e<m; e++)
    {
	const Edge &edge = g.edges()[e];
	int u = edge.source();
	int v = edge.dest();
	const string &type = g.getEdgeType(e);
	time_t t = edge.time();
	auto &edgeSet = edgeMap[u][v][type];
	if(edgeSet.find(t) != edgeSet.end())
	{
	    dupCount++;
	    cout << "Duplicate Edge: " << g.getLabel(u) << " -> " << g.getLabel(v) << " [" << type << "] time(" << t << ")" << endl;
	}
	else
	    edgeSet.insert(t);
    }

    return dupCount;
}
