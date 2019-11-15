#include "DataGraph.h"
#include <iostream>

using namespace std;

void DataGraph::addNode(int v)
{
    string name = to_string(v);
    if(_nodeNames.size() <= v)
    {
	_nodeNames.resize(v+1);
	_nodeAttributes.resize(v+1);
    }
    _nodeNames[v] = to_string(v);
    _nodeNameMap[name] = v;
    Graph::addNode(v);
}

void DataGraph::addNode(const std::string &name, const Attributes &a)
{
    if(_nodeAttributesDef.isSizeMatch(a) == false)
    {
        cerr << "Node added has:" << endl;
        cerr << a.floatValues().size() << " float values" << endl;
        cerr << a.intValues().size() << " int values" << endl;
        cerr << a.stringValues().size() << " string values" << endl;
        cerr << "Attribute definition requires:" << endl;
        cerr << _nodeAttributesDef.numFloatValues() << " float values" << endl;
        cerr << _nodeAttributesDef.numIntValues() << " int values" << endl;
        cerr << _nodeAttributesDef.numStringValues() << " string values" << endl;
	throw "Node attributes don't match the attributes set in the definition.";
    }

    int u = _nodeNameMap.size();
    _nodeNames.push_back(name);
    _nodeNameMap[name] = u;
    Graph::addNode(u);
    _nodeAttributes.push_back(a);
}

void DataGraph::addEdge(int u, int v)
{
    time_t dateTime = numEdges();
    this->addEdge(u,v,dateTime);
}

void DataGraph::addEdge(int u, int v, time_t dateTime)
{
    Graph::addEdge(u,v,dateTime);
    Attributes a;
    _edgeAttributes.push_back(a);
    _timeEdgeAttributesMap[dateTime].push_back(a);
}

void DataGraph::addEdge(const std::string &source, const std::string &dest, time_t dateTime, const Attributes &a)
{
    if(_edgeAttributesDef.isSizeMatch(a) == false)
    {
        cerr << "Edge added has:" << endl;
        cerr << a.floatValues().size() << " float values" << endl;
        cerr << a.intValues().size() << " int values" << endl;
        cerr << a.stringValues().size() << " string values" << endl;
        cerr << "Attribute definition requires:" << endl;
        cerr << _edgeAttributesDef.numFloatValues() << " float values" << endl;
        cerr << _edgeAttributesDef.numIntValues() << " int values" << endl;
        cerr << _edgeAttributesDef.numStringValues() << " string values" << endl;
	throw "Edge attributes don't match the attributes set in the definition.";
    }

    int u = _nodeNameMap[source];
    int v = _nodeNameMap[dest];
    Graph::addEdge(u,v,dateTime);
    _edgeAttributes.push_back(a);
    _timeEdgeAttributesMap[dateTime].push_back(a);
}

void DataGraph::copyEdge(int edgeIndex, const Graph &g)
{
    const DataGraph &dg = (const DataGraph&)g;
    int n = g.nodes().size();
    // Make sure we actually have all the node values, etc, first
    if(n > _nodeNames.size())
    {
	_nodeNames = dg._nodeNames;
	_nodeNameMap = dg._nodeNameMap;
	_nodeAttributes = dg._nodeAttributes;
    }

    const Edge &edge = dg.edges()[edgeIndex];
    const Attributes &a = dg.edgeAttributes()[edgeIndex];

    /*int e = this->numEdges();
    Graph::copyEdge(edgeIndex, dg);
    _edgeAttributes.push_back(a);
    _timeEdgeAttributesMap[edge.time()].push_back(a);*/
    
    const string &node1 = _nodeNames[edge.source()];
    const string &node2 = _nodeNames[edge.dest()];
    this->addEdge(node1, node2, edge.time(), a);
}

void DataGraph::setNodeAttributesDef(const AttributesDef &def)
{
    _nodeAttributesDef = def;
}

void DataGraph::setEdgeAttributesDef(const AttributesDef &def)
{
    _edgeAttributesDef = def;
}

const AttributesDef &DataGraph::nodeAttributesDef() const { return _nodeAttributesDef; }

const AttributesDef &DataGraph::edgeAttributesDef() const { return _edgeAttributesDef; }

DataGraph DataGraph::createSubGraph(const std::vector<GraphMatch> &matches) const
{
    DataGraph g;
    g._nodeAttributesDef = this->_nodeAttributesDef;
    g._edgeAttributesDef = this->_edgeAttributesDef;
        
    // Determine which nodes we are using, and create edges between them
    unordered_map<int,int> edgeMap;
    unordered_map<int,int> nodeMap;
    //for(const vector<int> &edges : subGraphEdges)
    for(const GraphMatch &gm : matches)
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
		int e2 = edgeMap.size();
                edgeMap[e] = e2;
                if(nodeMap.find(u) == nodeMap.end())
                {
                    int u2 = nodeMap.size();
                    nodeMap[u] = u2;            
                    g.addNode(this->getName(u),this->nodeAttributes()[u]);
		    //g.setNodeAttributes(u2,this->nodeAttributes()[u]);
                }
                if(nodeMap.find(v) == nodeMap.end())
                {
                    int v2 = nodeMap.size();
                    nodeMap[v] = v2;
                    g.addNode(this->getName(v),this->nodeAttributes()[v]);
		    //g.setNodeAttributes(v2,this->nodeAttributes()[v]);
                }
		const string &name1 = g.getName(nodeMap[u]);
		const string &name2 = g.getName(nodeMap[v]);
                g.addEdge(name1, name2, edge.time(), this->edgeAttributes()[e]);		
		//g.setEdgeAttributes(e2,this->edgeAttributes()[e]);
            }
        }
    }
    
    return g;
}

const string &DataGraph::getName(int v) const
{
    return _nodeNames[v];
}

const std::vector<Attributes> &DataGraph::nodeAttributes() const
{
    return _nodeAttributes;
}

const std::vector<Attributes> &DataGraph::edgeAttributes() const
{
    return _edgeAttributes;
}

void DataGraph::dispNode(int u) const
{
    cout << u << "(" << _nodeNames[u];
    const auto &values = _nodeAttributes[u].stringValues();
    for(const string &val : values)
	cout << "," << val;
    for(int val : _nodeAttributes[u].intValues())
        cout << "," << val;
    for(double val : _nodeAttributes[u].floatValues())
        cout << "," << val;
    cout << ")" << flush;
}

void DataGraph::dispEdge(int e) const
{
    const Edge &edge = this->edges()[e];
    dispNode(edge.source());
    cout << " -> ";
    dispNode(edge.dest());
    cout << " " << edge.time();
    const auto &values = _edgeAttributes[e].stringValues();
    for(const string &val : values)
	cout << "," << val;
    for(int val : _edgeAttributes[e].intValues())
        cout << "," << val;
    for(double val : _edgeAttributes[e].floatValues())
        cout << "," << val;
    cout << flush;
}

void DataGraph::disp() const
{
    int m = this->numEdges();
    for(int e=0; e<m; e++)
    {
	dispEdge(e);
	cout << endl;
    }
    if(m == 0)
	cout << "[NO EDGES]" << endl;
}

void DataGraph::updateOrderedEdges() const
{
    cout << "Updating the order of the edges." << endl;

    // Make sure base class is updated first
    Graph::updateOrderedEdges();
    
    // Clear old edge attributes
    _edgeAttributes.clear();
    
    // Build edge list in chronological order
    for(auto &pair : _timeEdgeAttributesMap)
    {
        time_t dateTime = pair.first;
        const vector<Attributes> &attributes = _timeEdgeAttributesMap.find(dateTime)->second;
        for(const Attributes &a : attributes)
        {
            _edgeAttributes.push_back(a);
        }
    }
}


