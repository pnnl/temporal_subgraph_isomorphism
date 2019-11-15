#ifndef SEARCH_CONFIG__H
#define SEARCH_CONFIG__H

#include <time.h>
#include <vector>
#include <string>

/**
 * Represents the variables associated with streaming data in real time.
 */
struct SearchConfig
{
public:
    SearchConfig() : useStreaming(false), duration(0), delay(0), window(0), startDate(0), endDate(0) {} 
    /** Only actually use streaming if this is set to true */
    bool useStreaming;
    /** The duration (sec) of data to pull down from the Mongo database for next streaming block of data */
    time_t duration;
    /** The time to wait interval (sec) before pulling down the next block of data */
    time_t delay;
    /** The maximum time (sec) of data to hold in our graph we our analyzing. NOT to be confused with the delta value. */
    time_t window;
    /** Delta is the max duration (sec) to allow between edges in a matching subgraph. */
    time_t delta;
    /** List of unique node ID's that we want to find the subgraphs for. */
    std::vector<std::string> find_subgraphs_nodes;
    /** The name of the query that we are finding links for, for the given nodes */
    std::string find_subgraphs_query;
    /** Fixed start date and end date to use */
    time_t startDate, endDate;
};

#endif
