#ifndef DATA_GRAPH__H
#define DATA_GRAPH__H

#include "Attributes.h"
#include "AttributesDef.h"
#include "Graph.h"
#include "GraphMatch.h"
#include <unordered_map>
#include <vector>

/**
 * Subclass of our standard directed, temporal Graph class that also
 * supports attributes on both nodes and edges.
 */
class DataGraph : public Graph
{
public:
    virtual void addNode(int v) override;
    virtual void addNode(const std::string &name, const Attributes &a);
    virtual void addEdge(int u, int v) override;
    virtual void addEdge(int u, int v, time_t dateTime) override;
    virtual void addEdge(const std::string &source, const std::string &dest, time_t dateTime, const Attributes &a);
    virtual void copyEdge(int edgeIndex, const Graph &g) override;
    virtual void setNodeAttributesDef(const AttributesDef &def);
    virtual void setEdgeAttributesDef(const AttributesDef &def);
    //virtual void setNodeAttributes(int v, const Attributes &attributes);
    //virtual void setEdgeAttributes(int e, const Attributes &attributes);
    virtual DataGraph createSubGraph(const std::vector<GraphMatch> &matches) const;
    virtual const std::string &getName(int v) const;
    virtual void dispNode(int u) const;
    virtual void dispEdge(int e) const;
    virtual void disp() const;
    const std::vector<Attributes> &nodeAttributes() const;
    const std::vector<Attributes> &edgeAttributes() const;
    const AttributesDef &nodeAttributesDef() const;
    const AttributesDef &edgeAttributesDef() const;
    //const std::vector<std::string> &nodeAttributeNames() const;
    //const std::vector<std::string> &edgeAttributeNames() const;

protected:
    virtual void updateOrderedEdges() const override;

private:
    std::vector<std::string> _nodeNames;
    std::unordered_map<std::string,int> _nodeNameMap;
    AttributesDef _nodeAttributesDef, _edgeAttributesDef;
    std::vector<Attributes> _nodeAttributes;
    std::map<time_t,std::vector<Attributes>> _timeEdgeAttributesMap;
    mutable std::vector<Attributes> _edgeAttributes;
};

#endif
