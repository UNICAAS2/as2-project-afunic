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
    Trapezoid(cg3::Point2d leftP, cg3::Segment2d top, cg3::Point2d rightP, cg3::Segment2d bottom);

private:
    cg3::Point2d _leftP;
    cg3::Segment2d _top;
    cg3::Point2d _rightP;
    cg3::Segment2d _bottom;

    Trapezoid * _tln; // Adjacents trapezoids, to left neighbor
    Trapezoid * _trn; // top right
    Trapezoid * _brn; // bottom right
    Trapezoid * _bln; // bottom left

    Node * _node;
};

#endif // TRAPEZOID_H
