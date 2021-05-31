#include "node.h"

/**
 * @brief construtor
 * @param node_type the node type (x,y,leaf)
 * @param node_value index to a vector of points (if XNODE), segments (if YNODE) or trapezoids (if "type" is LEAF) or any other vector.
 */
Node::Node(NodeType node_type, size_t node_value):
    type(node_type),
    value(node_value),
    left_child_index(SIZE_MAX), // like nullptr, SIZE_MAX => null value
    right_child_index(SIZE_MAX)
{}

/**
 * @brief Set the left child
 * @param lc the index of the left child
 */
void Node::setLeftChild(size_t left_child) {
    left_child_index=left_child;
}

/**
 * @brief Set the right child
 * @param rc the index of the right child
 */
void Node::setRightChild(size_t right_child) {
    right_child_index=right_child;
}

/**
 * @brief Get the left child
 * @return the index of the left child
 */
size_t Node::getLeftChild() {
    return left_child_index;
}

/**
 * @brief Get the right child
 * @return the index of the right child
 */
size_t Node::getRightChild() {
    return right_child_index;
}

/**
 * @brief Get the node type
 * @return the node type: XNODE, YNODE or LEAF
 */
Node::NodeType Node::getType() {
    return type;
}

size_t Node::getValue() {
    return value;
}
