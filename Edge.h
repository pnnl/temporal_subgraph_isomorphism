/* 
 * File:   Edge.h
 * Author: D3M430
 *
 * Created on January 13, 2017, 2:36 PM
 */

#ifndef EDGE_H
#define	EDGE_H

#include <time.h>

/**
 * Defines a basic directed edge in our graph.
 */
class Edge
{
public:
    /**
     * Constructor
     * @param index  Index of the edge (also implies order of occurrence).
     * @param source  Source node index (starts at 0).
     * @param dest  Destination node index (starts at 0).
     * @param dateTime  Date/time of edge (or use a sequential integer if NA).
     */
    Edge(int index, int source, int dest, time_t dateTime) { _index = index; _source = source; _dest = dest; _time = dateTime; }
    int index() const { return _index; }
    int source() const { return _source; }
    int dest() const { return _dest; }
    time_t time() const { return _time; }
private:
    int _index, _source, _dest;
    time_t _time;
};

#endif	/* EDGE_H */

