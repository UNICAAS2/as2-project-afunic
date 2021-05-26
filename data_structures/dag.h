#ifndef DAG_H
#define DAG_H

#include "node.h"
#include <list>

/**
 * @brief The DAG data structure
 *
 * In this second version I sotre the nodes of the DAG in a list. I've abandoned (unfortunately:-) the first "vector" version!
 */
class Dag {

public:
    Dag();
    ~Dag();
    void clear();

    Node * addNode(Node node);

private:
    std::list<Node> nodes;

};

#endif // DAG_H
