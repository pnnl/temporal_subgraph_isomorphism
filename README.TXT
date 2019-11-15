
=============================================
Source Code for Temporal Subgraph Isomorphism
Patrick Mackey, 2017 - 2019
=============================================

To build, run make.
(Note: Requires gcc 4.9 or newer)

Builds executable: graph_search

Run graph_search with no command line parameters to get list of possible options.

Current tool supports GDF files only, for both the larger data graph and query graph.
For more information on the GDF file format, see here:
https://gephi.org/users/supported-graph-formats/gdf-format/

IMPORTANT NOTES ON PERFORMING QUERIES:

 * Graphs must have one (and only one) "time" attribute on the edges for the algorithm to 
   work. The value type should be INT, and represents seconds (i.e., Unix time).

 * The number and types of attributes of the data and query graphs MUST match.

 * Leaving an attribute blank in the query graph will match any values for that attribute.

 * Numerical attributes can have an inequality value in the query graph.  For example,
   having an attribute have the value of ">30" for an edge weight will mean it will match
   any edge with a weight of at least 30.

 * The "time" associated with edges in a query graph can be just assigned sequential
   numbers (e.g., 1, 2, 3) as the exact values are not important, only their order.

 * The final results are represented by a single GDF graph that represents all the nodes
   and edges that exist on one or more of the isomorphic subgraphs.

EXAMPLE GRAPHS:

All example data and query graphs can be found in the "examples" subfolder.  This contains
two subfolders:

 * "non-attributed": Contains data and query graphs with no attributes except for "time".

 * "attributed": Contains data and query graphs with additional attributes to demonstrate 
   their use.

For more information about the algorithm, see our paper:
"A Chronological Edge-Driven Approach to Temporal Subgraph Isomorphism",
Patrick Mackey, Katherine Porterfield, Erin Fitzhenry, Sutanay Choudhury, George Chin.
IEEE Big Data, 2018
https://ieeexplore.ieee.org/abstract/document/8622100
https://arxiv.org/abs/1801.08098

For questions or more information, feel free to contact: patrick.mackey@pnnl.gov
