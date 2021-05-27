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
    Trapezoid(cg3::Point2d leftP, cg3::Segment2d top, cg3::Point2d rightP, cg3::Segment2d bottom);
    void setNeighbors(size_t tl, size_t tr, size_t br, size_t bl);

private:
    cg3::Point2d _leftP;
    cg3::Segment2d _top;
    cg3::Point2d _rightP;
    cg3::Segment2d _bottom;

    size_t _tln; // Adjacents trapezoids, to left neighbor
    size_t _trn; // top right
    size_t _brn; // bottom right
    size_t _bln; // bottom left

    size_t _node;
};

#endif // TRAPEZOID_H
