#include "MatchCriteria_Weighted.h"
#include "LabeledWeightedGraph.h"
#include <iostream>

using namespace std;

void MatchCriteria_Weighted::addMinWeight(int h_i, double minWeight)
{
    this->_minWeights[h_i] = minWeight;
}

bool MatchCriteria_Weighted::isEdgeMatch(const Graph& g, int gEdgeIndex, const Graph& h, int hEdgeIndex) const
{
    // Test base class first
    if(MatchCriteria::isEdgeMatch(g,gEdgeIndex,h,hEdgeIndex) == false)
	return false;

    // If no min weight for this edge, just return true
    if(_minWeights.find(hEdgeIndex) == _minWeights.end())
    {
        //cout << "No min weight for this edge, so it's acceptable" << endl;
        return true;
    }
    
    // Otherwise, test if this edge has the min requirements
    //cout << "Casting to PajekGraph" << endl;
    LabeledWeightedGraph &wg = (LabeledWeightedGraph&)g;
    //cout << "Getting edge weight" << endl;
    double w = wg.getEdgeWeight(gEdgeIndex);  
    //cout << "Weight = " << w << endl;
    double minW = _minWeights.find(hEdgeIndex)->second;
    //cout << "Min weight = " << minW << endl;
    return w >= minW;
}

