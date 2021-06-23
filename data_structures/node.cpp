#include "node.h"

/**
 * @brief construtor
 * @param node_type the node type (x,y,leaf)
 * @param node_value index to a vector of points (if XNODE)/segments (if YNODE)/trapezoids (if "type" is LEAF)/any other vector.
 */
Node::Node(NodeType node_type, size_t node_value, Node* left_child, Node* right_child):
    type(node_type),
    value(node_value),
    leftChild(left_child),
    rightChild(right_child)
{}

/**
 * @brief Node::update update a node, used to replace leaves with subgraphs
 * @param node_type
 * @param node_value
 * @param left_child
 * @param right_child
 */
void Node::update(NodeType node_type, size_t node_value, Node* left_child, Node* right_child) {
    type=node_type;
    value=node_value;
    leftChild=left_child;
    rightChild=right_child;
}

/**
 * @brief Set the left child
 */
void Node::setLeftChild(Node* left_child) {
    leftChild=left_child;
}

/**
 * @brief Set the right child
 */
void Node::setRightChild(Node* right_child) {
    rightChild=right_child;
}

void Node::setChilds(Node* left_child, Node* right_child) {
    this->setLeftChild(left_child);
    this->setRightChild(right_child);
}

/**
 * @brief Get the left child
 */
Node* Node::getLeftChild() {
    return leftChild;
}

/**
 * @brief Get the right child
 */
Node* Node::getRightChild() {
    return rightChild;
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
