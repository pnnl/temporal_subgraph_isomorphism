#include "GraphFilter.h"
#include "CertGraph.h"
#include <iostream>
#include <unordered_set>
#include <string>

using namespace std;

void GraphFilter::filter(const Graph& g, const Graph& h, const MatchCriteria& criteria, Graph &g2)
{
    cout << "Filtering graph" << endl;
    
    int g_n = g.numNodes();
    int g_m = g.numEdges();
    
    int h_n = h.numNodes();
    int h_m = h.numEdges();    
        
    // Try each edge
    for(int g_i=0; g_i<g_m; g_i++)
    {
        //cout << "Testing edge " << g_i << endl;
        // See if it matches any of the search edges' criteria
        for(int h_i=0; h_i<h_m; h_i++)
        {
            //cout << "Checking against " << h_i << endl;
            // If it matches at least one, we can go ahead and add it, and
            // stop the search
            if(criteria.isEdgeMatch(g, g_i, h, h_i))
            {
                //cout << "Match found" << endl;
                g2.copyEdge(g_i, g);
                break;
            }
        }        
    }
}

void GraphFilter::filter(const CertGraph &g, const CertGraph &h, const MatchCriteria &criteria, CertGraph &g2)
{
    cout << "Filtering graph" << endl;
    
    int g_n = g.numNodes();
    int g_m = g.numEdges();
    
    int h_n = h.numNodes();
    int h_m = h.numEdges();    

    // Check if h has any deg restrictions
    bool hasDegRes = false;
    for(int h_v=0; h_v<h_n; h_v++)
    {
	if(h.hasDegRestrictions(h_v))
	{
	    hasDegRes = true;
	    break;
	}
    }
    
    // If we have degree restrictions, we have to use a different approach
    // that's not quite as efficient
    if(hasDegRes)
    {
	bool useAllEdges = false;
	unordered_set<string> edgeTypes;
	// Find edge types used in node restrictions
	for(int h_v=0; h_v<h_n; h_v++)
	{
	    if(h.hasDegRestrictions(h_v))
	    {
		const vector<DegRestriction> &restricts = h.getDegRestrictions(h_v);
		for(const DegRestriction &restrict : restricts)
		{
		    edgeTypes.insert(restrict.edgeType());
		}
	    }
	}
	// Add all other used edge types as well
	for(int h_e=0; h_e<h_m; h_e++)
	{
	    const string &type = h.getEdgeType(h_e);
	    if(type.empty())        
		useAllEdges = true;
	    else
		edgeTypes.insert(type);
	}        
	// Try each edge
	for(int g_i=0; g_i<g_m; g_i++)
	{	
	    const string &type = g.getEdgeType(g_i);
	    if(useAllEdges || edgeTypes.find(type) != edgeTypes.end())
	    {
		g2.copyEdge(g_i, g);
	    }
	}
    }
    else
    {    
	for(int g_i=0; g_i<g_m; g_i++)
	{      
	    //cout << "Testing edge " << g_i << endl;
	    // See if it matches any of the search edges' criteria
	    for(int h_i=0; h_i<h_m; h_i++)
	    {
		//cout << "Checking against " << h_i << endl;
		// If it matches at least one, we can go ahead and add it, and
		// stop the search
		if(criteria.isEdgeMatch(g, g_i, h, h_i))
		{
		    //cout << "Match found" << endl;
		    g2.copyEdge(g_i, g);
		    break;
		}
	    }       
	}
    }
}

void GraphFilter::filter(const CertGraph &g, time_t start, time_t end, CertGraph &g2)
{
    const vector<Edge> &edges = g.edges();
    // Try all edges
    for(const Edge &edge : edges)
    {
	time_t t = edge.time();
	// Only copy those in the date/time range
        if(t >= start && t <= end)
        {
            g2.copyEdge(edge.index(), g);
        }       
    }
}
