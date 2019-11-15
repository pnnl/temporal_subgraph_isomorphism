#include "GraphMatch.h"
#include <iostream>

using namespace std;

void GraphMatch::addEdge(const Edge &gEdge, const Edge &hEdge)
{
    int g_i = gEdge.index(), h_i = hEdge.index();
    _edges.push_back(g_i);
    _gEdge2hEdgeMap[g_i] = h_i;       
    
    int g_u = gEdge.source(), g_v = gEdge.dest();
    int h_u = hEdge.source(), h_v = hEdge.dest();
    
    _gNode2hNodeMap[g_u] = h_u;
    _gNode2hNodeMap[g_v] = h_v;
    _nodes.push_back(g_u);
    _nodes.push_back(g_v);
    _nodeSet.insert(g_u);
    _nodeSet.insert(g_v);
}

bool GraphMatch::hasNode(int u) const
{
    return _nodeSet.find(u) != _nodeSet.end();
}

void GraphMatch::disp() const
{
    cout << "NodeSet = ";
    for(int u : _nodeSet)
	cout << u << " ";
    cout << endl;
    cout << "Nodes = ";
    for(int u : _nodes)
	cout << u << " ";
    cout << endl;
}
