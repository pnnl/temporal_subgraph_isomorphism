/* 
 * File:   GraphMatch.h
 * Author: D3M430
 *
 * Created on June 2, 2017, 11:21 AM
 */

#ifndef GRAPHMATCH_H
#define	GRAPHMATCH_H

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include "Edge.h"

/**
 * Stores a list of edges and nodes in the original graph that matched a query graph.
 */
class GraphMatch
{
public:
    /**
     * Adds a matched edge in our original graph that matches to an edge in the query graph.
     * @param gEdge  The edge in the original graph that we've matched to our query edge.
     * @param hEdge  The edge in the query graph that we've found a match for in the original graph.
     */
    void addEdge(const Edge &gEdge, const Edge &hEdge);
    /**
     * Gets the indices of the edges from the original graph that are in our matching subgraph.
     */
    const std::vector<int> &edges() const { return _edges; }
    /**
     * Gets the indices of the nodes from the original graph that are in our matching subgraph.     
     */
    const std::vector<int> &nodes() const { return _nodes; }
    /**
     * Returns true if the give node in the original graph is in this subgraph.
     */
    bool hasNode(int u) const;
    /**
     * Gets the index in the query graph that's associate with the matched edge in our original graph.
     * @param gEdge  Index of graph edge we want to find the corresponding query edge for.
     */
    int getQueryEdge(int gEdge) const { return _gEdge2hEdgeMap.find(gEdge)->second; }
    /**
     * Gets the node in the query graph that match the node in the original graph
     */
    int getQueryNode(int gNode) const { return _gNode2hNodeMap.find(gNode)->second; }
    /**
     * Displays contents of GraphMatch for testing
     */
    void disp() const;
private:
    std::vector<int> _edges, _nodes;
    std::unordered_set<int> _nodeSet;
    std::unordered_map<int,int> _gEdge2hEdgeMap;
    std::unordered_map<int,int> _gNode2hNodeMap;
};

#endif	/* GRAPHMATCH_H */

