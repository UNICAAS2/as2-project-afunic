#ifndef TRAPEZOID_H
#define TRAPEZOID_H

#include <cg3/geometry/segment2.h>

/**
 * @brief Class representing the trapezoid
 *
 * TODO
 */
class Trapezoid {

public:
    Trapezoid(cg3::Point2d left, cg3::Segment2d top, cg3::Point2d right, cg3::Segment2d bottom);
    void setNeighbors(size_t top_left, size_t top_right, size_t bottom_right, size_t bottom_left);
    const std::array<cg3::Point2d, 4> getVertices() const;

private:
    double getIntersectionYCoord(double xSlab, cg3::Segment2d segment) const;
    cg3::Point2d left_point;
    cg3::Segment2d top_segment;
    cg3::Point2d right_point;
    cg3::Segment2d bottom_segment;

    size_t top_left_neighbor; // Adjacents trapezoids
    size_t top_right_neighbor;
    size_t bottom_right_neighbor;
    size_t bottom_left_neighbor;

    size_t leaf_node_id; //punta al nodo foglia del dag
};

#endif // TRAPEZOID_H
