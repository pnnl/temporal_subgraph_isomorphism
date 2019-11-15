#include <limits.h>
#include <unordered_set>
#include <unordered_map>
#include <stack>
#include <iostream>
#include "GraphSearch.h"
#include "Graph.h"
#include "Edge.h"
#include <limits.h>

using namespace std;

vector<GraphMatch> GraphSearch::findAllSubgraphs(const Graph &g, const Graph &h, int limit)
{
    // If no criteria specified, just use the "dummy" criteria, that accepts everything.
    MatchCriteria criteria;
    return this->findAllSubgraphs(g, h, criteria, limit);
}

vector<GraphMatch> GraphSearch::findAllSubgraphs(const Graph &g, const Graph &h, const MatchCriteria &criteria, int limit)
{    
    // Store class data structures
    _g = &g;
    _h = &h;
    _criteria = &criteria;
    
    bool debugOutput = false;
    
    // Stores the matching subgraphs as list of edge indices
    vector<GraphMatch> results;
    
    int n = _g->numNodes();
    int m = _g->numEdges();
        
    // Create lists of nodes that could be mapped to the given nodes
    vector<unordered_set<int>> h2gPossible = this->mapPossibleNodes();
    
    // Tables for mapping nodes and edges between the two graphs
    // -1 means no match has been assigned yet
    _h2gNodes.clear();
    _h2gNodes.resize(h.numNodes(),-1);
    _g2hNodes.clear();
    _g2hNodes.resize(n,-1);
    
    // Perform subgraph search, storing results along the way
    int numAssigned = 0;
    this->search(numAssigned, h2gPossible, results);
    
    return results;
}

vector<unordered_set<int>> GraphSearch::mapPossibleNodes()
{
    vector<unordered_set<int>> possible(_h->numNodes());
    
    int n = _g->numNodes();
    // Look at each vertex in H
    for(int h_v=0; h_v<_h->numNodes(); h_v++)
    {        
        //cout << "Testing possible nodes for " << h_v << endl;
        // Check each vertex in G for a possible match
        for(int g_v=0; g_v<n; g_v++)
        {
            //cout << "Trying " << g_v << endl;
            // If it passes the criteria, than add it to the list of possible
            if(_criteria->isNodeMatch(*_g, g_v, *_h, h_v))
            {                
                possible[h_v].insert(g_v);
            }
        }
    }
    return possible;
}

bool GraphSearch::search(int &numAssigned, vector<unordered_set<int>> &h2gPossible)
{
    // Test if nodes/edges match so far
    if(!matchesSoFar(numAssigned))
        return false;
    
    // Stop if we've reach the end
    if(numAssigned == _h->numNodes())
        return true;
    
    // Performs recursive DFS for matches
    int h_v = numAssigned;
    const auto &possible = h2gPossible[h_v];
    for(int g_v : possible)
    {
        if(_g2hNodes[g_v] < 0)
        {
            _h2gNodes[h_v] = g_v;
            _g2hNodes[g_v] = h_v;
            numAssigned++;
            if(search(numAssigned, h2gPossible))
                return true;
            _g2hNodes[g_v] = -1;
            _h2gNodes[h_v] = -1;
            numAssigned--;
        }
    }
    
    return false;
}

bool GraphSearch::search(int &numAssigned, vector<unordered_set<int>> &h2gPossible, vector<GraphMatch> &results)
{
    // Test if nodes/edges match so far
    if(!matchesSoFar(numAssigned))
        return false;
    
    // We've found a match if we've reached the end
    if(numAssigned == _h->numNodes())
    {
        //cout << "Found match!" << endl;
        // Find the matching edges for the assignment
        //vector<int> matchingEdgeIndexes;
        GraphMatch matchingEdges;
        for(const Edge &hEdge : _h->edges())
        {
            int h_u = hEdge.source();
            int h_v = hEdge.dest();
            int g_u = _h2gNodes[h_u];
            int g_v = _h2gNodes[h_v];
            const vector<int> &gEdges = _g->getEdgeIndexes(g_u, g_v);
            for(int g_e : gEdges)
            {
                if(_criteria->isEdgeMatch(*_g, g_e, *_h, hEdge.index()))
                {
                    //matchingEdgeIndexes.push_back(g_e);
                    matchingEdges.addEdge(_g->edges()[g_e], hEdge);
                }
            }
        }
        results.push_back(matchingEdges);
        return true;
    }
    
    // Performs recursive DFS for matches
    int h_v = numAssigned;
    const auto &possible = h2gPossible[h_v];
    for(int g_v : possible)
    {
        if(_g2hNodes[g_v] < 0)
        {
            _h2gNodes[h_v] = g_v;
            _g2hNodes[g_v] = h_v;
            numAssigned++;
            search(numAssigned, h2gPossible, results);                        
            _g2hNodes[g_v] = -1;
            _h2gNodes[h_v] = -1;
            numAssigned--;
        }
    }    
    
    return false;
}

bool GraphSearch::matchesSoFar(int numAssigned)
{
    // Check to see if every edge between the currently assigned vertices from
    // the subgraph is a matching edge in our graph
    for(const Edge &edge : _h->edges())
    {
        int h_u = edge.source();
        int h_v = edge.dest();
        if(h_u < numAssigned && h_v < numAssigned)
        {
            int g_u = _h2gNodes[h_u];
            int g_v = _h2gNodes[h_v];

            bool hasEdge = _g->hasEdge(g_u,g_v);
            if(hasEdge)
            {
                // Check to make sure they match the criteria
                const vector<int> edges = _g->getEdgeIndexes(g_u,g_v);
                for(int e : edges)
                {
                    if(_criteria->isEdgeMatch(*_g, e, *_h, edge.index()))
                    {
                        hasEdge = true;
                        break;
                    }
                }
            }
            if(!hasEdge)
            {
                return false;
                break;
            }
        }
    }
    return true;
}

vector<GraphMatch> GraphSearch::findOrderedSubgraphs(const Graph &g, const Graph &h, int limit, int delta)
{
    // If no criteria specified, just use the "dummy" criteria, that accepts everything.
    MatchCriteria criteria;
    return findOrderedSubgraphs(g,h,criteria,limit,delta);
}

vector<GraphMatch> GraphSearch::findOrderedSubgraphs(const Graph &g, const Graph &h, const MatchCriteria &criteria, int limit, int delta)
{
    // Store class data structures
    _g = &g;
    _h = &h;
    _criteria = &criteria;
    _delta = delta;
    
    bool debugOutput = false;
    
    // Stores the matching subgraphs as list of edge indices
    vector<GraphMatch> results;
    
    int n = _g->numNodes();
    int m = _g->numEdges();
    
    // List of all edge indexes
    _allEdges.resize(m);
    for(int i=0; i<m; i++)
        _allEdges[i] = i;
    
    // Tables for mapping nodes and edges between the two graphs
    // -1 means no match has been assigned yet
    _h2gNodes.clear();
    _h2gNodes.resize(h.numNodes(),-1);
    _g2hNodes.clear();
    _g2hNodes.resize(n,-1);
    
    // Keeps track of the number of search edges mapped to a particular
    // node, so we can know if we need to reset its mapping when removing
    // edges from a search trail.
    _numSearchEdgesForNode.clear();
    _numSearchEdgesForNode.resize(n,0);
            
    // Stores all edges found that match our query.
    // Stack used to backtrack when a particular search ends up a dead-end.
    while(_sg_edgeStack.empty()==false) // Make sure it's empty to start
    {
        _sg_edgeStack.pop();
        //_h_edgeStack.pop();
    }
        
    // The edge from H we are trying to match in G
    int h_i = 0;
    // The current edge from G we are testing out (yes, should start at -1)
    int g_i = 0;
         
    // Loop until we can account for all subgraphs matching our edges
    while(true)
    {           
	// Time of the last matched edge, for delta comparison
	//time_t lastEdgeTime = 0;
	//if(_sg_edgeStack.empty()==false)
	    //lastEdgeTime = g.edges()[_sg_edgeStack.top()].time();

	// Time of current edge, for delta comparison
	time_t curEdgeTime = 0;
	if(g_i < m)
	    curEdgeTime = g.edges()[g_i].time();

        // If we've run out of edges, we need to pop the last edge used,
        // and start the search back at the edge after that one
        while(g_i >= m || (_sg_edgeStack.empty()==false && curEdgeTime - _firstEdgeTime > delta))
        {             
            // If the edge stack is empty, then we have no options left
            // and need to give up.
            if(_sg_edgeStack.empty())            
                return results;          
                    
            // Pop the stack
            int last_g_i = _sg_edgeStack.top();            
            _sg_edgeStack.pop(); 

            // Get edge object
            const Edge &g_edge = _g->edges()[last_g_i];            
            if(debugOutput)
                cout << "Giving up on edge " << last_g_i << ": " << g_edge.source() << ", " << g_edge.dest() << endl;

            // Decrement the number of edges for the nodes in g_i
            _numSearchEdgesForNode[g_edge.source()]--;
            _numSearchEdgesForNode[g_edge.dest()]--;

            // If any of them reach zero, then we need to remove the
            // node mapping for that node, since none of our edges are
            // currently using it (making it free to be re-assigned).
            if(_numSearchEdgesForNode[g_edge.source()]==0)
            {
                int old_h_u = _g2hNodes[g_edge.source()];
                _h2gNodes[old_h_u] = -1;
                _g2hNodes[g_edge.source()] = -1;
            }
            if(_numSearchEdgesForNode[g_edge.dest()]==0)
            {
                int old_h_v = _g2hNodes[g_edge.dest()];
                _h2gNodes[old_h_v] = -1;
                _g2hNodes[g_edge.dest()] = -1;
            }

            // Decrement h_i, so that we can find a new one
            h_i--;     
            // Make sure we start the search immediately after the failed edge
            g_i = last_g_i+1;   
        }       
        
        // Get query edge
        const Edge &h_edge = _h->edges()[h_i];
        int h_u = h_edge.source();
        int h_v = h_edge.dest();
                
        // Find matching edge, if possible
        g_i = this->findNextMatch(h_i, g_i);     
                
        if(g_i < m)
        {            
            // Test to see if whole graph is found
            if(h_i+1 == _h->numEdges())
            {
                // Convert stack to vector
                //vector<int> edges = convert(_sg_edgeStack); 
                // Add the last edge to the list
                //edges.push_back(g_i);
                // Add new subgraph to the results
                //results.push_back(edges);  
                
                // Create GraphMatch object
                GraphMatch match = convert(_sg_edgeStack, g_i);
                // Add new subgraph to the results
                results.push_back(match);
                
                // Don't increment h_i (or perform mappings), because we want 
                // to find if there are other alternative subgraphs for that edge.                
		//cout << "Found subgraph #" << results.size() << endl;                
                // Test if we've reached our limit, and stop if we have.
                if(results.size() >= limit)
                    return results;
            }
            // Otherwise, add the edge and mappings to the subgraph search
            // and continue on to find next edges.
            else
            {  
                // Get matched edge
                const Edge &g_edge = _g->edges()[g_i];
                int g_u = g_edge.source();
                int g_v = g_edge.dest();

		// Set the first edge time, if needed
		if(_sg_edgeStack.empty())
		    _firstEdgeTime = g_edge.time();
                
                // Map the nodes from each graph
                _h2gNodes[h_u] = g_u;
                _h2gNodes[h_v] = g_v;
                _g2hNodes[g_u] = h_u;
                _g2hNodes[g_v] = h_v;            

                // Increment number of search edges for each node in our G edge
                _numSearchEdgesForNode[g_u]++;
                _numSearchEdgesForNode[g_v]++; 
                
                // Add it to the stack
                _sg_edgeStack.push(g_i);
                //_h_edgeStack.push(h_i);
                                
                // Increment to next edge to find
                h_i++;  
            }
        }    
        
        // Increment the edge to test
        g_i++;
    }  
    return results;
}

int GraphSearch::findNextMatch(int h_i, int g_i)
{
    bool debugOutput = false;
        
    // Get query edge
    const Edge &h_edge = _h->edges()[h_i];
    int h_u = h_edge.source();
    int h_v = h_edge.dest();
        
    // Default is to search over all edges starting at g_i
    const vector<int> *searchEdges = &_allEdges;
        
    // Look to see if nodes are already mapped, and just use those
    // node edges, if so. (Much faster!)
    if(_h2gNodes[h_u] >= 0 && _h2gNodes[h_v] >= 0)
    {
        const vector<int> &uEdges = _g->nodes()[_h2gNodes[h_u]].outEdges();
        const vector<int> &vEdges = _g->nodes()[_h2gNodes[h_v]].inEdges();
        if(uEdges.size() < vEdges.size())
            searchEdges = &uEdges;
        else
            searchEdges = &vEdges;
    }
    else if(_h2gNodes[h_u] >= 0)
    {
        searchEdges = &_g->nodes()[_h2gNodes[h_u]].outEdges();
    }
    else if(_h2gNodes[h_v] >= 0)
    {
        searchEdges = &_g->nodes()[_h2gNodes[h_v]].inEdges();
    }
    
    // Find starting place in the list
    int start = findStart(g_i, *searchEdges);
    
    // If no starting place can be found, just return that it's not possible
    if(start == searchEdges->size())
        return _g->numEdges();
    /*if(debugOutput)
    {
        cout << "Looking for an edge >= " << g_i << endl;
        cout << "Found this one: " << 
    }*/
    
    // Perform search
    return findNextMatch(h_i, *searchEdges, start);
}

int GraphSearch::findStart(int g_i, const std::vector<int> &edgeIndexes)
{
    // If it's the original edges, just return g_i
    if(edgeIndexes.size() > g_i && edgeIndexes[g_i] == g_i)
        return g_i;
       
    // Test if any edge will work
    if(edgeIndexes.empty())
        return edgeIndexes.size();
    if(edgeIndexes.back() < g_i)
        return edgeIndexes.size();
    if(edgeIndexes.front() >= g_i)
        return 0;
    
    /*for(int i=0; i<edgeIndexes.size(); i++)
    {
        if(edgeIndexes[i] >= g_i)
            return i;
    }    
    // If nothing was found, return the size of our list
    return edgeIndexes.size();*/
        
    // Otherwise, perform binary search
    int left = 0, right = edgeIndexes.size()-1;    
    while(true)
    {        
        //cout << "left = " << left << ", right =  " << right << endl;
        if(right <= left)
        {
            //cout << "edgeIndexes[left] = " << edgeIndexes[left] << endl;
            return left;        
        }
        int i = (right + left)/2;
        int ei = edgeIndexes[i];
        //cout << "i = " << i << ", ei = " << ei << endl;
        if(ei == g_i)
            return i;
        if(ei >= g_i && i == left)
            return i;
        if(ei < g_i)        
            left = i+1;
        else
        {
            if(edgeIndexes[i-1] < g_i)
            {
                //cout << "edgeIndexes[i-1] = " << edgeIndexes[i-1] << endl;
                return i;
            }
            right = i-1;
        }
    }
}

int GraphSearch::findNextMatch(int h_i, const std::vector<int> &edgesToSearch, int startIndex)
{
    bool debugOutput = false;
    
    // Get query edge
    const Edge &h_edge = _h->edges()[h_i];
    int h_u = h_edge.source();
    int h_v = h_edge.dest();

    // Check the time against the previous matched edge, if any exist
    bool checkTime = _sg_edgeStack.empty()==false;
    
    // Loop over all the edges to search
    for(int i=startIndex; i<edgesToSearch.size(); i++)
    {
        // Get the index of our edge in G
        int g_i = edgesToSearch[i];
        
        // Get original edge
        const Edge &g_edge = _g->edges()[g_i];
        int g_u = g_edge.source();
        int g_v = g_edge.dest();     

	// If we've gone past our delta, stop the search
	if(checkTime && g_edge.time() - _firstEdgeTime > _delta)
	    return _g->numEdges();
        
        if(debugOutput)
        {
            cout << "Trying edge " << g_i << ": " << g_u << ", " << g_v << "    ";
            cout << "Need to match edge " << h_i << ": " <<  h_u << ", " << h_v << endl;
            cout << "   g[" << g_u << "]=" << _g2hNodes[g_u] << " g[" << g_v << "]=" << _g2hNodes[g_v] << endl;
            cout << "   h[" << h_u << "]=" << _h2gNodes[h_u] << " h[" << h_v << "]=" << _h2gNodes[h_v] << endl;
        }
        
	// Make sure if the edge is a self-loop or not
	if((h_u == h_v && g_u == g_v) || (h_u != h_v && g_u != g_v))
	{
	    // Test if source nodes match, or both are unassigned
	    if(_h2gNodes[h_u] == g_u || (_h2gNodes[h_u] < 0 && _g2hNodes[g_u] < 0))
	    {              
		// Test if destination nodes match, or both are unassigned
		if(_h2gNodes[h_v] == g_v || (_h2gNodes[h_v] < 0 && _g2hNodes[g_v] < 0))
		{
		    // Test if metadata criteria is a match
		    if(_criteria->isEdgeMatch(*_g,g_i,*_h,h_i))
		    {   
			if(debugOutput)
			    cout << "Edge " << g_i << ": " << g_u << ", " << g_v << " is a match" << endl;
			return g_i;
		    }
		}
	    }
	}
    }
    // If no match found, return the number of edges
    return _g->numEdges();
}

vector<int> GraphSearch::convert(stack<int> s)
{
    vector<int> v(s.size());
    for(int i=v.size()-1; i>=0; i--)
    {
        v[i] = s.top();
        s.pop();
    }
    return v;
}

GraphMatch GraphSearch::convert(const std::stack<int> &s, int g_lastEdge)
{
    GraphMatch gm;
    vector<int> gEdges = convert(s);
    gEdges.push_back(g_lastEdge);
    for(int h_i=0; h_i<gEdges.size(); h_i++)
    {
        int g_i = gEdges[h_i];
        gm.addEdge(_g->edges()[g_i], _h->edges()[h_i]);
    }
    return gm;
}

