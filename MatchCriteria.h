/* 
 * File:   EdgeMatchCriteria.h
 * Author: D3M430
 *
 * Created on January 16, 2017, 10:57 AM
 */

#ifndef EDGEMATCHCRITERIA_H
#define	EDGEMATCHCRITERIA_H

#include "Graph.h"

/**
 * Base class allows users to define a criteria for whether or
 * not the edge or node of a searched graph (G) matches the edge 
 * from the query graph (H). By default, just makes sure the
 * edge is within the current window time frame.
 */
class MatchCriteria
{
public:
    /**
     * Pure virtual function returns true if there is a match between the
     * graph edge, and the criteria for the query edge.
     * @param g  The graph we are searching.
     * @param gEdgeIndex  The index of the edge in our search graph we are comparing.
     * @param h  The query graph we are looking for.
     * @param hEdgeIndex  The index of the edge in the query graph we are comparing against.
     * @return  True if the graph edge matches the criteria of the query edge.
     */
    virtual bool isEdgeMatch(const Graph &g, int gEdgeIndex, const Graph &h, int hEdgeIndex) const;
    
    /**
     * Pure virtual function returns true if there is a match between the
     * graph node, and the criteria for the query node.
     * @param g  The graph we are searching.
     * @param gNodeIndex  The index of the node in our search graph we are comparing.
     * @param h  The query graph we are looking for.
     * @param hNodeIndex  The index of the node in the query graph we are comparing against.
     * @return  True if the graph edge matches the criteria of the query edge.
     */
    virtual bool isNodeMatch(const Graph &g, int gNodeIndex, const Graph &h, int hNodeIndex) const;
};

#endif	/* EDGEMATCHCRITERIA_H */

