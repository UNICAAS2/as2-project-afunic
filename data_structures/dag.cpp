#include "dag.h"

Dag::Dag() {
    // TODO: set bbox in root?
}

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
    nodes.clear();
}

/**
 * @brief Dag::addNode add a node in the DAG
 * @param node the node
 * @return the index of the new node
 */
size_t Dag::addNode(Node node) {
    nodes.push_back(node);
    return nodes.size()-1;
}
