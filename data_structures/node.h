#ifndef NODE_H
#define NODE_H

#include <cstddef>
#include <iostream>

/**
 * @brief Class representing the node of a graph with max 2 childs
 *
 * Minimal (second) version, I only store a "value" that represents a generic pointer to a point (if XNODE), a segment (if YNODE)
 * or a trapezoid (if "type" is LEAF).
 */
class Node {

public:
    enum NodeType {XNODE, YNODE, LEAF};
    Node(NodeType node_type, void * node_value);

    void setLeftChild(Node * lc);
    void setRightChild(Node * rc);
    Node * getLeftChild();
    Node * getRightChild();
    NodeType getType();

private:
    NodeType type;
    void * value;       // a generic pointer to a point (if XNODE), a segment (if YNODE) * or a trapezoid (if "type" is LEAF)
    Node * leftChild;   // pointer to the left child
    Node * rightChild;  // pointer to the right child
};

#endif // NODE_H
