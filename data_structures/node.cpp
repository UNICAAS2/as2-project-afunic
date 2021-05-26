#include "node.h"

/**
 * @brief construtor
 * @param node_type the node type (x,y,leaf)
 * @param node_value pointer to a point (if XNODE), a segment (if YNODE) or a trapezoid (if "type" is LEAF)
 */
Node::Node(NodeType node_type, void * node_value) {
    type = node_type;
    value = node_value;
    leftChild=nullptr;
    rightChild=nullptr;
}

/**
 * @brief Set the left child
 * @param lc pointer tothe left child
 */
void Node::setLeftChild(Node * lc) {
    leftChild=lc;
}

/**
 * @brief Set the right child
 * @param rc pointer the right child
 */
void Node::setRightChild(Node * rc) {
    rightChild=rc;
}

/**
 * @brief Get the left child
 * @return the pointer to the left child
 */
Node * Node::getLeftChild() {
    return leftChild;
}

/**
 * @brief Get the right child
 * @return the pointer to the right child
 */
Node * Node::getRightChild() {
    return rightChild;
}

/**
 * @brief Get the node type
 * @return the node type: XNODE, YNODE or LEAF
 */
Node::NodeType Node::getType() {
    return type;
}
