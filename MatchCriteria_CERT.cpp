#include "MatchCriteria_CERT.h"
#include "CertGraph.h"
#include <iostream>

using namespace std;

bool MatchCriteria_CERT::isEdgeMatch(const Graph& g, int gEdgeIndex, const Graph& h, int hEdgeIndex) const
{
    // Test base class first
    if(MatchCriteria::isEdgeMatch(g,gEdgeIndex,h,hEdgeIndex) == false)
	return false;

    CertGraph &cg = (CertGraph&)g;
    CertGraph &ch = (CertGraph&)h;
    
    // Check edge type first
    const string &hEdgeType = ch.getEdgeType(hEdgeIndex);
    //cout << "h edge type = " << hEdgeType << endl;
    if(hEdgeType.empty() == false) // Only check if it's not empty in the search graph   
    {
        const string &gEdgeType = cg.getEdgeType(gEdgeIndex);
        //cout << "g edge type = " << gEdgeType << endl;
        if(gEdgeType.compare(hEdgeType) != 0)
            return false;
    }
        
    const Edge &hEdge = ch.edges()[hEdgeIndex];
    int hSource = hEdge.source();
    int hDest = hEdge.dest();
    
    const Edge &gEdge = cg.edges()[gEdgeIndex];
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

bool MatchCriteria_CERT::isNodeMatch(const Graph &g, int gNodeIndex, const Graph &h, int hNodeIndex) const
{
    // Test base class first
    if(MatchCriteria::isNodeMatch(g,gNodeIndex,h,hNodeIndex) == false)
	return false;

    CertGraph &cg = (CertGraph&)g;
    CertGraph &ch = (CertGraph&)h;
    
    // Check node name, if needed
    if(ch.needsNameMatch(hNodeIndex))
    {
        if(ch.getLabel(hNodeIndex) != cg.getLabel(gNodeIndex))
            return false;
    }
    
    // Check node type
    const string &hType = ch.getNodeType(hNodeIndex);
    if(hType.empty() == false) // Only check if it's not empty in the search graph
    {
        const string &gType = cg.getNodeType(gNodeIndex);
        if(gType.compare(hType) != 0)
            return false;
    }
    
    // Check degree restrictions
    if(ch.hasDegRestrictions(hNodeIndex))
    {
        const vector<DegRestriction> &restricts = ch.getDegRestrictions(hNodeIndex);
        for(const DegRestriction &restrict : restricts)
        {
            int deg = 0;            
            if(restrict.isOutDeg())
                deg = cg.getOutDeg(gNodeIndex, restrict.edgeType());         
            else
                deg = cg.getInDeg(gNodeIndex, restrict.edgeType());
            if(restrict.isLessThan())
            {
                //cout << gSource << " " << restrict.edgeType() << ": " << deg << " < " << restrict.value() << "?" << endl;
                if(deg >= restrict.value())
                    return false;
            }
            else
            {
                //cout << gSource << " " << restrict.edgeType() << ": " << deg << " > " << restrict.value() << "?" << endl;
                if(deg <= restrict.value())
                    return false;
            }
        }
    }

    // Check regex restrictions
    if(ch.needsRegexMatch(hNodeIndex))
    {
	const regex &rx = ch.getRegex(hNodeIndex);
	if(regex_search(cg.getLabel(gNodeIndex), rx) == false)
	{
	    //cout << cg.getLabel(gNodeIndex) << " doesn't match the regex" << endl;
	    return false;
	}
    }

    // Check adjacent out edge types    
    const unordered_set<string> &gOutTypes = cg.getOutEdgeTypes(gNodeIndex);
    const unordered_set<string> &hOutTypes = ch.getOutEdgeTypes(hNodeIndex);
    for(const string &type : hOutTypes)
    {
        if(!type.empty())
        {
            if(gOutTypes.find(type) == gOutTypes.end())
                return false; 
        }
    }
    // Check adjacent in edge types
    const unordered_set<string> &gInTypes = cg.getInEdgeTypes(gNodeIndex);
    const unordered_set<string> &hInTypes = ch.getInEdgeTypes(hNodeIndex);
    for(const string &type : hInTypes)
    {
        if(!type.empty())
        {
            if(gInTypes.find(type) == gInTypes.end())
                return false;
        }
    }
    
    // If it pasts all tests, then it matches for our search
    return true;
}
