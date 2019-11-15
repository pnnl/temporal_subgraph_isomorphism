#include "LabeledWeightedGraph.h"
#include <iostream>

using namespace std;

void LabeledWeightedGraph::addWeightedEdge(int u, int v, time_t dateTime, double weight)
{
    Graph::addEdge(u, v, dateTime);
    //this->_edgeWeights.push_back(weight);  
    _timeEdgeWeightMap[dateTime].push_back(weight);
    
    // Make sure that labels are all accounted for
    if(max(u,v) >= _nodeLabels.size())
    {
        _nodeLabels.resize(max(u,v)+1);
    }
}

void LabeledWeightedGraph::addEdge(int u, int v, time_t dateTime)
{
    Graph::addEdge(u, v, dateTime);
    //this->_edgeWeights.push_back(0.0);
    _timeEdgeWeightMap[dateTime].push_back(0.0);
    
    // Make sure that labels are all accounted for
    if(max(u,v) >= _nodeLabels.size())
    {
        _nodeLabels.resize(max(u,v)+1);
    }
}

void LabeledWeightedGraph::copyEdge(int edgeIndex, const Graph& g)
{
    LabeledWeightedGraph &wg = (LabeledWeightedGraph&)g;
    const Edge &edge = g.edges()[edgeIndex];
    int u = edge.source(), v = edge.dest();
    this->addWeightedEdge(u, v, edge.time(), wg.getEdgeWeight(edgeIndex));
    // Make sure node labels are updated too
    if(this->getLabel(u).empty())
        this->addLabeledNode(u, wg.getLabel(u));
    if(this->getLabel(v).empty())
        this->addLabeledNode(v, wg.getLabel(v));
}

void LabeledWeightedGraph::addLabeledNode(int v, std::string label)
{
    if(v < 0)
        throw "Vertices must be >= 0";
    this->addNode(v);
    if(this->_nodeLabels.size() <= v)
    {
        _nodeLabels.resize(v+1);
    }
    _nodeLabels[v] = label;
    _nameMap[label] = v;    
}

double LabeledWeightedGraph::getEdgeWeight(int edgeIndex) const
{ 
    return _edgeWeights[edgeIndex]; 
}

const string &LabeledWeightedGraph::getLabel(int nodeIndex) const
{    
    return _nodeLabels[nodeIndex];
}

int LabeledWeightedGraph::getIndex(const std::string &nodeLabel) const
{
    return _nameMap.find(nodeLabel)->second;
}

void LabeledWeightedGraph::disp() const
{
    cout << numNodes() << " nodes" << endl;
    cout << numEdges() << " edges:" << endl;
    for(int i=0; i<numEdges(); i++)
    {
        cout << "  ";
        disp(i);
    }
}

void LabeledWeightedGraph::disp(int edgeIndex) const
{
    const Edge &edge = edges()[edgeIndex];
    cout << "[" << edgeIndex << "] " << edge.source() << " " << getLabel(edge.source()) << " -> " 
         << edge.dest() << " " << getLabel(edge.dest()) << " (" << _edgeWeights[edgeIndex] << ")" << endl;
}

const vector<WeightRestriction> &LabeledWeightedGraph::getWeightRestrictions(int e) const
{
    return _weightRestricts.find(e)->second;
}

bool LabeledWeightedGraph::hasWeightRestrictions(int e) const
{
    return _weightRestricts.find(e) != _weightRestricts.end();
}

void LabeledWeightedGraph::addWeightRestriction(int e, const WeightRestriction &restrict)
{
    _weightRestricts[e].push_back(restrict);
}

void LabeledWeightedGraph::updateOrderedEdges() const
{
    // Make sure base class is updated first
    Graph::updateOrderedEdges();
    
    // Clear old edge weights
    _edgeWeights.clear();
    
    // Build edge list in chronological order
    for(auto &pair : _timeEdgeWeightMap)
    {
        time_t dateTime = pair.first;
        const vector<double> &weights = _timeEdgeWeightMap.find(dateTime)->second;
        for(double w : weights)
        {
            _edgeWeights.push_back(w);
        }
    }
}


