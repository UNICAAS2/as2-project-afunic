#ifndef DAG_H
#define DAG_H

#include "node.h"
#include <vector>

/**
 * @brief The DAG data structure
 *
 * Third version !!!! I have (again) replaced the list with a vector. In the DAG there are no deletions but only additions, the
 * trapezoid/leaf is always replaced by a subgraph...
 */
class Dag {

public:
    Dag();
    ~Dag();
    void clear();

    size_t addNode(Node node);

private:
    std::vector<Node> nodes;

};

#endif // DAG_H
