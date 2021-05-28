#ifndef NODE_H
#define NODE_H

#include <cstddef>
#include <iostream>

/**
 * @brief Class representing the node of a graph with max 2 childs
 *
 * Minimal (third) version. I only store a "value" that represents an index to a vector of points (if XNODE), segments (if YNODE)
 * or trapezoids (if "type" is LEAF). In general, this "value" can be used for any data structure in  which the elements are
 * uniquely identified by a size_t index.
 */
class Node {

public:
    enum NodeType {XNODE, YNODE, LEAF};
    Node(NodeType node_type, size_t node_value);

    void setLeftChild(size_t left_child);
    void setRightChild(size_t right_child);
    size_t getLeftChild();
    size_t getRightChild();
    NodeType getType();

private:
    NodeType type;
    size_t value;       // an index to a point (if XNODE), a segment (if YNODE) or a trapezoid (if "type" is LEAF)
    size_t left_child_index;   // index of the left child
    size_t right_child_index;  // index of the right child
};

#endif // NODE_H
