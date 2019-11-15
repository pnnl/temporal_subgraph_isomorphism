/* 
 * File:   CertGraph.h
 * Author: D3M430
 *
 * Created on February 6, 2017, 11:36 AM
 */

#ifndef CERTGRAPH_H
#define	CERTGRAPH_H

#include "LabeledWeightedGraph.h"
#include "Graph.h"
#include "GraphMatch.h"
#include "DegRestriction.h" // Used for search graphs
#include "Roles.h"
#include <string>
#include <time.h>
#include <map> // For a red-black binary tree (sorting date/time)
#include <regex>
#include <unordered_map>
#include <unordered_set>
#include <vector>

/**
 * Subclass of our labeled, weighted, directed temporal graph design
 * specifically for use with the CERT synthetic cyber data set.
 * https://resources.sei.cmu.edu/library/asset-view.cfm?assetid=508099
 */
class CertGraph : public LabeledWeightedGraph
{
public:
    // Node type constants
    static constexpr const char* USER_NODE = "user";
    static constexpr const char* PC_NODE = "pc";
    static constexpr const char* FILENAME_NODE = "filename";
    static constexpr const char* CONTENT_NODE = "content";
    static constexpr const char* FILETREE_NODE = "filetree";
    static constexpr const char* ADDRESS_NODE = "address";
    static constexpr const char* DOMAIN_NODE = "domain";

    /** Returns concatenated string with user ID and role (or UNKNOWN, if not known) */
    static std::string getUserRole(const std::string &user, const Roles &roles);
   
    CertGraph(int windowDuration) : LabeledWeightedGraph(windowDuration) { }
    /** Overriding from base class. Adds a node with the appropriate label. (Nodes will have blank string otherwise.) */
    void addLabeledNode(int v, std::string label) override;
    /** Adds a node with all the CERT metadata we are using */
    void addTypedNode(const std::string &name, const std::string &type); //, double weight);
    /** Overriding from base class. Adds edge with 0.0 weight */
    void addEdge(int u, int v, time_t dateTime) override;
    /** Overriding from base class. Adds edge with the given weight */
    void addWeightedEdge(int u, int v, time_t dateTime, double weight) override;
    /** Adds an edge with all the CERT metadata we are using */
    void addEdge(const std::string &source, const std::string &dest, time_t dateTime, const std::string &type);
    /** Adds an edge with all the CERT metadata we are using */
    void addEdge(int u, int v, time_t dateTime, const std::string &type);
    /** Overriding from base class. Copies the given edge from another PajekGraph, including weight information. */
    void copyEdge(int edgeIndex, const Graph &g) override;
    /** Creates a new subgraph with just the given edges, and any adjoining nodes */
    CertGraph createSubGraph(const std::vector<int> &edges);
    /** Creates a new subgraph based on all of the given subgraphs.
     * @param subGraphs  (INPUT) List of matching subgraphs we want to combine.
     * @param edgeCounts  (OUTPUT) The number of times each edge occurs in subgraphs.
     * @param nodeCounts  (OUTPUT) The number of times each node occurs in subgraphs.
     */
    CertGraph createSubGraph(const std::vector<GraphMatch> &subGraphs,
	      std::vector<int> &edgeCounts, std::vector<int> &nodeCounts);
    /** Creates a new subgraph based on all of the given subgraphs,
     * and aggregating their edges, regardless of edge type.
     * @param ignoreDir  (INPUT) If true, edges are combined regardless of direction.
     * @param subGraphs  (INPUT) List of matching subgraphs we want to combine.
     * @param edgeCounts  (OUTPUT) The number of unique edges in the original graph that occur for this edge.
     */
    CertGraph createAggregateSubGraph(bool ignoreDir, const std::vector<GraphMatch> &subGraphs, std::vector<int> &edgeCounts);
    
    /** Gets the type of edge */
    const std::string &getNodeType(int v) const { return _nodeTypes[v]; }
    /** Gets weight associated with node */
    const std::string &getEdgeType(int edgeIndex) const;
    /** Returns the number of outgoing edges adjacent to the node for the given edge type.
     * @param v  The index of the node.
     * @param edgeType  The type of edge. */
    int getOutDeg(int v, const std::string &edgeType) const;
    /** Returns the number of incoming edges adjacent to the node for the given edge type.
     * @param v  The index of the node.
     * @param edgeType  The type of edge. */
    int getInDeg(int v, const std::string &edgeType) const;
    
    /** Gets the types used for out edges for the given node.
     * @param v  Index of the node. */
    const std::unordered_set<std::string> &getOutEdgeTypes(int v) const;
    /** Gets the types used for in edges for the given node.
     * @param v  Index of the node. */
    const std::unordered_set<std::string> &getInEdgeTypes(int v) const;
    
    /** Displays all graph contents */
    void disp() const override;
    /** Displays the contents of the given edge */
    void disp(int edgeIndex) const override;
    
    /** Used for search graphs. Expects certain nodes to have degrees within
     particular ranges for particular edge types.
     * @param v  The index of the node.
     * @return A list of all degree restrictions for that vertex, if any exist. */
    const std::vector<DegRestriction> &getDegRestrictions(int v) const;
    /** Used for search graphs. Returns true if there are any degree restrictions
     for this node. */
    bool hasDegRestrictions(int v) const;
    /** Adds the given restriction to the given node. */
    void addDegRestriction(int v, const DegRestriction &restrict);
    
    /** Returns if the given node needs to have an exact name match for
     the matching subgraph.
     * @param v  The index of the node.
     * @return  True if the node needs to have a matching name in the subgraph. */
    bool needsNameMatch(int v) const { return _needsNameMatch.find(v) != _needsNameMatch.end(); }
    /** Requires that this node have its named match in a matching subgraph.
     * @param v  The index of the node. */
    void setNeedsNameMatch(int v) { _needsNameMatch.insert(v); }

    /** Returns if the given node needs to have a regular expression match
    for the matching subgraph. 
    * @param v  The index of the node.
    * @return  True if the node needs to have a regular expression match in the subgraph. */
    bool needsRegexMatch(int v) const { return _regexMatch.find(v) != _regexMatch.end(); }
    /** Adds the given regular expression as a requirement for matching
     * the given node in the subgraph.
     * @param v  The index of the node.
     * @param regex  Regular expression that needs to be matched in the subgraph. */
    void addRegex(int v, const std::regex &regex);
    /** Returns the regular expression needed for the given node (if it exists)
     * or throws an exception, if none is needed.
     * @param v  The index of the node.
     * @param return  The compiled regular expression. */
    const std::regex &getRegex(int v) const { return _regexMatch.find(v)->second; }
protected:
    void updateOrderedEdges() const override;
    
private:
    std::vector<std::string> _nodeTypes;
    std::unordered_map<std::string,std::vector<int>> _typeOutDegs, _typeInDegs;
    std::unordered_map<int,std::vector<DegRestriction>> _degRestricts;
    std::unordered_set<int> _needsNameMatch;
    std::unordered_map<int,std::regex> _regexMatch;
    // Ordered map, by date/time
    std::map<time_t,std::vector<std::string>> _timeEdgeTypeMap;
    // Ordered list of edges for the graph
    //mutable std::vector<Edge> _edges;
    mutable std::vector<std::string> _edgeTypes;
    std::vector<std::unordered_set<std::string>> _nodeOutEdgeTypes, _nodeInEdgeTypes;
};

#endif	/* CERTGRAPH_H */

