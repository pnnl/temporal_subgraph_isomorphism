#ifndef MATCH_CRITERIA_DATA_GRAPH_H
#define MATCH_CRITERIA_DATA_GRAPH_H

#include "Attributes.h"
#include "DataGraph.h"
#include "Graph.h"
#include "MatchCriteria.h"

/**
 * Criteria for CERT graphs, whether or not the edge or node 
 * of a searched graph (G) matches the edge from the query graph (H). 
 */
class MatchCriteria_DataGraph : public MatchCriteria
{
public:
    /**
     * Overloaded function from MatchCriteria, but for CertGraphs.
     * Returns true if there is a match between the graph edge, and the 
     * criteria for the query edge.
     * @param g  The graph we are searching.
     * @param gEdgeIndex  The index of the edge in our search graph we are comparing.
     * @param h  The query graph we are looking for.
     * @param hEdgeIndex  The index of the edge in the query graph we are comparing against.
     * @return  True if the graph edge matches the criteria of the query edge.
     */
    virtual bool isEdgeMatch(const Graph &g, int gEdgeIndex, const Graph &h, int hEdgeIndex) const override;
    
    /**
     * Overloaded function from MatchCriteria, but for CertGraphs.
     * Returns true if there is a match between the graph node, and the 
     * graph node, and the criteria for the query node.
     * @param g  The graph we are searching.
     * @param gNodeIndex  The index of the node in our search graph we are comparing.
     * @param h  The query graph we are looking for.
     * @param hNodeIndex  The index of the node in the query graph we are comparing against.
     * @return  True if the graph edge matches the criteria of the query edge.
     */
    virtual bool isNodeMatch(const Graph &g, int gNodeIndex, const Graph &h, int hNodeIndex) const override;
    
    /**
     * Returns true if the attributes match.
     */
    virtual bool doAttributesMatch(const Attributes &a1, const Attributes &a2) const;
};


#endif
