#ifndef DAG_H
#define DAG_H

#include "node.h"
#include <list>

/**
 * @brief The DAG data structure
 *
 */
class Dag {

public:
    Dag();
    ~Dag();
    void clear();

    Node* addNode(Node node, bool isRootNode=false);
    Node* getRootNode();

private:
    Node* rootNode;
    std::list<Node> nodes;

};

#endif // DAG_H
