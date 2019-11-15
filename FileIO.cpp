#include "FileIO.h"
#include "LabeledWeightedGraph.h"
#include "FastReader.h"
#include "GraphMatch.h"
#include "SearchConfig.h"
#include <dirent.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <exception>
#include <iostream>
#include <fstream>
#include <regex>
#include <sstream>
#include <unordered_map>
#include <unordered_set>

using namespace std;

const std::string FileIO::STREAM_SECTION("stream");
const std::string FileIO::IS_STREAMING("isStreaming");
const std::string FileIO::STREAM_DUR("duration");
const std::string FileIO::STREAM_DELAY("delay");
const std::string FileIO::STREAM_WIN("window");
const std::string FileIO::DELTA("delta");
const std::string FileIO::NUM_NODES("nodes");
const std::string FileIO::NUM_LINKS("links");
const std::string FileIO::START_DATE("startDate");
const std::string FileIO::END_DATE("endDate");
const std::string FileIO::QUERIES_SECTION("queries");
const std::string FileIO::SEL_SUBGRAPHS_SECTION("selected_subgraphs");
const std::string FileIO::FIND_SUBGRAPHS_SECTION("find_subgraphs");
const std::string FileIO::QUERY("query");
const std::string FileIO::NODES("nodes");
const std::string FileIO::LINKS("links");


Graph FileIO::loadEdges(const std::string& fname)
{
    Graph g(0);
    ifstream ifs(fname);
    time_t time = 0;
    while(ifs.good())
    {        
        string line;
        std::getline(ifs, line);
        if(line.size() == 0 || line[0] == '#')            
            continue;
        stringstream ss(line);       
        int u=0, v=0;
        ss >> u;
        ss >> v;
        g.addEdge(u,v,time);
        time++;
    }
    ifs.close();
    return g;
}

void FileIO::loadSNAP(const string &fname, CertGraph &g)
{
    ifstream ifs(fname);
    string edgeType = "";
    while(ifs.good())
    {
	string line;
	std::getline(ifs, line);
	if(line.size() == 0)
	    continue;
	stringstream ss(line);
	string uName, vName;
	ss >> uName;
	ss >> vName;
	time_t dateTime;
	ss >> dateTime;
	if(dateTime < 0)
	    throw "Time value in the temporal SNAP data should be a positive integer >= 0.";
	g.addEdge(uName, vName, dateTime, edgeType);
    }
    ifs.close();
}

Graph FileIO::loadLabeledEdges(const string &fname)
{
    unordered_map<string,int> nameMap;
    Graph g(0);
    ifstream ifs(fname);
    time_t time = 0;
    while(ifs.good())
    {        
        string line;
        std::getline(ifs, line);
        if(line.size() == 0 || line[0] == '#')            
            continue;
        stringstream ss(line);       
        string uName, vName;
        ss >> uName;
        ss >> vName;
        //cout << "Edge between '" << uName << "' and '" << vName << "'" << endl;
        if(nameMap.find(uName)==nameMap.end())
            nameMap[uName] = nameMap.size()-1;
        if(nameMap.find(vName)==nameMap.end())
            nameMap[vName] = nameMap.size()-1;
        int u = nameMap[uName];
        int v = nameMap[vName];        
        //cout << u << " -> " << v << endl;
        if(u != v)
        {
            g.addEdge(u,v,time);
            time++;
        }
    }
    ifs.close();
    return g;
}

LabeledWeightedGraph FileIO::loadPajek(const string& fname)
{
    LabeledWeightedGraph g(0);
    ifstream ifs(fname);    
    time_t time = 0;
    string line;
    // Read nodes
    string blank;
    ifs >> blank;
    int n;
    ifs >> n;
    //cout << "Loading " << n << " nodes" << endl;
    while(ifs.good())
    {
        std::getline(ifs, line);
        //cout << line << endl;
        if(line[0] == '*') // "*Arcs"
            break;
        stringstream ss(line);
        int v;
        ss >> v;
        v--;
        string label;
        ss >> label;
        //cout << "Adding node " << v << " " << label << endl;
        try
        {
            g.addLabeledNode(v, label);
        }
        catch(exception &e)
        {
            cout << "EXCEPTION: " << e.what() << endl;
            cout << "LINE: " << line << endl;
        }
        catch(const char *msg)
        {
            cout << "EXCEPTION: " << msg << endl;
            cout << "LINE: " << line << endl;
        }
        catch(...)
        {
            cout << "UNKNOWN EXCEPTION" << endl;
            cout << "LINE: " << line << endl;
        }
    }    
    // Read edges
    while(ifs.good())
    {
        std::getline(ifs, line);
        if(line.size() <= 1)
            break;
        //cout << line << endl;
        stringstream ss(line);
        int u, v;
        double w;
        ss >> u;
        ss >> v;
        ss >> w;
        u--;
        v--;
        //cout << "Adding edge " << u << " -> " << v << " " << w << endl;        
        try
        {
            g.addWeightedEdge(u, v, time, w);
            time++;
        }
        catch(exception &e)
        {
            cout << "EXCEPTION: " << e.what() << endl;
            cout << "LINE: " << line << endl;
        }
        catch(const char *msg)
        {
            cout << "EXCEPTION: " << msg << endl;
            cout << "LINE: " << line << endl;
        }
        catch(...)
        {
            cout << "UNKNOWN EXCEPTION" << endl;
            cout << "LINE: " << line << endl;
        }
    }
    ifs.close();
    return g;
}

void FileIO::loadCertGDF(const std::string &fname, CertGraph &g)
{
    int lineNum = 0;    
    bool nodeData = false, edgeData = false;    
    FastReader csv(fname);
    while(csv.good())
    {
        char **vars = csv.next();
	int nc = csv.rowSize();
        lineNum++;
        try
        {
            if(nc > 1)
            {
		// Skip comments and empty lines
		if(vars[0][0] == '\0' || vars[0][0] == '#')
		    continue;

                if(nodeData)
                {
                    // Look if we reached the edge data section
		    if(strncmp(vars[0], "edgedef>", 8) == 0)
                    {
                        cout << "Reading edge data" << endl;
                        nodeData = false;
                        edgeData = true;
                        continue;
                    }
                    // Add node to graph
                    //cout << "Adding node" << endl;
                    g.addTypedNode(vars[0],vars[1]); //,atof(vars[2].c_str()));
                    
                    // Get any deg restrictions, if they exist
                    if(nc == 3 && vars[2][0] != '\0')
                    {
                        FileIO::addNodeRestrictions(vars[0], vars[2], g);
                    }
                }
                else if(edgeData)
                {		    
		    if(nc != 4)
		    {
			cerr << "ERROR: Row had " << nc << " columns, instead of 4." << endl;
                        throw "Error reading GDF file. Edge data should have 4 columns.";                    
		    }
                    /*cout << "source: " << vars[0] << endl;
                    cout << "dest: " << vars[1] << endl;
                    cout << "type: " << vars[2] << endl;
                    cout << "dateTime: " << vars[3] << endl;*/
                    // Add edge to graph
                    time_t dateTime = (long)(atof(vars[3]));
                    g.addEdge(vars[0], vars[1], dateTime, vars[2]);
                }
                else
                {
                    // Look to see if we reached the node data section (probably shouldn't happen)
                    if(strncmp(vars[0], "nodedef>", 8) == 0)
                    {
                        cout << "Reading node data" << endl;
                        nodeData = true;
                        edgeData = false;
                        continue;
                    }
                    else
                        throw "Error reading GDF file. No nodedef> section found.";
                }
            }
        }
        catch(exception &e)
        {
            cerr << "Problem on line #" << lineNum << endl;
            cerr << "Problematic line: " << endl;
	    csv.dispRow();
	    cerr << e.what() << endl;
            throw e.what();            
        }
        catch(const char *msg)
        {
            cerr << "Problem on line #" << lineNum << endl;
            cerr << "Problematic line: " << endl;
	    csv.dispRow();
	    cerr << msg << endl;
            throw msg;            
        }
        catch(...)
        {
            cerr << "Problem on line #" << lineNum << endl;
            cerr << "Problematic line: " << endl;
	    csv.dispRow();
	    cerr << "Unknown exception" << endl;
            throw "Unknown exception occurred when parsing CERT data.";
        }
    }
    csv.close();
    //cout << "Done reading data" << endl;
    if(edgeData == false)
        throw "Error reading GDF file. No edgedef> section found.";
}

CertGraph FileIO::loadCertGDFs(const std::string &folder)
{
    // Get list of filenames from folder
    vector<string> fnames = FileIO::getFileNames(folder);
    
    // Graph to contain all edges from files in the folder
    CertGraph g(0);
    
    for(const string &fname : fnames)
    {
        // Check for *.gdf extension first        
        if(fname.size() > 4 && fname.substr(fname.size()-4,4).compare(".gdf")==0)
        {
            cout << "Add edges from: " << fname << endl;
            FileIO::loadCertGDF(fname, g);
            cout << "   Total number of nodes: " << g.numNodes() << endl;
            cout << "   Total number of edges: " << g.numEdges() << endl;
        }
        else
            cout << "Ignoring file: " << fname << endl;
    }
    
    return g;
}

CertGraph FileIO::loadCertGDF(const std::string &fname)
{    
    CertGraph g(0);
    FileIO::loadCertGDF(fname, g);
    return g;
}

void FileIO::addNodeRestrictions(const std::string &nodeID, const std::string &restrictions, CertGraph &g)
{
    cout << "Text containing node restrictions for " << nodeID << ": " << restrictions << endl;
    int v = g.getIndex(nodeID);
    stringstream ss;
    ss << restrictions;
    while(ss.good())
    {        
        // Get next restriction
        string var;
        if(ss.str().find('&') != string::npos)
            std::getline(ss, var, '&');
        else
            std::getline(ss, var);
        
        // Determine whether it's out or in degree restriction
        bool isOutDeg = false;
        int startIndex = 0;
        if(var.size() >= 4 && var.substr(0,4) == "OUT:")
        {
            isOutDeg = true;
            startIndex = 4;
        }
        else if(var.size() >= 3 && var.substr(0,3) == "IN:")
        {
            isOutDeg = false;
            startIndex = 3;
        }
	else if(var.size() >= 6 && var.substr(0,6) == "REGEX:")
	{
	    try
	    {
		cout << "Adding regex node restriction: \"" << var.substr(6) << "\"" << endl;
		regex rx(var.substr(6));
		g.addRegex(v, rx);
	    }
	    catch(...)
	    {
		cout << "Problem encountered creating regular expression for the following input: ";
		cout << "\"" << var.substr(6) << "\"" << endl;
		cout << "Make sure the input is a valid basic POSIX regular expression." << endl;
	    }
	    continue;
	}
        else if(var == "NameMatch")
        {  
            g.setNeedsNameMatch(v);
	    cout << "Requiring exact name match for node: " << nodeID << endl;
            continue;
        }
        else
            throw "Unknown node restriction in the GDF file.  Options are 'OUT:', 'IN:', 'REGEX:' and 'NameMatch'";
        
        // Determine if it's less than or greater than
        bool isLessThan = false;
        string inequalityStr = var.substr(startIndex);
        cout << "Adding node restriction for " << nodeID << ": " << inequalityStr << endl;
        int i = inequalityStr.find('<');
        if(i != string::npos)
        {
            isLessThan = true;
        }
        else
        {
            i = inequalityStr.find('>');
            if(i != string::npos)
            {
                isLessThan = false;
            }
            else
                throw "Restriction inequality not found in GDF file.  Must be either > or <.";
        }
        
        // Determine the edge type
        string type = inequalityStr.substr(0,i);
        
        // Determine the value
        int value = atoi(inequalityStr.substr(i+1).c_str());
        
        // Create the restriction
        DegRestriction restrict(type, isOutDeg, isLessThan, value);
        g.addDegRestriction(v, restrict);
    }
}

void FileIO::saveCertGDF(const CertGraph &g, const string &fname)
{
    map<string,vector<int>> blank;
    saveCertGDF(g, fname, blank, blank);
}

void FileIO::saveCertGDF(const CertGraph &g, const string &fname,
        const map<string,vector<int>> &extraEdgeValues,
        const map<string,vector<int>> &extraNodeValues)
{
    ofstream ofs(fname.c_str());
    
    // Write node data
    //------------------------
    ofs << "nodedef>name VARCHAR,type VARCHAR";
    for(const auto &pair : extraNodeValues)
    {
        ofs << "," << pair.first << " DOUBLE";
    }
    ofs << endl;
    int n = g.nodes().size();
    for(int v=0; v<n; v++)
    {
        ofs << g.getLabel(v) << "," << g.getNodeType(v);
        for(const auto &pair : extraNodeValues)
        {
            ofs << "," << pair.second[v];
        }
        ofs << endl;
    }
    
    // Write edge data
    //------------------------
    // Using double for the long int date value, because I'm not sure
    // if they're normal int value will be long enough.  This will guarantee
    // it can handle the size, although it will cost us some precision in the
    // process.  It's possible they support a long or even a date type
    // but I haven't seen it yet.
    ofs << "edgedef>node1 VARCHAR,node2 VARCHAR,type VARCHAR,date DOUBLE";
    for(const auto &pair : extraEdgeValues)
    {
        ofs << "," << pair.first << " DOUBLE";
    }
    ofs << endl;
    int m = g.numEdges();
    for(int e=0; e<m; e++)
    {
        const Edge &edge = g.edges()[e];
        ofs << g.getLabel(edge.source()) << "," << g.getLabel(edge.dest()) << "," << g.getEdgeType(e) << "," << edge.time();
        for(const auto &pair : extraEdgeValues)
        {
            ofs << "," << pair.second[e];
        }
        ofs << endl;
    }
    ofs.close();
}

DataGraph FileIO::loadGenericGDF(const string &fname)
{
    DataGraph g;

    int lineNum = 0;    
    bool isNodeData = false, isEdgeData = false;    
    int sourceCol = -1, destCol = -1, timeCol = -1, nameCol = -1;
    int numNodeCols = 0, numEdgeCols = 0;

    FastReader csv(fname);
    while(csv.good())
    {
        //vector<string> vars = csv.next();
        char **vars = csv.next();
	//cout << csv.line() << endl;
        lineNum++;
	if(lineNum % 100000 == 0)
	    cout << "Parsing line #" << lineNum << endl;
	//if(vars.empty() || (vars.size() == 1 && (vars[0].empty() || vars[0][0] == '#')))
        int numVars = csv.rowSize();
        if(numVars == 0 || (numVars == 1 && (vars[0][0] == '\0' || vars[0][0] == '#')))
	{
	    //cout << "SKIPPING COMMENT/EMPTY LINE" << endl;
	    continue;
	}

        try
        {
	    if(!isNodeData && !isEdgeData)
	    {
		// Look to see if we reached the node data section (probably shouldn't happen)
		//if(vars[0].size() > 8 && vars[0].substr(0,8) == "nodedef>")
                if(strncmp(vars[0],"nodedef>",8)==0)
		{
		    cout << "Reading node data" << endl;
		    isNodeData = true;
		    isEdgeData = false;

                    AttributesDef def = getAttributesDef(isNodeData, vars, numVars);
                    for(int i=0; i<def.attributeNames().size(); i++)
                    {
                        if(def.attributeNames()[i] == "name")
                            nameCol = i;
                    }
		    if(nameCol < 0)
			throw "Missing \"name\" column in \"nodedef>\"";
		    g.setNodeAttributesDef(def);
                    numNodeCols = def.attributeNames().size();
			
		    continue;
		}
		else
		    throw "Error reading GDF file. No nodedef> section found.";
	    }

	    if(isNodeData)
	    {
		// Look if we reached the edge data section
		//if(vars[0].size() > 8 && vars[0].substr(0,8) == "edgedef>")
                if(strncmp(vars[0],"edgedef>",8)==0)
		{
		    cout << "Reading edge data" << endl;
		    isNodeData = false;
		    isEdgeData = true;

		    AttributesDef def = getAttributesDef(isNodeData, vars, numVars);
                    const auto &names = def.attributeNames();
		    for(int i=0; i<names.size(); i++)
		    {
			const string &name = names[i];
			if(name == "node1")
			    sourceCol = i;
			else if(name == "node2")
			    destCol = i;
			else if(name == "date" || name == "time")
			    timeCol = i;			
		    }
		    if(sourceCol < 0)
			throw "Missing \"node1\" column in \"edgedef>\"";
		    if(destCol < 0)
			throw "Missing \"node2\" column in \"edgedef>\"";
		    if(timeCol < 0)
			throw "Missing \"time\" column in \"edgedef>\"";
		    numEdgeCols = names.size();
		    g.setEdgeAttributesDef(def);
                    
		    continue;
		}		    
		if(numVars != numNodeCols)
		{
		    cerr << "Number of node attributes defined = " << numNodeCols << endl;
		    cerr << "Number of variables found = " << numVars << endl;
		    throw "Mismatch in number of columns in node section."; 
		}

		Attributes a;
                FileIO::addAttributeValues(g.nodeAttributesDef(), vars, a);
		g.addNode(vars[nameCol], a);
	    }
                
	    if(isEdgeData)
	    {	       
		if(numVars != numEdgeCols)
		    throw "Mismatch in number of columns in edge section.";

		Attributes a;
                FileIO::addAttributeValues(g.edgeAttributesDef(), vars, a);
		g.addEdge(vars[sourceCol], vars[destCol], atoi(vars[timeCol]), a);
	    }
              
        }
        catch(exception &e)
        {
            cerr << "Problem on line #" << lineNum << endl;
            cerr << "Problematic line: ";
            csv.dispRow();
	    cerr << e.what() << endl;
            throw e.what();            
        }
        catch(const char *msg)
        {
            cerr << "Problem on line #" << lineNum << endl;
            cerr << "Problematic line: ";
            csv.dispRow();
	    cerr << msg << endl;
            throw msg;            
        }
        catch(...)
        {
            cerr << "Problem on line #" << lineNum << endl;
            cerr << "Problematic line: ";
            csv.dispRow();
	    cerr << "Unknown exception" << endl;
            throw "Unknown exception occurred when parsing GDF data.";
        }
    }
    csv.close();
    //cout << "Done reading data" << endl;
    if(isEdgeData == false)
        throw "Error reading GDF file. No edgedef> section found.";

    return g;
}

void FileIO::saveGenericGDF(const DataGraph &g, const string &fname)
{
    ofstream ofs(fname);

    // Save node header
    ofs << "nodedef>";
    ofs << "name VARCHAR";
    FileIO::saveAttributesHeader(g.nodeAttributesDef(), ofs);
    
    const AttributesDef &nodeDef = g.nodeAttributesDef();
    const AttributesDef &edgeDef = g.edgeAttributesDef();
    int numNodeAtts = nodeDef.attributeNames().size();
    int numEdgeAtts = edgeDef.attributeNames().size();
    
    // Save node data
    int n = g.nodes().size();
    for(int u=0; u<n; u++)
    {
        const Attributes &a = g.nodeAttributes()[u];
        int si=0, ii=0, fi=0; // Indices into string values, int values, float values
	for(int i=0; i<numNodeAtts; i++)
	{
            AttributeType attType = nodeDef.attributeTypes()[i];
            if(attType == AttributeType::DATA)
            {
                DataType dataType = nodeDef.dataTypes()[i];       
                if(dataType == DataType::STRING)
                {
                    ofs << a.stringValues()[si];
                    si++;
                }
                else if(dataType == DataType::INT)
                {
                    ofs << a.intValues()[ii];
                    ii++;
                }
                else if(dataType == DataType::FLOAT)
                {
                    ofs << a.floatValues()[fi];
                    fi++;
                }
                else
                    throw "Can't save node attributes. Unknown data type.";
            }
            else if(attType == AttributeType::NODE_ID)
            {
                ofs << g.getName(u);
            }
            else
            {
                throw "Inappropriate attribute type found in node section.";
            }
            if(i < numNodeAtts-1)
                ofs << ",";
	}
	ofs << endl;
    }

    // Save edge header
    ofs << "edgedef>";
    FileIO::saveAttributesHeader(g.edgeAttributesDef(), ofs);

    // Save edge data
    int m = g.numEdges();
    for(int e=0; e<m; e++)
    {
        const Edge &edge = g.edges()[e];
	const Attributes &a = g.edgeAttributes()[e];
        int si=0, ii=0, fi=0; // Indices into string values, int values, float values
	for(int i=0; i<numEdgeAtts; i++)
	{
            AttributeType attType = edgeDef.attributeTypes()[i];
            if(attType == AttributeType::DATA)
            {
                DataType dataType = edgeDef.dataTypes()[i];
                if(dataType == DataType::STRING)
                {
                    ofs << a.stringValues()[si];
                    si++;
                }
                else if(dataType == DataType::INT)
                {
                    ofs << a.intValues()[ii];
                    ii++;
                }
                else if(dataType == DataType::FLOAT)
                {
                    ofs << a.floatValues()[fi];
                    fi++;
                }
                else
                    throw "Can't save node attributes. Unknown data type.";
            }
            else if(attType == AttributeType::SOURCE_ID)
            {
                ofs << g.getName(edge.source());
            }
            else if(attType == AttributeType::DEST_ID)
            {
                ofs << g.getName(edge.dest());
            }
            else if(attType == AttributeType::TIME)
            {
                ofs << edge.time();
            }
            else
            {
                throw "Inappropriate attribute type found in edge section.";
            }
            if(i < numEdgeAtts-1)
                ofs << ",";
	}
	ofs << endl;
    }

    ofs.close();
}

void FileIO::saveNodeCount(const LabeledWeightedGraph &g, const vector<GraphMatch> &subgraphs, 
			   const LabeledWeightedGraph &h, const unordered_map<string,vector<int>> &timeCounts, 
			   int numTimeSlices, time_t startTime, time_t endTime, const string &fname)
{
    // Count up the results
    //-------------------------------
    //unordered_map<int,int> nodeCounts;     
    unordered_map<int,unordered_map<int,int>> nodeCounts;
    for(GraphMatch gm : subgraphs)
    {
        // Create set of all nodes in the subgraph (ensures we add each only once per subgraph)
        /*const vector<int> &subgraph = gm.edges();        
        unordered_set<int> nodes;
        for(int e : subgraph)
        {
            const Edge &edge = g.edges()[e];
            nodes.insert(edge.source());
            nodes.insert(edge.dest());
        }
        // Add up count for each node in the subgraph
        for(int v : nodes)
        {
            if(nodeCounts.find(v) == nodeCounts.end())
            {
                nodeCounts[v] = 0;
            }
            nodeCounts[v]++;
        }*/
        for(int g_u : gm.nodes())
        {
            int h_u = gm.getQueryNode(g_u);
            if(nodeCounts[g_u].find(h_u) == nodeCounts[g_u].end())
                nodeCounts[g_u][h_u] = 1;
            else
                nodeCounts[g_u][h_u]++;
        }
    }
    
    
    // Save results to file
    //-------------------------------
    ofstream ofs(fname);
    // Create header
    ofs << "Node Name, Query Node, Subgraph Count";
    time_t sliceDur = (endTime - startTime)/numTimeSlices;
    for(int i=0; i<numTimeSlices; i++)
    {
	time_t sliceStart = startTime + sliceDur*i;
	time_t sliceEnd = sliceStart + sliceDur;
	ofs << ",Time Count " << sliceStart << " - " << sliceEnd;
    }
    ofs << endl;
    // Save data
    for(auto pair : nodeCounts)
    {
        int g_u = pair.first;
        //ofs << g.getLabel(pair.first) << "," << pair.second << endl;
        for(auto countPair : pair.second)
        {
            int h_u = countPair.first;
            int count = countPair.second;
	    const string &label = g.getLabel(g_u);
            ofs << label << "," << h.getLabel(h_u) << "," << count;
	    // If no count is listed, just give everything 0
	    if(timeCounts.find(label) == timeCounts.end())
	    {
		for(int i=0; i<numTimeSlices; i++)
		    ofs << ",0";
	    }
	    // Otherwise, output the counted results
	    else
	    {
		const vector<int> &uTimeCounts = timeCounts.find(label)->second;
		for(int count : uTimeCounts)
		{
		    ofs << "," << count;
		}
	    }
	    ofs << endl;
        }
    }
    ofs.close();
}

void FileIO::saveQueryGraph(const CertGraph &query, const vector<GraphMatch> &subgraphs, const string &fname)
{
    int h_n = query.numNodes(), h_m = query.numEdges();

    // Keeps track of what subgraph nodes/edges have mapped to query nodes/edges
    // (Using the index of nodes/edges)
    vector<unordered_set<int>> nodeMap(h_n), edgeMap(h_m);

    for(const GraphMatch &subgraph : subgraphs)
    {
	for(int g_u : subgraph.nodes())
	{
	    int h_u = subgraph.getQueryNode(g_u);
	    nodeMap[h_u].insert(g_u);
	}
	for(int g_e : subgraph.edges())
	{
	    int h_e = subgraph.getQueryEdge(g_e);
	    edgeMap[h_e].insert(g_e);
	}
    }

    // These maps are used to store the counts as additional information
    map<string,vector<int>> nodeData, edgeData;
    vector<int> nodeCounts(h_n), edgeCounts(h_m);
    for(int h_v=0; h_v<h_n; h_v++)
    {
	nodeCounts[h_v] = nodeMap[h_v].size(); // Number of unique nodes
    }
    for(int h_e=0; h_e<h_m; h_e++)
    {
	edgeCounts[h_e] = edgeMap[h_e].size(); // Number of unique edges
    }
    string COUNT_LABEL = "count";
    nodeData[COUNT_LABEL] = nodeCounts;
    edgeData[COUNT_LABEL] = edgeCounts;				    

    // Save graph
    FileIO::saveCertGDF(query, fname, edgeData, nodeData);
}

vector<string> FileIO::getFileNames(const string &folder)
{
    string folderName = folder;
    // Convert all '\' to '/'
    for(char &c : folderName)
    {
        if(c == '\\')
            c = '/';
    }
    if(folderName.back() != '/')
        folderName.push_back('/');
    
    std::vector<string> out;    
    struct dirent *ent;
    DIR *dir = opendir(folderName.c_str());
    if(dir != 0)
    {
	while(true) 
	{
	    ent = readdir(dir);
	    if(ent == 0)
		break;
	    const string fname = ent->d_name;
	    if (fname[0] == '.')
		continue;
	    const string fullFname = folderName + fname;
	    out.push_back(fullFname);
	}    
	closedir(dir);    
    }
    return out;
}

vector<string> FileIO::getFileNames(const string &folder, const string &ext)
{
    vector<string> fnames = FileIO::getFileNames(folder);
    vector<string> outFnames;
    for(const string &fname : fnames)
    {
	int n = fname.size();
	int n2 = ext.size();
	if(n > n2)
	{
	    if(fname.substr(n-n2) == ext)
		outFnames.push_back(fname);       
	}
    }
    return outFnames;
}

time_t FileIO::getFileDate(const string &fname)
{
    struct stat fileStat;
    lstat(fname.c_str(), &fileStat);
    return fileStat.st_mtime;
}

string FileIO::getFname(const std::string &path)
{
    size_t pos = path.rfind('/');
    if(pos == string::npos)
	pos = path.rfind('\\');
    if(pos != string::npos)
	return path.substr(pos+1);
    return path;
}

bool FileIO::fileExists(const std::string &fname)
{
    ifstream ifs(fname);
    return ifs.good();
}

string FileIO::makeOutGraphFname(const std::string &fname, const std::string &outFolder)
{
    string outGraphFname = outFolder;
    string fname2 = getFname(fname);
    outGraphFname.append(fname2.substr(0,fname2.size()-4));
    outGraphFname.append(".outgraph.gdf");
    return outGraphFname;
}
 
string FileIO::makeNodeCountFname(const std::string &fname, const std::string &outFolder)
{
    string nodeCountFname = outFolder;
    string fname2 = getFname(fname);
    nodeCountFname.append(fname2.substr(0,fname2.size()-4));
    nodeCountFname.append(".nodecount.csv");
    return nodeCountFname;
}

vector<string> FileIO::getAttributeNames(char **vars, int numVars)
{
    vector<string> names;
    if(numVars <= 0)
	return names;

    names.resize(numVars);
    string front = vars[0]; // Later, might want to improve performance by leaving things in C strings
    size_t pos1 = front.find('>');
    if(pos1 == string::npos)
	throw "Couldn't find '>' on attribute definition line.";
    size_t pos2 = front.rfind(' ');
    if(pos2 == string::npos)
	throw "Couldn't find ' ' on attribute definition line.";
    pos1++;

    names[0] = front.substr(pos1, pos2-pos1);
    for(int i=1; i<numVars; i++)
    {
	string var = vars[i]; // Later, might want to improve performance by leaving things in C strings
	int pos = var.find(' ');
	if(pos == string::npos)
	    throw "Couldn't find ' ' on attribute line.";
	names[i] = var.substr(0, pos);
    }
    return names;
}

AttributesDef FileIO::getAttributesDef(bool isNodeData, char **vars, int numVars)
{
    AttributesDef a;
    for(int i=0; i<numVars; i++)
    {
	string var = vars[i]; // Later, we can improve the performance by leaving it a C string
        size_t pos1 = 0;
        if(i ==0)
        {
            pos1 = var.find('>');
            if(pos1 == string::npos)
                pos1 = 0;
            else
	    {
	        do
		{
                    pos1++;
		} while(var[pos1] == ' ' || var[pos1] == '\t');
	    }
        }
	size_t pos2 = var.find(' ', pos1);
	if(pos2 == string::npos)
	    throw "Couldn't find ' ' on attribute line.";
	
        string name = var.substr(pos1,pos2-pos1);
        string typeStr = var.substr(pos2+1);
        DataType dataType = DataType::STRING;
        if(typeStr == "INT" or typeStr == "INTEGER")
            dataType = DataType::INT;
        else if(typeStr == "FLOAT" or typeStr == "DOUBLE")
            dataType = DataType::FLOAT;
        else if(typeStr != "VARCHAR")
        {
            cerr << "Currently unable to use graphs with this data type: \"" << typeStr << "\"" << endl;
            throw "Unknown data type in GDF file.";
        }
        AttributeType attType = AttributeType::DATA;
        if(name == "name" && isNodeData)
            attType = AttributeType::NODE_ID;
        else if(name == "node1" && !isNodeData)
            attType = AttributeType::SOURCE_ID;
        else if(name == "node2" && !isNodeData)
            attType = AttributeType::DEST_ID;
        else if(name == "time" || name == "date" && !isNodeData)
            attType = AttributeType::TIME;
        //cout << "Adding \"" << name << "\" to attributes." << endl;
        a.addAttribute(attType, dataType, name);
    }
    return a;
}

void FileIO::saveAttributesHeader(const AttributesDef& def, std::ofstream& ofs)
{
    int n = def.attributeNames().size();
    for(int i=0; i<n; i++)
    {
        const string &name = def.attributeNames()[i];        
        ofs << name << " ";
        DataType type = def.dataTypes()[i];
        if(type == DataType::STRING)
            ofs << "VARCHAR";
        else if(type == DataType::INT)
            ofs << "INT";
        else if(type == DataType::FLOAT)
            ofs << "FLOAT";
        else
            throw "Problem saving attributes header. Unknown data type in attributes definition.";
        if(i < n-1)
            ofs << ",";
    }    
    ofs << endl;
}

void FileIO::addAttributeValues(const AttributesDef &def, char **vars, Attributes &a)
{
    const auto &dataTypes = def.dataTypes();
    const auto &attTypes = def.attributeTypes();
    int numCols = attTypes.size();
    for(int i=0; i<numCols; i++)
    {
        if(attTypes[i] == AttributeType::DATA)
        {
            DataType type = dataTypes[i];
            const char *var = vars[i];
            if(type == DataType::STRING)
            {
                if(var[0] == '\0')
                {
                    StringRestrictions r;
                    r.allowAny();
                    a.addStringValue(r);
                }
                else
                    a.addStringValue(var);
            }
            else if(type == DataType::FLOAT)
            {
                if(var[0] == '\0')
                {
                    FloatRestrictions r;
                    r.allowAny();
                    a.addFloatValue(r);
                }
		else if(var[0] == '>')
		{
		    FloatRestrictions r;
		    float val = atof(var+1);
		    cout << "Setting minimum: " << val << endl;
		    r.setMin(val);
		    a.addFloatValue(r);
		}
		else if(var[0] == '<')
		{
		    FloatRestrictions r;
		    float val = atof(var+1);
		    cout << "Setting maximum: " << val << endl;
		    r.setMax(val);
		    a.addFloatValue(r);
		}
                else                            
                    a.addFloatValue(atof(var));
            }
            else if(type == DataType::INT)
            {
                if(var[0] == '\0')
                {
                    IntRestrictions r;
                    r.allowAny();
                    a.addIntValue(r);
                }
                else if(var[0] == '>')
		{
		    IntRestrictions r;
		    int val = atoi(var+1);
		    cout << "Setting minimum: " << val << endl;
		    r.setMin(val);
		    a.addIntValue(r);
		}
		else if(var[0] == '<')
		{
		    IntRestrictions r;
		    int val = atoi(var+1);
		    cout << "Setting maximum: " << val << endl;
		    r.setMax(val);
		    a.addIntValue(r);
		}
		else                            
                    a.addIntValue(atoi(var));
            }
            else
                throw "Can't add attributes. Unknown data type.";
        }
    }  
}
