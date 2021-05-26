#ifndef DAG_H
#define DAG_H

#include "node.h"
#include <vector>

/**
 * @brief The Dag class
 * I sotre the DAG in a vector, less code & same speed of a list (no deletions)
 */
class Dag {

public:
    Dag();
    ~Dag();
    void clear();

private:
    std::vector<Node> nodes;

};

#endif // DAG_H
