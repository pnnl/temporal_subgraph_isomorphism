/* 
 * File:   SearchCERT.h
 * Author: D3M430
 *
 * Created on May 10, 2017, 11:36 AM
 */

#ifndef SEARCHCERT_H
#define	SEARCHCERT_H

#include "CertGraph.h"
#include "MatchCriteria_CERT.h"
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

/**
 * This class provides functionality for doing the subgraph searching on CERT data sets.
 */
class SearchCERT
{
public:
    /**
     * Constructor initializes the search functionality with options given from the command line.
     * @param dataGraph  The large graph we will be searching against.
     * @param delta  The max amount of time (in sec) allowed between matched edges.
     * @param limit  Max number of subgraphs we want to find. (Set to INT_MAX to include all).
     * @param stdOut  If true, displays graph matching information to the std out console.
     * @param unorderd  If true, does standard subgraph matching, ignoring what order the edges occurred in.
     *                  If false, it requires the edges occur in the same order as the query graph.
     * @param saveAllGraphs  If true, saves each matching subgraph as a separate GDF file (not recommended.)
     * @param saveAllEdges  If true, saves each individual edge, instead of merging them. 
     * @param outFolder  The location to save any output files.
     */
    SearchCERT(const CertGraph &dataGraph, int delta, int limit, bool stdOut, bool unordered, bool saveAllGraphs, bool saveAllEdges, const std::string &outFolder);

    /**
     * Performs a subgraph search for query graph stored in the given file name, and saves
     * the necessary results.
     * @param hFname  Name of the GDF file to load the query graph from.
     * @param outGraphFname  Name of the GDF file to save the combined results graph to.
     * @param nodeCountFname  Name of the CSV file to save the node counts (number of matching subgraphs each node was found on).
     * @param saveQueryGraph  If true, also saves a copy of the query graph with additional information about the number of nodes matched to it.
     * @return Number of matching subgraphs found.
     */
    int search(const std::string &hFname, const std::string &outGraphFname, const std::string &nodeCountFname, bool saveQueryGraph);    

    /**
     * Calculates the number matching subgraphs each node was found on for the given time slices.
     * @param g  The graph we are searching against.
     * @param h  The graph we are looking for.
     * @param criteria  Match criteria for edge/node attributes.
     * @param start  Start time for our search.
     * @param end   End time for our search.
     * @param numSlices  Number of bins to slice up the time range into.
     * @return  Maps the node name to a list of counts, for each time slice.
     */
    std::unordered_map<std::string, std::vector<int>> calcTemporalCounts(const CertGraph &g, const CertGraph &h, const MatchCriteria_CERT &criteria, time_t start, time_t end, int numSlices);

    /**
     * Calculates the number matching subgraphs each node was found for a particular time slice.
     * @param g  The graph we are searching against.
     * @param h  The graph we are looking for.
     * @param criteria  Match criteria for edge/node attributes.
     * @param start  Start time for our search.
     * @param end   End time for our search.
     * @return  Maps the node name to the number of subgraphs it was found on for this slice.
     */
    std::unordered_map<std::string, int> calcTemporalSlice(const CertGraph &g, const CertGraph &h, const MatchCriteria_CERT &criteria, time_t start, time_t end);

    /**
     * Perform motif test, with the given motif/query graphs, and the given answers.
     */
    void motifTest(const std::vector<std::string> &motifFnames, const std::string &answersFname);

    /**
     * Perform motif test, with the given motif/query graphs, and the given answers.
     */
    void motifTest(const std::vector<std::string> &motifFnames, const std::vector<std::unordered_set<std::string>> &answers);

    /** Reference to the most recently matched subgraphs */
    const std::vector<GraphMatch> &subgraphs() const { return _subgraphs; }

    /** Finds all subgraphs the given nodes are on, and returns all edges
     * associated with those subgraphs (regardless if the nodes are on the edges or not). 
     */
    std::vector<std::pair<std::string,std::string>> findSubgraphEdges(const std::vector<std::string> &nodeIDs) const;

    /** Reference to the filtered graph that was actually used to do the search */
    //const CertGraph &filteredGraph() const { return _g2; }

private:
    const CertGraph *_dataGraph;
    CertGraph _g2;
    std::vector<GraphMatch> _subgraphs;
    int _delta, _limit;
    bool _stdOut, _unordered, _saveAllGraphs, _saveAllEdges;
    std::string _outFolder;
};

#endif	/* SEARCHCERT_H */

