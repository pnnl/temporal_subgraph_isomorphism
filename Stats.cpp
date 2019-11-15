#include "Stats.h"
#include <math.h>

using namespace std;

Stats::Stats(const std::vector<int> &counts)
{
    int n = counts.size();

    // Calculate mean
    double sum = 0.0;
    for(int count : counts)
	sum += count;
    _mean = sum / n;

    // Calculate std dev
    sum = 0.0;
    for(int count : counts)
    {
	double diff = (double)count - _mean; 
	sum += diff * diff;
    }
    _stdDev = sqrt(sum / (n-1));
}
