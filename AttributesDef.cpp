#include "AttributesDef.h"
#include "Attributes.h"

using namespace std;

AttributesDef::AttributesDef()
{
    _numStringValues = 0;
    _numFloatValues = 0;
    _numIntValues = 0;
}

void AttributesDef::addAttribute(AttributeType attributeType, DataType dataType, const std::string &name)
{
    _attributeTypes.push_back(attributeType);
    _dataTypes.push_back(dataType);
    _names.push_back(name);
    if(attributeType == AttributeType::DATA)
    {
        if(dataType == DataType::STRING)
            _numStringValues++;
        else if(dataType == DataType::FLOAT)
            _numFloatValues++;
        else if(dataType == DataType::INT)
            _numIntValues++;
        else
            throw "Can't add attribute to definition. Unknown type.";
    }
}
    
bool AttributesDef::isSizeMatch(const Attributes &a) const
{
    return a.floatValues().size() == _numFloatValues && a.intValues().size() == _numIntValues && a.stringValues().size() == _numStringValues;
}

const std::vector<AttributeType> &AttributesDef::attributeTypes() const
{
    return _attributeTypes;
}
    
const std::vector<DataType> &AttributesDef::dataTypes() const
{
    return _dataTypes;
}
    
const std::vector<std::string> &AttributesDef::attributeNames() const
{
    return _names;
}
    
int AttributesDef::numStringValues() const { return _numStringValues; }
    
int AttributesDef::numIntValues() const { return _numIntValues; }
    
int AttributesDef::numFloatValues() const { return _numFloatValues; }

bool AttributesDef::operator==(const AttributesDef &rhs) const
{
    int n = this->_names.size();
    if(n != rhs.attributeNames().size())
        return false;
    for(int i=0; i<n; i++)
    {
        if(_names[i] != rhs._names[i])
            return false;
        if(this->_dataTypes[i] != rhs._dataTypes[i])
            return false;
        if(this->_attributeTypes[i] != rhs._attributeTypes[i])
            return false;
    }
    return true;
}

bool AttributesDef::operator!=(const AttributesDef &rhs) const
{
    return !(*this == rhs);
}
