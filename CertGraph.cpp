#include "CertGraph.h"
#include "GraphMatch.h"
#include <iostream>
#include <unordered_set>

using namespace std;

// Static method
string CertGraph::getUserRole(const string &user, const Roles &roles)
{
    string userRole = user;
    userRole.push_back('_');
    userRole.append(roles.getRole(user));
    return userRole;
}

void CertGraph::addLabeledNode(int v, std::string label)
{
    //throw "CertGraph::addNode(int,string) not usable";
        
    // Call base class
    LabeledWeightedGraph::addLabeledNode(v, label);
    // Extend other variables
    int n = this->nodes().size();
    if(n > this->_nodeTypes.size())
    {        
        _nodeTypes.resize(n);
        _nodeOutEdgeTypes.resize(n);
        _nodeInEdgeTypes.resize(n);
        //_nodeWeights.resize(n,0.0);        
    }
}
 
void CertGraph::addEdge(int u, int v, time_t dateTime)
{
    //throw "CertGraph::addEdge(int,int) not usable";
        
    // Call base class
    LabeledWeightedGraph::addEdge(u, v, dateTime);    
    this->_timeEdgeTypeMap[dateTime].push_back("");
}
    
void CertGraph::addWeightedEdge(int u, int v, time_t dateTime, double weight)
{
    //throw "CertGraph::addWeightedEdge(u,v) not usable";
        
    // Call base class
    LabeledWeightedGraph::addWeightedEdge(u, v, dateTime, weight);
    this->_timeEdgeTypeMap[dateTime].push_back("");
}
    
void CertGraph::copyEdge(int edgeIndex, const Graph &g)
{
    CertGraph &cg = (CertGraph&)g;
    const Edge &edge = g.edges()[edgeIndex];
    int u = edge.source(), v = edge.dest();
    // Copy all nodes, if not there yet
    if(this->nodes().size() < g.nodes().size())
    {
        int n = g.nodes().size();
        for(int i=this->nodes().size(); i<n; i++)
        {
            this->addTypedNode(cg.getLabel(i), cg.getNodeType(i));
        }
    }
    this->addEdge(u, v, edge.time(), cg.getEdgeType(edgeIndex)); 
}

CertGraph CertGraph::createSubGraph(const vector<int> &edges)
{    
    CertGraph g(this->windowDuration());
    
    // Determine which nodes we are using, and create edges between them
    unordered_map<int,int> nodeMap;
    for(int e : edges)
    {
        const Edge &edge = this->edges()[e];
        int u = edge.source();
        int v = edge.dest();
        if(nodeMap.find(u) == nodeMap.end())
        {
            int u2 = nodeMap.size();
            nodeMap[u] = u2;            
            g.addTypedNode(this->getLabel(u),this->getNodeType(u));
        }
        if(nodeMap.find(v) == nodeMap.end())
        {
            int v2 = nodeMap.size();
            nodeMap[v] = v2;
            g.addTypedNode(this->getLabel(v),this->getNodeType(v));
        }
        g.addEdge(nodeMap[u], nodeMap[v], edge.time(), this->getEdgeType(e));
    }
    
    return g;
}

CertGraph CertGraph::createSubGraph(const vector<GraphMatch> &subGraphs,
    vector<int> &edgeCounts, vector<int> &nodeCounts)
{
    edgeCounts.clear();
    nodeCounts.clear();    

    // Keeps track of unique edges found between pairs of nodes
    // (Only used when NOT counting subgraphs)
    unordered_map<int,unordered_map<int,unordered_set<int>>> uniqueEdges;
    
    CertGraph g(this->windowDuration());
    
    // Determine which nodes we are using, and create edges between them
    unordered_map<int,int> edgeMap;
    unordered_map<int,int> nodeMap;
    //for(const vector<int> &edges : subGraphEdges)
    for(const GraphMatch &gm : subGraphs)
    {
        const vector<int> &edges = gm.edges();
        // Look at each edge
        for(int e : edges)
        {
            const Edge &edge = this->edges()[e];
            int u = edge.source();
            int v = edge.dest();
            // Make sure edge hasn't been used already
            if(edgeMap.find(e) == edgeMap.end())
            {
                edgeMap[e] = edgeCounts.size();
                edgeCounts.push_back(0);
                if(nodeMap.find(u) == nodeMap.end())
                {
                    int u2 = nodeMap.size();
                    nodeMap[u] = u2;            
                    g.addTypedNode(this->getLabel(u),this->getNodeType(u));
		    nodeCounts.push_back(0);
                }
                if(nodeMap.find(v) == nodeMap.end())
                {
                    int v2 = nodeMap.size();
                    nodeMap[v] = v2;
                    g.addTypedNode(this->getLabel(v),this->getNodeType(v));
		    nodeCounts.push_back(0);		    
                }
                g.addEdge(nodeMap[u], nodeMap[v], edge.time(), this->getEdgeType(e));		
	        nodeCounts[nodeMap[u]]++;
		nodeCounts[nodeMap[v]]++;		
            }
	    edgeCounts[edgeMap[e]]++;
        }
    }
    
    return g;
}

CertGraph CertGraph::createAggregateSubGraph(bool ignoreDir, const vector<GraphMatch> &subGraphs,
    vector<int> &edgeCounts)
{
    edgeCounts.clear();

    // Keeps track of unique edges found between pairs of nodes
    unordered_map<int,unordered_map<int,unordered_set<int>>> uniqueEdges;
    // Maps a pair of nodes to a new edge index
    unordered_map<int,unordered_map<int,int>> edgeMap;

    CertGraph g(this->windowDuration());
    
    // Determine which nodes we are using, and create edges between them
    unordered_map<int,int> nodeMap;
    //for(const vector<int> &edges : subGraphEdges)
    for(const GraphMatch &gm : subGraphs)
    {
        const vector<int> &edges = gm.edges();
        // Look at each edge
        for(int e : edges)
        {
            const Edge &edge = this->edges()[e];
            int u = edge.source();
            int v = edge.dest();
	    // Make sure we don't count edges twice if ignoring direction
	    if(ignoreDir && u > v)
	    {
		swap(u,v);
	    }
            // Add the edge if none between those nodes exist
            if(edgeMap[u].find(v) == edgeMap[u].end())
            {
                edgeMap[u][v] = edgeCounts.size();
                edgeCounts.push_back(0);
                if(nodeMap.find(u) == nodeMap.end())
                {
                    int u2 = nodeMap.size();
                    nodeMap[u] = u2;            
                    g.addTypedNode(this->getLabel(u),this->getNodeType(u));
                }
                if(nodeMap.find(v) == nodeMap.end())
                {
                    int v2 = nodeMap.size();
                    nodeMap[v] = v2;
                    g.addTypedNode(this->getLabel(v),this->getNodeType(v));
                }
                g.addEdge(nodeMap[u], nodeMap[v], edge.time(), ""); // blank edge type
            }
	    // Make sure the edge count is based on number of unique edges
	    int u2 = nodeMap[u], v2 = nodeMap[v];
	    int e2 = edgeMap[u][v];
	    uniqueEdges[u2][v2].insert(e);	    
	    edgeCounts[e2] = uniqueEdges[u2][v2].size();	    
        }
    }
    
    return g;
}

void CertGraph::addTypedNode(const std::string &name, const std::string &type) //, double weight)
{    
    //cout << "addNode(" << name << "," << type << ")" << endl;
    if(this->hasLabeledNode(name)==false)
    {
        LabeledWeightedGraph::addLabeledNode(nodes().size(), name);
    
        // Resize our variables as needed
        int n = this->nodes().size();
        //cout << "n = " << n << endl;    
        // Set values
        //cout << "Getting v (" << name << ")" << endl;
        int v = this->getIndex(name);
        if(v >= this->_nodeTypes.size())
        {        
            //cout << "resizing node types" << endl;
            _nodeTypes.resize(v+1);
            //_nodeWeights.resize(n,0.0);        
            _nodeOutEdgeTypes.resize(v+1);
            _nodeInEdgeTypes.resize(v+1);
        }
        //cout << "v = " << v << endl;
        //cout << "setting nodetypes" << endl;
        _nodeTypes[v] = type;
        //cout << "nodeTypes[v] = " << _nodeTypes[v] << endl;
        //_nodeWeights[v] = weight;
    }
}

void CertGraph::addEdge(const std::string &source, const std::string &dest, time_t dateTime, const std::string &type)
{
    if(this->hasLabeledNode(source) == false)
        this->addLabeledNode(nodes().size(), source);
    if(this->hasLabeledNode(dest) == false)
        this->addLabeledNode(nodes().size(), dest);
    
    int u = this->getIndex(source);
    int v = this->getIndex(dest);
    this->addEdge(u, v, dateTime, type);
    
    //_edgesReady = false;
}

void CertGraph::addEdge(int u, int v, time_t dateTime, const std::string &type)
{    
    LabeledWeightedGraph::addEdge(u, v, dateTime);
    _timeEdgeTypeMap[dateTime].push_back(type);
    
    // Get degrees for this edge type
    vector<int> &outDegs = _typeOutDegs[type];
    vector<int> &inDegs = _typeInDegs[type];
    
    // Update size of mapping, if needed
    int n = this->nodes().size();
    if(outDegs.size() < n)
    {
        outDegs.resize(n, 0);
        inDegs.resize(n, 0);
    }
    
    // Increment degrees
    outDegs[u]++;
    inDegs[v]++;
    
    // Update sets of adjacent edge types
    // (These allow quicker graph filtering)
    if(u >= _nodeOutEdgeTypes.size())
        _nodeOutEdgeTypes.resize(u+1);
    _nodeOutEdgeTypes[u].insert(type);
    if(v >= _nodeInEdgeTypes.size())
        _nodeInEdgeTypes.resize(v+1);
    _nodeInEdgeTypes[v].insert(type);
}

const std::string &CertGraph::getEdgeType(int edgeIndex) const
{
    if(_edgesReady == false)
    {
        this->updateOrderedEdges();
    }
    return _edgeTypes[edgeIndex];
}

int CertGraph::getOutDeg(int v, const std::string &edgeType) const
{
    const auto &pair = _typeOutDegs.find(edgeType);
    if(pair == _typeOutDegs.end())
        return 0;
    else if(pair->second.size() <= v)
        return 0;
    else
    {
        //cout << pair->second[v] << endl;
        return pair->second[v];        
    }
}

int CertGraph::getInDeg(int v, const std::string &edgeType) const
{
    const auto &pair = _typeInDegs.find(edgeType);
    if(pair == _typeInDegs.end())
        return 0;
    else if(pair->second.size() <= v)
        return 0;
    else
    {
        //cout << pair->second[v] << endl;
        return pair->second[v];        
    }
}

const unordered_set<string> &CertGraph::getOutEdgeTypes(int v) const 
{ 
    if(v >= _nodeOutEdgeTypes.size())
    {
        cout << "Missing out edge types for vertex " << v << endl;
        throw "Missing out edge types for vertex.";
        //_nodeOutEdgeTypes.resize(v+1);
    }
    return _nodeOutEdgeTypes[v]; 
}
       
const unordered_set<string> &CertGraph::getInEdgeTypes(int v) const 
{ 
    if(v >= _nodeInEdgeTypes.size())
    {
        cout << "Missing in edge types for vertex " << v << endl;
        throw "Missing in edge types for vertex.";
        //_nodeInEdgeTypes.resize(v+1);
    }
    return _nodeInEdgeTypes[v]; 
}

const vector<DegRestriction> &CertGraph::getDegRestrictions(int v) const
{
    return this->_degRestricts.find(v)->second;
}
    
bool CertGraph::hasDegRestrictions(int v) const
{
    return this->_degRestricts.find(v) != _degRestricts.end();
}

void CertGraph::addDegRestriction(int v, const DegRestriction &restrict)
{
    this->_degRestricts[v].push_back(restrict);
}

void CertGraph::addRegex(int v, const std::regex &regex)
{
    _regexMatch[v] = regex;
}

void CertGraph::disp() const
{
    cout << nodes().size() << " nodes" << endl;
    for(int i=0; i<nodes().size(); i++)
    {        
        cout << "  " << this->getLabel(i); 
        const string &type = this->getNodeType(i);
        if(type.empty() == false)
            cout << " (" << type << ")";
        if(this->hasDegRestrictions(i))
        {
            const vector<DegRestriction> &restricts = this->getDegRestrictions(i);
            for(const DegRestriction &restrict : restricts)
            {
                cout << ", "; 
                if(restrict.isOutDeg())
                    cout << "OUT:";
                else
                    cout << "IN:";
                cout << restrict.edgeType();
                if(restrict.isLessThan())
                    cout << "<";
                else
                    cout << ">";
                cout << restrict.value();
            }
        }
	if(this->needsNameMatch(i))
	  {
	    cout << " [Needs Exact Name Match]";
	  }
        cout << endl;
    }
    cout << numEdges() << " edges:" << endl;
    for(int i=0; i<numEdges(); i++)
    {
        cout << "  ";
        disp(i);
    }
}

void CertGraph::disp(int edgeIndex) const
{
    const Edge &edge = edges()[edgeIndex];
    int u = edge.source(), v = edge.dest();
    const string &source = this->getLabel(u);
    const string &dest = this->getLabel(v);
    const string &sourceType = this->getNodeType(u);
    const string &destType = this->getNodeType(v);
    const string &edgeType = this->getEdgeType(edgeIndex);
    time_t dateTime = edge.time();
    
    cout << source;
    if(sourceType.empty()==false)
        cout << " (" << sourceType << ")";
    if(edgeType.empty()==false)
        cout << " --" << edgeType << "--> ";
    else
        cout << " -> ";
    cout << dest;
    if(destType.empty()==false)
        cout << " (" << destType << ")";
    
    struct tm *timeInfo = gmtime(&dateTime);
    cout << " | " << asctime(timeInfo);// << endl;
}

void CertGraph::updateOrderedEdges() const
{
    //cout << "UPDATE ORDERED EDGES" << endl;
    
    // Make sure base class is updated first
    LabeledWeightedGraph::updateOrderedEdges();
    
    // Clear old edge weights
    _edgeTypes.clear();
    
    // Build edge list in chronological order
    for(auto &pair : _timeEdgeTypeMap)
    {
        time_t dateTime = pair.first;
        const vector<string> &types = _timeEdgeTypeMap.find(dateTime)->second;
        for(const string &type : types)
        {
            //cout << "Adding edge type: " << type << endl;
            _edgeTypes.push_back(type);
        }
    }
}
