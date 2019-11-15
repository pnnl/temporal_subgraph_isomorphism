/* 
 * File:   DegRestriction.h
 * Author: D3M430
 *
 * Created on February 28, 2017, 9:18 AM
 */

#ifndef DEGRESTRICTION_H
#define	DEGRESTRICTION_H

#include <string>

/**
 * DegRestriction.
 * Defines a restriction for node matching, where the degree of the vertex
 * must be greater and/or less than particular values for a specific edge type.
 */
class DegRestriction
{
public:
    DegRestriction(const std::string &edgeType, bool isOutDeg, bool isLessThan, int value);
    const std::string &edgeType() const { return _edgeType; }
    bool isOutDeg() const { return _isOutDeg; }
    bool isLessThan() const { return _isLessThan; }
    int value() const { return _value; }
private:
    std::string _edgeType;
    int _value;
    bool _isOutDeg, _isLessThan;
};

#endif	/* DEGRESTRICTION_H */

