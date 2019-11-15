/* 
 * File:   LabeledWeightedGraph.h
 * Author: D3M430
 *
 * Created on January 20, 2017, 8:30 AM
 */

#ifndef LABELED_WEIGHTED_GRAPH_H
#define	LABELED_WEIGHTED_GRAPH_H

#include "Graph.h"
#include "WeightRestriction.h"
#include <string>
#include <unordered_map>

/**
 * Weighted, directed, labeled graph.
 */
class LabeledWeightedGraph : public Graph
{
public:
    LabeledWeightedGraph(int windowDuration=0) : Graph(windowDuration) {}
    /** Adds a node with the appropriate label. (Nodes will have blank string otherwise.) */
    virtual void addLabeledNode(int v, std::string label);
    /** Adds edge with 0.0 weight */
    virtual void addEdge(int u, int v, time_t dateTime) override;
    /** Adds edge with the given weight */
    virtual void addWeightedEdge(int u, int v, time_t dateTime, double weight);
    /** Copies the given edge from another PajekGraph, including weight information. */
    virtual void copyEdge(int edgeIndex, const Graph &g) override;
    /** Creates a subgraph from the given set of nodes */
    //virtual Graph createSubGraph(const std::vector<int> &nodes) override;
    /** Gets the weight of the edge */
    double getEdgeWeight(int edgeIndex) const;
    /** Gets the label of the node */
    const std::string &getLabel(int nodeIndex) const;    
    /** Returns true if there is a node with the given label */
    bool hasLabeledNode(const std::string &label) const { return _nameMap.find(label) != _nameMap.end(); }
    /** Gets the index of the labeled node */
    int getIndex(const std::string &nodeLabel) const;
    /** Displays nodes/edges including metadata */
    void disp() const override;
    /** Displays edge information for the given edge */
    void disp(int edgeIndex) const override;    
    
    /** Used for search graphs. Expects certain nodes to have edge weights within
     particular ranges.
     * @param e  The index of the edge.
     * @return A list of all weight restrictions for that edge, if any exist. */
    const std::vector<WeightRestriction> &getWeightRestrictions(int e) const;
    /** Used for search graphs. Returns true if there are any weight restrictions
     for this edge. */
    bool hasWeightRestrictions(int e) const;
    /** Adds the given restriction to the given edge. */
    void addWeightRestriction(int e, const WeightRestriction &restrict);

protected:
    virtual void updateOrderedEdges() const override;
    
private:
    std::vector<std::string> _nodeLabels;
    std::unordered_map<std::string,int> _nameMap;
    std::map<time_t,std::vector<double>> _timeEdgeWeightMap;
    mutable std::vector<double> _edgeWeights;
    std::unordered_map<int,std::vector<WeightRestriction>> _weightRestricts;
};

#endif	/* LABELED_WEIGHTED_GRAPH_H */

