#ifndef STATS__H
#define STATS__H

#include <vector>

/**
 * Calculates and stores statistics associates with results from a search.
 */
class Stats
{
public:
    Stats(const std::vector<int> &counts);
    double mean() const { return _mean; }
    double stdDev() const { return _stdDev; }
private:
    double _mean, _stdDev;
};

#endif
