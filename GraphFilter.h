/* 
 * File:   GraphFilter.h
 * Author: D3M430
 *
 * Created on January 20, 2017, 8:18 AM
 */

#ifndef GRAPHFILTER_H
#define	GRAPHFILTER_H

#include "Graph.h"
#include "CertGraph.h"
#include "MatchCriteria.h"

/**
 * Class for filtering/copying graphs based on search criteria.  Vastly improves
 * performance in some cases.
 */
class GraphFilter
{
public:
    /**
     * Creates a copy of a given graph by filtering out edges that don't match
     * any of the query edges, given a query graph and the criteria.
     * @param g  Graph we are copying/filtering.
     * @param h  Query graph that we will be using.
     * @param criteria  Criteria for the query graph.
     * @param g2  Empty graph to have nodes/edges added to it, based on the criteria.
     */
    static void filter(const Graph &g, const Graph &h, const MatchCriteria &criteria, Graph &g2);
    /**
     * Creates a copy of a given CERT graph by filtering out edges that don't match
     * any of the query edges, given a query graph and the criteria.
     * @param g  Graph we are copying/filtering.
     * @param h  Query graph that we will be using.
     * @param criteria  Criteria for the query graph.
     * @param g2  Empty graph to have nodes/edges added to it, based on the criteria.
     */
    static void filter(const CertGraph &g, const CertGraph &h, const MatchCriteria &criteria, CertGraph &g2);
    /**
     * Creates a copy of a given graph by filtering out edges that don't
     * occur during the given time range.
     * @param g  Graph we are copying/filtering.
     * @param start  Start time for the range we are interested in.
     * @param end  End time for the range we are interested in.
     * @param g2  Empty graph to have nodes/edges added to it, based on the criteria.
     */
    static void filter(const CertGraph &g, time_t start, time_t end, CertGraph &g2);
};

#endif	/* GRAPHFILTER_H */

