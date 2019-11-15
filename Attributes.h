#pragma once

#include <vector>
#include <string>

/**
 * Restrictions on an attribute (such as max, min, etc)
 */
class Restrictions
{
public:
    Restrictions();
    void allowAny();
    void setMin();
    void setMax();
    bool any() const;
    bool hasMin() const;
    bool hasMax() const;
    bool exact() const;
private:
    bool _any, _hasMin, _hasMax, _exact;
};

/**
 * Restrictions specifically for floating point attributes.
 */
class FloatRestrictions : public Restrictions
{
public:
    FloatRestrictions();
    void setMin(float value);
    void setMax(float value);
    float min() const;
    float max() const;
private:
    float _min, _max;
};

/**
 * Restrictions specifically for integer attributes.
 */
class IntRestrictions : public Restrictions
{
public:
    IntRestrictions();
    void setMin(int value);
    void setMax(int value);
    int min() const;
    int max() const;
private:
    int _min, _max;
};

/**
 * Restrictions specifically for text string attributes.
 */
class StringRestrictions : public Restrictions
{
};

/**
 * Holds attributes for a node or link in the graph.
 */
class Attributes
{
public:
    Attributes();
    /** Adds a specific required string value */
    void addStringValue(const std::string &value);
    /** Adds a restriction instead of a specific string value */
    void addStringValue(StringRestrictions r);
    /** Adds a specific float value */
    void addFloatValue(float value);
    /** Adds a restriction instead of a specific float value */
    void addFloatValue(FloatRestrictions r);
    /** Adds a specific int value */
    void addIntValue(int value);
    /** Adds a restriction instead of a specific int value */
    void addIntValue(IntRestrictions r);
    /** Values associated with DATA attributes that are strings (in order they were in the file) */
    const std::vector<std::string> &stringValues() const;
    /** Values associated with DATA attributes that are floats (in order they were in the file) */
    const std::vector<double> &floatValues() const;
    /** Values associated with DATA attributes that are ints (in order they were in the file) */
    const std::vector<int> &intValues() const;
    /** Restrictions on what the string value can be for DATA attributes */
    const std::vector<StringRestrictions> &stringRestrictions() const;
    /** Restrictions on what the float value can be for DATA attributes */
    const std::vector<FloatRestrictions> &floatRestrictions() const;
    /** Restrictions on what the int value can be for DATA attributes */
    const std::vector<IntRestrictions> &intRestrictions() const;
private:
    std::vector<std::string> _stringValues;
    std::vector<double> _floatValues;
    std::vector<int> _intValues;
    std::vector<StringRestrictions> _stringRestrictions;
    std::vector<FloatRestrictions> _floatRestrictions;
    std::vector<IntRestrictions> _intRestrictions;
};
