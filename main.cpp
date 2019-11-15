
#include <iostream>
#include "CmdArgs.h"
#include "DataGraph.h"
#include "FileIO.h"
#include "GraphFilter.h"
#include "GraphSearch.h"
#include "MatchCriteria_DataGraph.h"

using namespace std;

int main(int argc, char **argv)
{
    try
    {
	// Parse command line arguments
	CmdArgs args(argc, argv);
	if(!args.success())
	    return -1;

        // Largest graph to display on console (for testing purposes)
	const int MAX_NUM_EDGES_FOR_DISP = 50;

	cout << "Loading data graph from " << args.graphFname() << endl;
	DataGraph g = FileIO::loadGenericGDF(args.graphFname());
        cout << g.nodes().size() << " nodes, " << g.edges().size() << " edges" << endl;
	if(g.numEdges() < MAX_NUM_EDGES_FOR_DISP)
	    g.disp();
	cout << endl;

        // Keeps track of the subgraph counts for each query and each delta value
        vector<vector<int>> queryDeltaCounts;
        
        // Try each of the requested query graphs
        for(int i=0; i<args.queryFnames().size(); i++)
        {
            const string &queryFname = args.queryFnames()[i];
            
            cout << "Loading query graph from " << queryFname << endl;
            DataGraph h = FileIO::loadGenericGDF(queryFname);
            cout << h.nodes().size() << " nodes, " << h.edges().size() << " edges" << endl;
            if(h.numEdges() < MAX_NUM_EDGES_FOR_DISP)
                h.disp();
            cout << endl;

            if(h.nodeAttributesDef() != g.nodeAttributesDef())
                throw "Node attribute definitions don't match between the query graph and data graph.";
            if(h.edgeAttributesDef() != h.edgeAttributesDef())
                throw "Edge attribute definitions don't match between the query graph and data graph.";

            MatchCriteria_DataGraph criteria;
            cout << "Filtering data graph to improve query performance." << endl;
            DataGraph g2;
            g2.setNodeAttributesDef(g.nodeAttributesDef());
            g2.setEdgeAttributesDef(g.edgeAttributesDef());
            GraphFilter::filter(g, h, criteria, g2);
            cout << g2.nodes().size() << " nodes, " << g2.edges().size() << " edges" << endl;
            if(g2.numEdges() < MAX_NUM_EDGES_FOR_DISP)
                g2.disp();
            cout << endl;
            
            // Try each of the requested delta time restrictions
            vector<int> deltaCounts; // Stores number of subgraph counts for each delta value
            for(time_t delta : args.deltaValues())
            {                                
                cout << "Using delta value = " << delta << endl;

                cout << "Searching for query graph in larger data graph" << endl;
                int limit = INT_MAX; // No limit
                GraphSearch search;
                vector<GraphMatch> results = search.findOrderedSubgraphs(g2, h, criteria, limit, delta);
                cout << results.size() << " matching subgraphs were found." << endl;
                deltaCounts.push_back(results.size());

                cout << "Creating combo graph of all matching subgraphs" << endl;
                DataGraph combo = g2.createSubGraph(results);
                if(combo.numEdges() < MAX_NUM_EDGES_FOR_DISP)
                    combo.disp();
                cout << endl;
                
                string outFname = args.outFname();
                if(outFname.empty())
                    outFname = args.createOutFname(args.graphFname(), queryFname, delta);
                cout << "Saving results to " << outFname << endl;
                FileIO::saveGenericGDF(combo, outFname);
            }
            queryDeltaCounts.push_back(deltaCounts);
        }        
        cout << "Done!\n" << endl;
        
        // Display a table of the count values, if searching for more than
        // one query, or more than one delta value
        if(args.queryFnames().size() > 1 || args.deltaValues().size() > 1)
        {
            cout << "Query,Delta,Count" << endl;
            for(int qi=0; qi<args.queryFnames().size(); qi++)
            {
                string query = FileIO::getFname(args.queryFnames()[qi]);
                for(int di=0; di<args.deltaValues().size(); di++)
                {
                    time_t delta = args.deltaValues()[di];
                    cout << query << "," << delta << "," << queryDeltaCounts[qi][di] << endl;
                }
            }
        }
    }
    catch(exception &e)
    {
	cout << "An error occurred: " << e.what() << endl;
    }
    catch(const char *msg)
    {
	cout << "An error occurred: " << msg << endl;
    }
    catch(...)
    {
	cout << "An unknown exception occurred." << endl;
    }
    return 0;
}
