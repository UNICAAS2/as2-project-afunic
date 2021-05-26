#include "dag.h"

Dag::Dag() {

}

/**
 * @brief Dag::~Dag: delete all nodes
 */
Dag::~Dag() {
    clear();
}

/**
 * @brief Dag::clear: remove all elements
 */
void Dag::clear() {
    nodes.clear();
}
