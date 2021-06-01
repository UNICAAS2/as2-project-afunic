#ifndef NODE_H
#define NODE_H

#include <cstddef>
#include <iostream>

/**
 * @brief Class representing the node of a graph with max 2 childs
 * I only store a "value" that represents an index of a vector of points (if XNODE), of segments (if YNODE)
 * or a vector of trapezoids (if "type" is LEAF). In general, this "value" can be used for any data structure in
 * which the elements are uniquely identified by a size_t index.
 */
class Node {

public:
    enum NodeType {XNODE, YNODE, LEAF};
    Node(NodeType node_type, size_t node_value, Node* left_child=nullptr, Node* right_child=nullptr);

    void update(NodeType node_type, size_t node_value, Node* left_child, Node* right_child);
    void setLeftChild(Node* left_child); // non più usata?
    void setRightChild(Node* right_child); // non più usata?
    void setChilds(Node* left_child, Node* right_child); // shortcut
    Node* getLeftChild();
    Node* getRightChild();
    NodeType getType();
    size_t getValue();

private:
    NodeType type;
    size_t value; // an index of a point (if XNODE)/segment (if YNODE)/trapezoid (if "type" is LEAF)
    Node* leftChild;
    Node* rightChild;
};

#endif // NODE_H
