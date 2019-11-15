/* 
 * File:   Graph.h
 * Author: D3M430
 *
 * Created on January 13, 2017, 11:32 AM
 */

#ifndef GRAPH_H
#define	GRAPH_H

#include <vector>
#include <map>
#include <unordered_map>
#include <time.h>
#include "Node.h"
#include "Edge.h"

/**
 * Our standard, directed graph, where edges are listed in the order they
 * occurred in.
 */
class Graph
{
public:
    /** Constructor.
     * @param windowDuration  Max amount of date/time in graph analytics. 
     *                        If 0, do no windowing. */
    Graph(int windowDuration=0);
    /** Makes sure at least v+1 nodes are accounted for. */
    virtual void addNode(int v);
    /** Adds edge, and resizes the nodes, as necessary */
    virtual void addEdge(int u, int v);
    /** Adds edge, and resizes the nodes, as necessary */
    virtual void addEdge(int u, int v, time_t dateTime);
    /** Copies the given edge from another graph (useful for making sure metadata is consistent */
    virtual void copyEdge(int edgeIndex, const Graph &g);
    /** Returns false if no edge exists between the vertices */    
    virtual bool hasEdge(int u, int v) const;
    /** Returns a list of all edges between the two points (directed) */    
    virtual const std::vector<int> &getEdgeIndexes(int u, int v) const;
    /** Creates a subgraph from the given set of nodes */
    //virtual Graph createSubGraph(const std::vector<int> &nodes);
    /** List of all nodes in the graph */
    virtual const std::vector<Node> &nodes() const { return _nodes; }
    /** Ordered list of all edges (sorted by order of occurrence) */
    virtual const std::vector<Edge> &edges() const;
    /** Gets the number of nodes (without having to sort anything) */
    virtual int numNodes() const { return _nodes.size(); }
    /** Gets the number of edges (without having to sort anything) */
    virtual int numEdges() const { return _numEdges; }
    /** Displays all graph contents */
    virtual void disp() const;
    /** Displays the contents of the given edge */
    virtual void disp(int edgeIndex) const;
    /** Displays the date/time range of the edges in the graph */
    virtual void dispDateTimeRange() const;
    /** Amount of time (seconds) for the window of edges to use when streaming */
    int windowDuration() const { return _windowDuration; }
    /** Adjust's the window size for amount of edges to use when streaming */
    void setWindowDuration(int duration);
    /** Start time of our current window */
    time_t windowStart() const;// { return _windowStart; }
    /** End time of our current window */
    time_t windowEnd() const;// { return _windowEnd; }
protected:    
    virtual void updateOrderedEdges() const;
    // Flag to determine if we've built our complete list of edges yet
    mutable bool _edgesReady;    
    
private:
    int _numEdges = 0;
    mutable std::vector<Node> _nodes;
    int _windowDuration;
    time_t _windowStart, _windowEnd;
    // Ordered map, by date/time
    std::map<time_t,std::vector<Edge>> _timeEdgeMap;
    mutable std::vector<time_t> _edgeTimes;
    mutable std::vector<Edge> _edges;
    mutable std::unordered_map<int,std::unordered_map<int,std::vector<int>>> _nodeEdges;
};

#endif	/* GRAPH_H */

