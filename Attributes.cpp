#include "Attributes.h"
#include <float.h>
#include <limits.h>

using namespace std;

Restrictions::Restrictions()
{       
    _exact = true;
    _any = false;
    _hasMin = false;
    _hasMax = false;
}

void Restrictions::allowAny()
{
    _any = true;
    _exact = false;
}
void Restrictions::setMin()
{
    _hasMin = true;
    _exact = false;
}
void Restrictions::setMax()
{
    _hasMax = true;
    _exact = false;
}

bool Restrictions::any() const { return _any; }
bool Restrictions::hasMin() const { return _hasMin; }
bool Restrictions::hasMax() const { return _hasMax; }
bool Restrictions::exact() const { return _exact; }

FloatRestrictions::FloatRestrictions()
{       
    _min = -FLT_MAX; // (Because FLT_MIN is 0)
    _max = FLT_MAX;
}
void FloatRestrictions::setMin(float value)
{
    Restrictions::setMin();
    _min = value;
}
void FloatRestrictions::setMax(float value)
{
    Restrictions::setMax();
    _max = value;
}
float FloatRestrictions::min() const { return _min; }
float FloatRestrictions::max() const { return _max; }

IntRestrictions::IntRestrictions()
{       
    _min = INT_MIN;
    _max = INT_MAX;
}
void IntRestrictions::setMin(int value)
{
    Restrictions::setMin();
    _min = value;
}
void IntRestrictions::setMax(int value)
{
    Restrictions::setMax();
    _max = value;
}
int IntRestrictions::min() const { return _min; }
int IntRestrictions::max() const { return _max; }


Attributes::Attributes()
{
}

/** Adds a specific required string value */
void Attributes::addStringValue(const std::string &value)
{
    _stringValues.push_back(value);
    _stringRestrictions.push_back(StringRestrictions());
}
/** Adds a restriction instead of a specific string value */
void Attributes::addStringValue(StringRestrictions r)
{
    _stringValues.push_back(string());
    _stringRestrictions.push_back(r);
}

/** Adds a specific float value */
void Attributes::addFloatValue(float value)
{
    _floatValues.push_back(value);
    _floatRestrictions.push_back(FloatRestrictions());
}
/** Adds a restriction instead of a specific float value */
void Attributes::addFloatValue(FloatRestrictions r)
{
    _floatValues.push_back(0.0);
    _floatRestrictions.push_back(r);
}

/** Adds a specific int value */
void Attributes::addIntValue(int value)
{
    _intValues.push_back(value);
    _intRestrictions.push_back(IntRestrictions());
}
/** Adds a restriction instead of a specific int value */
void Attributes::addIntValue(IntRestrictions r)
{
    _intValues.push_back(0);
    _intRestrictions.push_back(r);
}

const std::vector<std::string> &Attributes::stringValues() const { return _stringValues; }
const std::vector<double> &Attributes::floatValues() const { return _floatValues; }
const std::vector<int> &Attributes::intValues() const { return _intValues; }

const std::vector<StringRestrictions> &Attributes::stringRestrictions() const { return _stringRestrictions; }
const std::vector<FloatRestrictions> &Attributes::floatRestrictions() const { return _floatRestrictions; }
const std::vector<IntRestrictions> &Attributes::intRestrictions() const { return _intRestrictions; }

