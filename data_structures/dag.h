#ifndef DAG_H
#define DAG_H

#include "node.h"
#include <list>

/**
 * @brief The DAG data structure
 *
 * I have replaced (again) the vector with a list.
 */
class Dag {

public:
    Dag();
    ~Dag();
    void clear();

    Node* addNode(Node node, bool isRootNode=false);
    Node* getRootNode();
    // inline Node& node(size_t id); TOCUT: vector versioni...

private:
    Node* rootNode;
    std::list<Node> nodes;

};

#endif // DAG_H
