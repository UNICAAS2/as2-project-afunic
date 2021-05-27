#include "node.h"

/**
 * @brief construtor
 * @param node_type the node type (x,y,leaf)
 * @param node_value index to a vector of points (if XNODE), segments (if YNODE) or trapezoids (if "type" is LEAF) or any other vector.
 */
Node::Node(NodeType node_type, size_t node_value) {
    type = node_type;
    value = node_value;
    leftChild=SIZE_MAX; // like nullptr, SIZE_MAX => null value
    rightChild=SIZE_MAX;
}

/**
 * @brief Set the left child
 * @param lc the index of the left child
 */
void Node::setLeftChild(size_t lc) {
    leftChild=lc;
}

/**
 * @brief Set the right child
 * @param rc the index of the right child
 */
void Node::setRightChild(size_t rc) {
    rightChild=rc;
}

/**
 * @brief Get the left child
 * @return the index of the left child
 */
size_t Node::getLeftChild() {
    return leftChild;
}

/**
 * @brief Get the right child
 * @return the index of the right child
 */
size_t Node::getRightChild() {
    return rightChild;
}

/**
 * @brief Get the node type
 * @return the node type: XNODE, YNODE or LEAF
 */
Node::NodeType Node::getType() {
    return type;
}
