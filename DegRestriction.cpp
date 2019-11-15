#include "DegRestriction.h"
#include <limits.h>

using namespace std;

DegRestriction::DegRestriction(const std::string& edgeType, bool isOutDeg, bool isLessThan, int value)
{
    _edgeType = edgeType;
    _isOutDeg = isOutDeg;
    _isLessThan = isLessThan;
    _value = value;
}

