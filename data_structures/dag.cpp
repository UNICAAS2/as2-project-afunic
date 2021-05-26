#include "dag.h"

Dag::Dag() {
    // TODO: set bbox in root?
}

/**
 * @brief Dag::~Dag delete nodes of the graph
 */
Dag::~Dag() {
    clear();
}

/**
 * @brief Dag::clear remove all elements
 */
void Dag::clear() {
    nodes.clear();
}

/**
 * @brief Dag::addNode add a node in the DAG
 * @return the direct reference to the new node
 */
Node * Dag::addNode(Node node) {
    nodes.push_back(node);
    return &nodes.back();
}
