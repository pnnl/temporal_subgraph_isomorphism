#ifndef WEIGHT_RESTRICTION__H
#define WEIGHT_RESTRICTION__H

/**
 * WeightRestriction.
 * Defines a restriction for edge matching, where the weight of the edge
 * must be greater and/or less than particular values.
 */
class WeightRestriction
{
public:
    WeightRestriction(bool isLessThan, double value);
    /** If false, this represents a restriction to be greater than */
    bool isLessThan() const { return _isLessThan; }
    /** Weight limit (max or min, depending on if it's less than or not). */
    double value() const { return _value; }
private:
    double _value;
    bool _isLessThan;
};

#endif
