#include "MatchCriteria.h"

bool MatchCriteria::isEdgeMatch(const Graph &g, int gEdgeIndex, const Graph &h, int hEdgeIndex) const
{
    const Edge &e = g.edges()[gEdgeIndex];
    time_t t = e.time();
    return t >= g.windowStart() && t <= g.windowEnd();
}

bool MatchCriteria::isNodeMatch(const Graph &g, int gNodeIndex, const Graph &h, int hNodeIndex) const
{
    return true;
}
