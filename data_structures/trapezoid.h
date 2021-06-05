#ifndef TRAPEZOID_H
#define TRAPEZOID_H

#include <cg3/geometry/segment2.h>
#include "node.h"

/**
 * @brief Class representing the trapezoid
 *
 * TODO
 */
class Trapezoid {

public:
    enum NeighborType {TL, TR, BR, BL};
    Trapezoid(cg3::Point2d left_p, cg3::Segment2d top_s, cg3::Point2d right_p, cg3::Segment2d bottom_s);

    void setNeighbors(size_t top_left, size_t top_right, size_t bottom_right, size_t bottom_left);
    void setLeafNode(Node* node);
    const std::array<cg3::Point2d, 4> getVertices() const;
    cg3::Point2d getLeftPoint();
    cg3::Segment2d getTopSegment();
    cg3::Point2d getRightPoint();
    cg3::Segment2d getBottomSegment();
    Node* getLeafNode();
    size_t getNeighbor(NeighborType pos);
    void setNeighbor(NeighborType pos, size_t id_neighbor);
    double getIntersectionYCoord(double xSlab, cg3::Segment2d segment) const;
    void setRightPoint(cg3::Point2d p);
    void deleteTrapezoid();
    bool isDeleted() const;// used only in drawableTrapezoidalMap::draw()

private:
    cg3::Point2d left_point;
    cg3::Segment2d top_segment;
    cg3::Point2d right_point;
    cg3::Segment2d bottom_segment;

    size_t top_left_neighbor; // Adjacents trapezoids
    size_t top_right_neighbor;
    size_t bottom_right_neighbor;
    size_t bottom_left_neighbor;

    bool deleted;

    Node* leaf_node; //punta al nodo foglia del dag
};

#endif // TRAPEZOID_H
