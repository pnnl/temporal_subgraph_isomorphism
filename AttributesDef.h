
#ifndef ATTRIBUTESDEF_H
#define ATTRIBUTESDEF_H

#include <string>
#include <vector>
#include "Attributes.h"

/** Data types associated with attributes */
enum DataType { STRING, INT, FLOAT };

/** The kind of attribute it refers to */
enum AttributeType { SOURCE_ID, DEST_ID, NODE_ID, TIME, DATA };

/**
 * Defines what kind of attributes are associated with a particular graph.
 */
class AttributesDef
{
public:
    AttributesDef();
    /**
     * Add an attribute to this definition.
     * @param attType  What kind of attribute is this (ID, TIME, DATA)
     * @param dataType  What kind of data does it represent (int, string, float)
     * @param name  The name of this attribute.
     */
    void addAttribute(AttributeType attType, DataType dataType, const std::string &name);
    /** Returns true if the attributes matches the types and sizes associated with this definition */
    bool isSizeMatch(const Attributes &a) const;
    /** List of all attribute types (distinguishes if it's an ID, TIME or DATA) in their original order*/
    const std::vector<AttributeType> &attributeTypes() const;
    /** List of all attribute data types (including non-data ones) in their original order */
    const std::vector<DataType> &dataTypes() const;
    /** List of all attribute names (including non-data ones) in their original order*/
    const std::vector<std::string> &attributeNames() const;
    /** Number of DATA attributes that are strings */
    int numStringValues() const;
    /** Number of DATA attributes that are ints */
    int numIntValues() const;
    /** Number of DATA attributes that are floats */
    int numFloatValues() const;
    /** Test for equality */
    bool operator==(const AttributesDef &rhs) const;
    /** Test for inequality */
    bool operator!=(const AttributesDef &rhs) const;
private:
    std::vector<AttributeType> _attributeTypes;
    std::vector<DataType> _dataTypes;
    std::vector<std::string> _names;
    int _numStringValues, _numIntValues, _numFloatValues;
};

#endif /* ATTRIBUTESDEF_H */

