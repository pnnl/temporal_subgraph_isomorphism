/* 
 * File:   EdgeMatchCriteria_Weighted.h
 * Author: D3M430
 *
 * Created on January 20, 2017, 8:45 AM
 */

#ifndef EDGEMATCHCRITERIA_WEIGHTED_H
#define	EDGEMATCHCRITERIA_WEIGHTED_H

#include "MatchCriteria.h"
#include "Graph.h"
#include <vector>
#include <unordered_map>

/**
 * Criteria when at least some edges need to match a minimum weight.
 */
class MatchCriteria_Weighted : public MatchCriteria
{
public:
    /**
     * The minimum weight for a particular query edge.
     * @param h_i  Index of the query edge.
     * @param minWeight  Minimum weight required.
     */
    void addMinWeight(int h_i, double minWeight);
    /**
     * Returns true if the edge weight is >= the minimum edge weight assigned
     * to the query edge you are trying to match (or if there is none assigned).
     * @param g  The graph we are searching.
     * @param gEdgeIndex  The index of the edge in our search graph we are comparing.
     * @param h  The query graph we are looking for.
     * @param hEdgeIndex  The index of the edge in the query graph we are comparing against.
     * @return  True if the graph edge matches the criteria of the query edge.
     */
    virtual bool isEdgeMatch(const Graph &g, int gEdgeIndex, const Graph &h, int hEdgeIndex) const override;
    
    /**     
     * Always returns true, because no node metadata is relevant for this type of criteria.
     * @param g  The graph we are searching.
     * @param gNodeIndex  The index of the node in our search graph we are comparing.
     * @param h  The query graph we are looking for.
     * @param hNodeIndex  The index of the node in the query graph we are comparing against.
     * @return  True if the graph edge matches the criteria of the query edge.
     */
    virtual bool isNodeMatch(const Graph &g, int gNodeIndex, const Graph &h, int hNodeIndex) const override { return true; }
private:
    std::unordered_map<int,double> _minWeights;
};

#endif	/* EDGEMATCHCRITERIA_WEIGHTED_H */

