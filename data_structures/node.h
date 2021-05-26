#ifndef NODE_H
#define NODE_H

#include <cstddef>

/**
 * @brief The Node class
 * Minimal version, I only store a "value" that represents an index of a vector. The vector depends on the type of
 * node (XNODE, YNODE, LEAF). The responsibility and integrity of indices are handled at a higher level (e.g. DAG or algorithm).
 */
class Node {

public:
    enum NodeType {XNODE, YNODE, LEAF};
    Node(NodeType node_type, size_t element_id);

    void setLeftChild(size_t lc);
    void setRightChild(size_t rc);
    size_t getLeftChild();
    size_t getRightChild();

private:
    NodeType type;
    size_t value;       // this is an index in the vector points, segments or trapezoid
    size_t leftChild;   // this is an index of the vector nodes of the DAG
    size_t rightChild;  //           //                  //
};

#endif // NODE_H
