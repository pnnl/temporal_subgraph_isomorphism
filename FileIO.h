/* 
 * File:   FileIO.h
 * Author: D3M430
 *
 * Created on January 19, 2017, 8:06 AM
 */

#ifndef FILEIO_H
#define	FILEIO_H

#include "DataGraph.h"
#include "Graph.h"
#include "LabeledWeightedGraph.h"
#include "CertGraph.h"
#include "GraphMatch.h"
#include "Roles.h"
#include "SearchConfig.h"
#include <time.h>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <utility>
#include <vector>
#include <unordered_map>

class FileIO
{
public:
    /**
     * Loads basic graph, where each line is a source and destination
     * index (with the index starting at 0).
     */
    static Graph loadEdges(const std::string &fname);
    /**
     * Loads SNAP temporal network data into a CertGraph.
     * NOTE: A regular Graph would work fine, but this will
     * let us get things working quickly with existing test
     * framework for now.
     */
    static void loadSNAP(const std::string &fname, CertGraph &g);
    /**
     * Loads basic graph, where each line is a source and destination label.
     * The labels are mapped to indexes, starting at 0.
     */
    static Graph loadLabeledEdges(const std::string &fname);
    /**
     * Loads a weighted, labeled graph in the Pajek format.
     */
    static LabeledWeightedGraph loadPajek(const std::string &fname);
    /**
     * Loads a CERT graph with metadata, from a GDF file format, into the
     * given CertGraph file.  Appends the edges to any previously existing
     * ones.
     */
    static void loadCertGDF(const std::string &fname, CertGraph &g);    
    /**
     * Loads a CERT graph with metadata, from a GDF file format.     
     */
    static CertGraph loadCertGDF(const std::string &fname);    
    /**
     * Loads all *.gdf files as a CERT graph from the given folder.
     */
    static CertGraph loadCertGDFs(const std::string &folder);  
    
    /**
     * Saves CERT graph with metadata, in the GDF file format.
     * @param g  CERT Graph to save.
     * @param fname  File name to save to.
     */
    static void saveCertGDF(const CertGraph &g, const std::string &fname);
    /**
     * Saves CERT graph with metadata, in the GDF file format.
     * @param g  CERT Graph to save.
     * @param fname  File name to save to.
     * @param extraEdgeValues  Additional edge values to save to the file
     * (where the name is the string key, and each link is accounted for).     * 
     * @param extraNodeValues  Additional node values to save to the file
     * (where the name is the string key, and each node is accounted for).
     */
    static void saveCertGDF(const CertGraph &g, const std::string &fname,
        const std::map<std::string,std::vector<int>> &extraEdgeValues,
        const std::map<std::string,std::vector<int>> &extraNodeValues);

    /**
     * Loads a generic arbitrary graph from the GDF file format.
     * @param fname  File name to load from.
     * @return A DataGraph object containing our graph.
     */
    static DataGraph loadGenericGDF(const std::string &fname);

    /**
     * Saves the given attributed graph in the standard GDF file format.
     * @param g  Graph we want to save.
     * @param fname  Name of GDF file to save it to.
     */
    static void saveGenericGDF(const DataGraph &g, const std::string &fname);

    /**
     * Saves a CSV file with the number of times each node is encountered
     * in one of the matching subgraphs (for nodes with > 0 counts).
     * @param g  The graph containing the subgraphs.
     * @param subgraphs  List of edge indices for each subgraph.
     * @param h  The query graph.
     * @param timeCounts  Maps nodes to the number of subgraphs they matched to at different time slices.
     * @param numTimeSlices  Number of time slices (should match those in timeCounts)
     * @param startTime  Start of the date range.
     * @param endTime  End of the date range.
     * @param fname  Name of the CSV file to save results to.
     */
    static void saveNodeCount(const LabeledWeightedGraph &g, const std::vector<GraphMatch> &subgraphs, 
			      const LabeledWeightedGraph &h, const std::unordered_map<std::string,std::vector<int>> &timeCounts, 
			      int numTimeSlices, time_t startTime, time_t endTime, const std::string &fname);         

    /**
     * Saves a GDF graph based on the original query graph, with weights based on the
     * number of nodes and edges matching to each node and edge.
     * @param query  The query we want to save.
     * @param subgraphs  List of matched subgraphs.
     * @param fname  Name of the GDF file to save results to.
     */
    static void saveQueryGraph(const CertGraph &query, const std::vector<GraphMatch> &subgraphs, const std::string &fname);

    /**
     * Saves a JSON file describing current state of the large graph.
     * @param g  The large graph we want to save info about.
     * @param rangeStart  Beginning time of our data we're using.
     * @param rangeEnd  End time of our data we're using.
     * @param queryCounts  Number of matching subgraphs found for each query graph.
     * @param config  Current settings for streaming data and searching.
     * @param selEdges  Edges we want to save for highlighting in the GraphInfo file.
     * @param jsonFname  Name of the JSON file to save info to.
     */
    static void saveGraphInfo(const CertGraph &g, time_t rangeStart, time_t rangeEnd, const std::unordered_map<std::string,int> &queryCounts, const SearchConfig &config, const std::vector<std::pair<std::string,std::string>> &selEdges, const std::string &jsonFname);

    /**
     * Loads the config settings from the given JSON file.
     */
    static void loadConfig(const std::string &jsonFname, SearchConfig &config);

    /**
     * Returns a list of all filenames in the given folder.
     */
    static std::vector<std::string> getFileNames(const std::string &folder);
    /**
     * Returns a list of all filenames in the given folder that match the given extension.
     */
    static std::vector<std::string> getFileNames(const std::string &folder, const std::string &ext);
    /**
     * Returns the date the file was last modified on, in seconds since the epoch.
     */
    static time_t getFileDate(const std::string &fname);
    /**
     * Returns just the filename portion of the path.
     */
    static std::string getFname(const std::string &path);
    /**
     * Returns if the file exists and can be opened.
     */
    static bool fileExists(const std::string &fname);
    /**
     * Returns a file name for the single output graph, based on the query file name and output folder.
     */
    static std::string makeOutGraphFname(const std::string &fname, const std::string &outFolder);
    /**
     * Returns a file name for the node count CSV file, based on the query file name and output folder.
     */
    static std::string makeNodeCountFname(const std::string &fname, const std::string &outFolder);
    
private:
    static void addNodeRestrictions(const std::string &nodeID, const std::string &restrictions, CertGraph &g);

    static std::vector<std::string> getAttributeNames(char **vars, int numVars);
    
    static AttributesDef getAttributesDef(bool isNodeData, char **vars, int numVars);
    
    static void saveAttributesHeader(const AttributesDef &def, std::ofstream &ofs);
    
    static void addAttributeValues(const AttributesDef &def, char **vars, Attributes &a);
    
    // Constants for loading/saving JSON files
    static const std::string STREAM_SECTION;
    static const std::string IS_STREAMING;
    static const std::string STREAM_DUR;
    static const std::string STREAM_DELAY;
    static const std::string STREAM_WIN;
    static const std::string DELTA;
    static const std::string NUM_NODES;
    static const std::string NUM_LINKS;
    static const std::string START_DATE;
    static const std::string END_DATE;
    static const std::string QUERIES_SECTION;
    static const std::string SEL_SUBGRAPHS_SECTION;
    static const std::string FIND_SUBGRAPHS_SECTION;
    static const std::string QUERY, NODES, LINKS;
};

#endif	/* FILEIO_H */

