#include "dag.h"

Dag::Dag():
    rootNode(nullptr)
{}

/**
 * @brief Dag::~Dag delete all nodes of the graph
 */
Dag::~Dag() {
    clear();
}

/**
 * @brief Dag::clear remove all elements
 */
void Dag::clear() {
    rootNode=nullptr;
    nodes.clear();
}

/**
 * @brief Dag::addNode add a node in the DAG
 * @param node the node
 * @return the pointer of the new node
 */
Node* Dag::addNode(Node node, bool isRootNode) {
    nodes.push_back(node);
    if (isRootNode) rootNode=&nodes.back();
    return &nodes.back();
}

Node* Dag::getRootNode() {
    return rootNode;
}

/**
 * @brief Dag::node inline: execution should be faster
 * @param id: an index of a node
 * @return the reference for the node id
TOCUT, vector version...
inline Node& Dag::node(size_t id) {
    return nodes[id];
}
 */
