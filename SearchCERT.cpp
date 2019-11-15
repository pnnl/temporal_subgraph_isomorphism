#include "SearchCERT.h"
#include "FastReader.h"
#include "FileIO.h"
#include "GraphFilter.h"
#include "GraphSearch.h"
#include "MatchCriteria_CERT.h"
#include "Tools.h"
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <ctime>

using namespace std;

SearchCERT::SearchCERT(const CertGraph &dataGraph, int delta, int limit, bool stdOut, bool unordered, bool saveAllGraphs, bool saveAllEdges, const string &outFolder) : _g2(dataGraph.windowDuration())
{
    _dataGraph = &dataGraph;
    _delta = delta;
    _limit = limit;
    _stdOut = stdOut;
    _unordered = unordered;
    _saveAllGraphs = saveAllGraphs;
    _saveAllEdges = saveAllEdges;
    _outFolder = outFolder;
}

int SearchCERT::search(const string &hFname, const string &outGraphFname, const string &nodeCountFname, bool saveQueryGraph)
{
    cout << "Loading GDF search graph: " << hFname << endl;
    CertGraph h = FileIO::loadCertGDF(hFname);
    h.disp();

    // Determine start and end date/time of the edges
    time_t start = _dataGraph->edges().front().time();
    time_t end = _dataGraph->edges().back().time();
    
    // Criteria for CERT graphs
    MatchCriteria_CERT criteria;
    
    cout << "Filtering graph based on search criteria" << endl;
    //CertGraph g2(_dataGraph->windowDuration());
    _g2 = CertGraph(_dataGraph->windowDuration()); // Clears the old graph
    GraphFilter::filter(*_dataGraph, h, criteria, _g2);
    cout << "New graph size: " << _g2.numNodes() << " nodes, " << _g2.numEdges() << " edges" << endl;

    clock_t t1 = clock();

    GraphSearch search;
    if(_unordered)
    {
        cout << "Search CERT graph for matching unordered subgraphs" << endl;   
        _subgraphs = search.findAllSubgraphs(_g2, h, criteria, _limit);
    }
    else
    {
        cout << "Search CERT graph for matching ordered subgraphs" << endl;   
        _subgraphs = search.findOrderedSubgraphs(_g2, h, criteria, _limit, _delta);
    }

    clock_t t2 = clock();
    cout << endl;
    cout << ">>>>>>> Time to perform graph search = " << (double)(t2 - t1)/CLOCKS_PER_SEC << " sec" << endl;
    if(_subgraphs.empty())
        cout << ">>>>>>> No matching subgraph was found!" << endl;
    else
	cout << ">>>>>>> Number of subgraphs found: " << _subgraphs.size() << endl;                        
    cout << endl;

    if(_stdOut)
    {
        // If no output folder specified, display subgraphs to stdout
        for(int sgi=0; sgi<_subgraphs.size(); sgi++)
        {
            cout << "Subgraph #" << sgi+1 << endl;
            cout << "   Results:" << endl;
            const vector<int> &edges = _subgraphs[sgi].edges();
            for(int edgeIndex : edges)
            {
                cout << "      "; // Add some tabs for display purposes
                _g2.disp(edgeIndex);
            }
        }
    }

    if(_saveAllGraphs)
    {
        cout << "Saving results to the following folder: " << _outFolder << endl;
        try
        {
            for(int sgi=0; sgi<_subgraphs.size(); sgi++)
            {
                CertGraph g3 = _g2.createSubGraph(_subgraphs[sgi].edges());

                stringstream ss;
                ss << _outFolder << "results" << sgi+1 << ".gdf";                            
                //cout << "Saving graph " << sgi+1 << " to " << ss.str() << endl;
                FileIO::saveCertGDF(g3, ss.str());
            }
        }
        catch(exception &e)
        {
            cerr << "Problem encountered saving results.  Make sure output folder exists and has write access." << endl;
            cerr << e.what() << endl;
        }
        catch(const char *msg)
        {
            cerr << "Problem encountered saving results.  Make sure output folder exists and has write access." << endl;
            cerr << msg << endl;
        }
        catch(...)
        {
            cerr << "Problem encountered saving results.  Make sure output folder exists and has write access." << endl;
        }
    }              

    if(nodeCountFname.empty()==false)
    {
	cout << "Searching for subgraph matchings for time slices" << endl;
	const int numSlices = 5;
	unordered_map<string,vector<int>> timeCounts = calcTemporalCounts(_g2, h, criteria, start, end, numSlices); 
        cout << "Saving node counts to " << nodeCountFname << endl;
        FileIO::saveNodeCount(_g2, _subgraphs, h, timeCounts, numSlices, start, end, nodeCountFname);
    }
    if(outGraphFname.empty()==false)
    {
        cout << "Creating single graph from all matching subgraphs." << endl;
        vector<int> edgeCounts;
	bool ignoreDir = true;
        CertGraph g3(0);
	if(_saveAllEdges)
	{
	    // tempNodeCounts not actually used, but required as a parameter
	    // for the createSubGraph method.
	    // NOTE: I don't remember why we switched to only having a node
	    // count of 1 for each node in the results (see below).
	    vector<int> tempNodeCounts; 
	    g3 = _g2.createSubGraph(_subgraphs, edgeCounts, tempNodeCounts);
	}
	else
	{
	    g3 = _g2.createAggregateSubGraph(ignoreDir, _subgraphs, edgeCounts);
	}
	// NOTE: Not sure why we're doing this instead of a realy node count.
	vector<int> nodeCounts(g3.nodes().size(), 1); // 1 count per node
        map<string,vector<int>> extraEdgeValues, extraNodeValues;
        extraEdgeValues["count"] = edgeCounts;
        extraNodeValues["count"] = nodeCounts;
        cout << "Saving single graph to the file: " << outGraphFname << endl;
        FileIO::saveCertGDF(g3, outGraphFname, extraEdgeValues, extraNodeValues);
    }
    if(saveQueryGraph)
    {
	string outQueryFname = _outFolder + FileIO::getFname(hFname);
	cout << "Saving copy of query graph with node/edge counts to " << outQueryFname << endl;
	FileIO::saveQueryGraph(h, _subgraphs, outQueryFname);
    }
    cout << endl;

    // Total number of matching subgraphs found
    return _subgraphs.size();
}

unordered_map<string,vector<int>> SearchCERT::calcTemporalCounts(const CertGraph &g, const CertGraph &h, const MatchCriteria_CERT &criteria, time_t start, time_t end, int numSlices)
{
    unordered_map<string,vector<int>> results;

    time_t sliceDur = (end - start)/numSlices;
    for(int i=0; i<numSlices; i++)
    {
	time_t t0 = start + i*sliceDur;
	time_t t1 = t0 + sliceDur;
	unordered_map<string,int> sliceCounts = calcTemporalSlice(g, h, criteria, t0, t1);
	for(auto pair : sliceCounts)
	{
	    const string &name = pair.first;
	    int count = pair.second;
	    vector<int> &counts = results[name];
	    if(counts.size() < numSlices)
		counts.resize(numSlices, 0);
	    counts[i] = count;
	}
    }
    return results;
}

unordered_map<string,int> SearchCERT::calcTemporalSlice(const CertGraph &g, const CertGraph &h, const MatchCriteria_CERT &criteria, time_t start, time_t end)
{
    cout << "Filtering graph based on time range slice" << endl;
    CertGraph g2(g.windowDuration());
    GraphFilter::filter(g, start, end, g2);
    cout << "New graph size: " << g2.numNodes() << " nodes, " << g2.numEdges() << " edges" << endl;

    GraphSearch search;
    vector<GraphMatch> subgraphs;
    if(_unordered)
    {
        cout << "Searching slice for matching unordered subgraphs" << endl;   
        subgraphs = search.findAllSubgraphs(g2, h, criteria, _limit);
    }
    else
    {
        cout << "Search slice for matching ordered subgraphs" << endl;   
        subgraphs = search.findOrderedSubgraphs(g2, h, criteria, _limit, _delta);
    }

    cout << subgraphs.size() << " matching subgraphs found" << endl;
    
    unordered_map<string,int> results;
    for(const GraphMatch &sg : subgraphs)
    {
	for(int u : sg.nodes())
	{
	    const string &name = g2.getLabel(u);
	    if(results.find(name) == results.end())
		results[name] = 0;
	    results[name]++;
	}
    }

    cout << results.size() << " matching nodes" << endl;

    return results;
}

void SearchCERT::motifTest(const vector<string> &motifFnames, const string &answersFname)
{
    // Load answers from file
    vector<unordered_set<string>> answers;
    FastReader csv(answersFname);
    while(csv.good())
    {
	char **vars = csv.next();
	int nc = csv.rowSize();
	if(nc == 0 || (nc == 1 && vars[0][0] == '\0'))
	    continue;
	unordered_set<string> nodeNames;
	for(int i=0; i<nc; i++)
	  nodeNames.insert(vars[i]);
	answers.push_back(nodeNames);
    }
    csv.close();

    // Perform motif test
    motifTest(motifFnames, answers);
}

void SearchCERT::motifTest(const vector<string> &motifFnames, const vector<unordered_set<string>> &answers)
{
    // Create output filename based on date range and delta
    stringstream ss;
    ss << "test_" << Tools::getDate(_dataGraph->windowStart(), true) << "_";
    ss << Tools::getDate(_dataGraph->windowEnd(), true) << "_delta" << _delta << ".csv";
    string outFname = ss.str();

    cout << "Saving experiment results to: " << outFname << endl;
    ofstream ofs(outFname);
    ofs << "Motif,Delta(hr),Start Date,End Date,# Subgraphs,# Users,# PCs";
    for(int uc=0; uc<answers.size(); uc++)
    {
	ofs << ",Use Case " << uc+1;
    }
    ofs << endl;

    for(int hi=0; hi<motifFnames.size(); hi++)
    {
	const string &hFname = motifFnames[hi];
	cout << "Loading GDF search graph: " << hFname << endl;
	CertGraph h = FileIO::loadCertGDF(hFname);
	h.disp();

	// Determine start and end date/time of the edges
	time_t start = _dataGraph->edges().front().time();
	time_t end = _dataGraph->edges().back().time();
    
	// Criteria for CERT graphs
	MatchCriteria_CERT criteria;
    
	cout << "Filtering graph based on search criteria" << endl;
	CertGraph g2(_dataGraph->windowDuration());
	GraphFilter::filter(*_dataGraph, h, criteria, g2);
	cout << "New graph size: " << g2.numNodes() << " nodes, " << g2.numEdges() << " edges" << endl;

	clock_t t1 = clock();
	GraphSearch search;
	vector<GraphMatch> subgraphs;
	if(_unordered)
	{
	    cout << "Search CERT graph for matching unordered subgraphs" << endl;   
	    subgraphs = search.findAllSubgraphs(g2, h, criteria, _limit);
	}
	else
	{
	    cout << "Search CERT graph for matching ordered subgraphs" << endl;   
	    subgraphs = search.findOrderedSubgraphs(g2, h, criteria, _limit, _delta);
	}
	clock_t t2 = clock();

	cout << endl;
	cout << ">>>>>>> Time to perform graph search = " << (double)(t2 - t1)/CLOCKS_PER_SEC << " sec" << endl;
	if(subgraphs.empty())
	    cout << ">>>>>>> No matching subgraph was found!" << endl;
	else
	    cout << ">>>>>>> Number of subgraphs found: " << subgraphs.size() << endl;                           cout << endl;

	// Find number of users and pcs in the subgraphs
	cout << "Counting up the number of nodes found of type " << CertGraph::USER_NODE << endl;
	unordered_map<int,int> userCounts = Tools::count(CertGraph::USER_NODE, subgraphs, *_dataGraph);
	cout << "Number found = " << userCounts.size() << endl;
	cout << "Counting up the number of nodes found of type " << CertGraph::USER_NODE << endl;
	unordered_map<int,int> pcCounts = Tools::count(CertGraph::PC_NODE, subgraphs, *_dataGraph);
	cout << "Number found = " << pcCounts.size() << endl;
	
	int hours = (int)(_delta/3600);
	ofs << FileIO::getFname(hFname) << "," << hours << "," << Tools::getDate(start) << "," << Tools::getDate(end);
	ofs << "," << subgraphs.size() << "," << userCounts.size() << "," << pcCounts.size();

	cout << "Determining ranking of answers" << endl;
	// Determine ranking of answers
	for(const unordered_set<string> &nodeLabels : answers)
	{
	    int bestRanking = INT_MAX;
	    for(const string &label : nodeLabels)
	    {
		if(!_dataGraph->hasLabeledNode(label))
		    continue;

		int v = _dataGraph->getIndex(label);
		int ranking = Tools::findRanking(v, userCounts);
		if(!ranking)
		    ranking = Tools::findRanking(v, pcCounts);
		if(ranking > 0 && ranking < bestRanking)		
		    bestRanking = ranking;		
	    }
	    if(bestRanking == INT_MAX)
		ofs << ",NA";
	    else
		ofs << "," << bestRanking;
	}
	ofs << endl;
    }

    cout << "Finishing results file" << endl;
    ofs.close();
}

vector<pair<string,string>> SearchCERT::findSubgraphEdges(const vector<string> &nodeIDs) const
{
    // Get indices of nodes
    vector<int> nodes;
    for(const string &id : nodeIDs)
    {
	//cout << "NODE: " << id << endl;
	if(_g2.hasLabeledNode(id))
	{
	    int index = _g2.getIndex(id);
	    nodes.push_back(index);
	    //cout << "   index = " << index << endl;
	}
    }

    // Find subgraphs containing nodes, and add their edges
    unordered_set<int> edges;
    for(const GraphMatch &gm : _subgraphs)
    {
	for(int u : nodes)
	{
	    if(gm.hasNode(u))
	    {
		//cout << "Subgraph contains: " << _g2.getLabel(u) << endl;
		for(int e : gm.edges())
		{
		    const Edge &edge = _g2.edges()[e];
		    //cout << "EDGE " << e << ": " << _g2.getLabel(edge.source()) << " (" << edge.source() << ")";
		    //cout << " -> " << _g2.getLabel(edge.dest()) << " (" << edge.dest() << ")" << endl;
		    if(edge.index() != e)
			throw "Edge index not matching correctly!";
		}
		edges.insert(gm.edges().begin(), gm.edges().end());
		gm.disp();
		break;
	    }
	}
    }
    //throw "STOP";
    //return edges;
    vector<pair<string,string>> edgeVec;
    for(int e : edges)
    {
	const Edge &edge = _g2.edges()[e];
	const string &source = _g2.getLabel(edge.source());
	const string &dest = _g2.getLabel(edge.dest());
	pair<string,string> nodePair(source,dest);
	edgeVec.push_back(nodePair);
    }
    return edgeVec;
}

