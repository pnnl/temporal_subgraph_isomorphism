#include "Graph.h"
#include "Node.h"
#include <algorithm>
#include <iostream>
#include <time.h>
#include <limits.h>

using namespace std;

Graph::Graph(int windowDuration)
{
    _edgesReady = false;
    _windowDuration = windowDuration;
    _windowStart = 0;
    _windowEnd = 0;
    if(_windowDuration == 0)
    {
	_windowEnd = LONG_MAX;
    }
}

void Graph::addNode(int v)
{
    if(v >= _nodes.size())
        _nodes.resize(v+1);
}

void Graph::addEdge(int u, int v)
{
    this->addEdge(u, v, (time_t)_numEdges);
}

void Graph::addEdge(int u, int v, time_t dateTime)
{
    // No self loops for now
    //if(u == v)
        //throw "No self loops allowed";
    if(u < 0 || v < 0)
        throw "Vertices must be >= 0";
    
    // Calculate what the min number of nodes should be
    int min_n = std::max(u,v) + 1;
    // Resize as necessary
    if(_nodes.size() < min_n)
        _nodes.resize(min_n);
    
    // Add to the sorted map, based on time
    _timeEdgeMap[dateTime].push_back(Edge(_numEdges,u,v,dateTime));
    _numEdges++;
    
    _edgesReady = false;

    // Update window start/end if necessary
    if(dateTime > _windowEnd)
    {
	_windowEnd = dateTime;
	_windowStart = _windowEnd - _windowDuration;
    }
    
    // Index of new edge
    //int edge_index = _edges.size();
    // Create edge
    //_edges.push_back(Edge(edge_index,u,v));
    
    // Add to nodes
    /*_nodes[u].edges().push_back(edge_index);
    _nodes[v].edges().push_back(edge_index);
    _nodes[u].outEdges().push_back(edge_index);
    _nodes[v].inEdges().push_back(edge_index);*/
}

void Graph::copyEdge(int edgeIndex, const Graph &g)
{
    const Edge &edge = g.edges()[edgeIndex];
    this->addEdge(edge.source(), edge.dest(), edge.time());    
}

bool Graph::hasEdge(int u, int v) const
{
    if(!_edgesReady)
        this->updateOrderedEdges();
    
    return _nodeEdges.find(u) != _nodeEdges.end() && _nodeEdges[u].find(v) != _nodeEdges[u].end();
}

const vector<int> &Graph::getEdgeIndexes(int u, int v) const
{
    if(!_edgesReady)
        this->updateOrderedEdges();
    
    if(!hasEdge(u,v))
        throw "There are no edges between the vertices selected.";
    
    return _nodeEdges[u][v];
}

const vector<Edge> &Graph::edges() const
{
    if(!_edgesReady)
    {
        this->updateOrderedEdges();
    }
    return _edges;
}

void Graph::disp() const
{
    if(!_edgesReady)
        this->updateOrderedEdges();
    
    cout << _nodes.size() << " nodes" << endl;
    cout << _edges.size() << " edges:" << endl;
    for(int i=0; i<_edges.size(); i++)
    {
        cout << "  ";
        disp(i);
    }
}

void Graph::disp(int edgeIndex) const
{
    if(!_edgesReady)
        this->updateOrderedEdges();
    
    const Edge &edge = _edges[edgeIndex];
    cout << "[" << edgeIndex << "] " << edge.source() << " -> " << edge.dest() << endl;
}

void Graph::dispDateTimeRange() const
{
    time_t start = this->edges().front().time();
    struct tm *startTM = gmtime(&start);
    cout << "Total Data Ingested Start Date/Time: (" << start << ") " << asctime(startTM);
    time_t end = this->edges().back().time();    
    struct tm *endTM = gmtime(&end);
    cout << "Total Data Ingested End Date/Time: (" << end << ") " << asctime(endTM);

    time_t wStart = windowStart();
    struct tm *wStartTM = gmtime(&wStart);
    cout << "Current Window Start Date/Time: (" << wStart << ") " << asctime(wStartTM);
    time_t wEnd = windowEnd();    
    struct tm *wEndTM = gmtime(&wEnd);
    cout << "Current Window End Date/Time: (" << wEnd << ") " << asctime(wEndTM);
}

void Graph::updateOrderedEdges() const
{
//    cout << "Sorting edges chronologically." << endl;
    
    int n = _nodes.size();
    
    // Clear old edges
    _edges.clear();
    for(Node &node : _nodes)
    {
        node.edges().clear();
        node.outEdges().clear();
        node.inEdges().clear();
    }
    
    // Build edge list in chronological order
    int edge_index = 0;
    for(auto &pair : _timeEdgeMap)
    {
        time_t dateTime = pair.first;
        const vector<Edge> &edges = _timeEdgeMap.find(dateTime)->second;
        for(const Edge &edge : edges)
        {
            // Create new edge now that we know the time index
            Edge newEdge(edge_index, edge.source(), edge.dest(), edge.time());
            _edges.push_back(newEdge);
            _edgeTimes.push_back(dateTime);
            
            int u = edge.source(), v = edge.dest();
            
            _nodes[u].edges().push_back(edge_index);
            _nodes[v].edges().push_back(edge_index);
            _nodes[u].outEdges().push_back(edge_index);
            _nodes[v].inEdges().push_back(edge_index);
            
            edge_index++;
        }
    }
    
    // Setup nodeEdges map    
    this->_nodeEdges.clear();
    for(int u=0; u<n; u++)
    {
        const Node &node = _nodes[u];
        for(int e : node.outEdges())            
        {
            const Edge &edge = _edges[e];
            int v = edge.dest();
            _nodeEdges[u][v].push_back(e);
        }
    }
    
    // Make sure we flag the edges as ready now, so we don't redo this every time
    _edgesReady = true;
}

time_t Graph::windowStart() const
{
    time_t dataStart = _edgeTimes.front();
    if(dataStart > _windowStart)
	return dataStart;
    return _windowStart;
}

time_t Graph::windowEnd() const
{
    time_t dataEnd = _edgeTimes.back();
    if(dataEnd < _windowEnd)
	return dataEnd;
    return _windowEnd;
}

void Graph::setWindowDuration(int duration)
{
    if(duration == _windowDuration)
	return;

    // If it's shorter, truncate from the end
    if(duration < _windowDuration)
	_windowStart = _windowEnd - duration;
    // If it's longer, expand from the start
    else
	_windowEnd = _windowStart + duration;

    _windowDuration = duration;
}
