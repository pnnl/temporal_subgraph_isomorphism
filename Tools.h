#include <string>
#include <unordered_map>
#include <vector>
#include "GraphMatch.h"
#include "CertGraph.h"

/**
 * General purpose tools
 */
class Tools
{
public:
    /**
     * Splits the given string by the given delimiter.
     */
    static std::vector<std::string> split(const std::string &str, char delim);
    /**
     * Displays duration in most appropriate units (e.g., seconds, minutes, hours, days)
     * @param duration  Time in seconds.
     */
    static void dispDuration(int duration);
    
    /**
     * Converts the date/time to MM/DD/YYYY
     * @param date  Date/time in seconds since Unix epoch.
     * @param useHyphen  If true, uses hyphen '-' to separate instead of slash '/'. (Default is false).
     */
    static std::string getDate(time_t date, bool useHyphen=false);

    /**
     * Counts the number of times each node of the give type occur in
     * one of the matched subgraphs.
     * @param type  The type of the nodes we are counting up.
     * @param subgraphs  The subgraphs we want to search.
     * @param g  The graph the subgraphs came from.
     * @return A mapping of node index (in g) to the number of times that node occurred. */
    static std::unordered_map<int,int> count(std::string type, const std::vector<GraphMatch> &subgraphs, const CertGraph &g);

    /**
     * Finds the ranking of the given node based on number of counts.
     * (e.g., 1 = node with the most counts). Returns 0 if it couldn't
     * find the node in the counts.
     * @param v  Index of the node we want to find the ranking of.
     * @param nodeCounts  Map of node index to count. */
    static int findRanking(int v, const std::unordered_map<int,int> &nodeCounts);
};
