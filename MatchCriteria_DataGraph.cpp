#include "MatchCriteria_DataGraph.h"
#include "DataGraph.h"
#include <iostream>

using namespace std;

bool MatchCriteria_DataGraph::isEdgeMatch(const Graph& g, int gEdgeIndex, const Graph& h, int hEdgeIndex) const
{
    // Test base class first
    if(MatchCriteria::isEdgeMatch(g,gEdgeIndex,h,hEdgeIndex) == false)
	return false;

    DataGraph &dg = (DataGraph&)g;
    DataGraph &dh = (DataGraph&)h;
    
    // Check edge attributes first
    if(this->doAttributesMatch(dg.edgeAttributes()[gEdgeIndex], dh.edgeAttributes()[hEdgeIndex]) == false)
        return false;
        
    const Edge &hEdge = dh.edges()[hEdgeIndex];
    int hSource = hEdge.source();
    int hDest = hEdge.dest();
    
    const Edge &gEdge = dg.edges()[gEdgeIndex];
    int gSource = gEdge.source();
    int gDest = gEdge.dest();
    
    // Test nodes
    if(!isNodeMatch(g, gSource, h, hSource))
        return false;
    if(!isNodeMatch(g, gDest, h, hDest))
        return false;
    
    // If it pasts all tests, then it matches for our search
    return true;
}

bool MatchCriteria_DataGraph::isNodeMatch(const Graph &g, int gNodeIndex, const Graph &h, int hNodeIndex) const
{
    // Test base class first
    if(MatchCriteria::isNodeMatch(g,gNodeIndex,h,hNodeIndex) == false)
	return false;

    DataGraph &dg = (DataGraph&)g;
    DataGraph &dh = (DataGraph&)h;
    
    // Check node attributes first
    if(this->doAttributesMatch(dg.nodeAttributes()[gNodeIndex], dh.nodeAttributes()[hNodeIndex]) == false)
        return false;

    // TODO: May want to also look at adjacent edges, like we do
    // with the CERT MatchCriteria
    
    // If it pasts all tests, then it matches for our search
    return true;
}

bool MatchCriteria_DataGraph::doAttributesMatch(const Attributes& a1, const Attributes& a2) const
{
    if(a1.floatValues().size() != a2.floatValues().size() ||
       a1.intValues().size() != a2.intValues().size() ||
       a1.stringValues().size() != a2.stringValues().size())
    {
        cerr << "First attributes set:" << endl;
        cerr << a1.floatValues().size() << " floats" << endl;
        cerr << a1.intValues().size() << " ints" << endl;
        cerr << a1.stringValues().size() << " strings" << endl;
        cerr << "Second attributes set:" << endl;
        cerr << a2.floatValues().size() << " floats" << endl;
        cerr << a2.intValues().size() << " ints" << endl;
        cerr << a2.stringValues().size() << " strings" << endl;
        throw "Can't compare attributes. Number of attributes of each type don't match!";
    }
    
    const auto &f1 = a1.floatValues();
    const auto &f2 = a2.floatValues();
    for(int i=0; i<f1.size(); i++)
    {
        float x1 = f1[i], x2 = f2[i];
        FloatRestrictions r2 = a2.floatRestrictions()[i];
        if(r2.any())
            continue;
        if(r2.hasMin() && x1 < r2.min())
            return false;
        if(r2.hasMax() && x1 > r2.max())
            return false;
        if(r2.exact() && x1 != x2)
            return false;        
    }
    
    const auto &i1 = a1.intValues();
    const auto &i2 = a2.intValues();
    for(int i=0; i<i1.size(); i++)
    {
        int x1 = i1[i], x2 = i2[i];
        IntRestrictions r2 = a2.intRestrictions()[i];
        if(r2.any())
            continue;
        if(r2.hasMin() && x1 < r2.min())
            return false;
        if(r2.hasMax() && x1 > r2.max())
            return false;
        if(r2.exact() && x1 != x2)
            return false;        
    }
    
    const auto &s1 = a1.stringValues();
    const auto &s2 = a2.stringValues();
    for(int i=0; i<s1.size(); i++)
    {
        StringRestrictions r2 = a2.stringRestrictions()[i];
        if(r2.any())
            continue;
        if(r2.exact() && s1[i] != s2[i])
            return false;
    }
    return true;
}
