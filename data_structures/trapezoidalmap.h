#ifndef TRAPEZOIDALMAP_H
#define TRAPEZOIDALMAP_H

#include "trapezoid.h"
#include <vector>

/**
 * @brief Class representing the The TrapezoidalMap data structure T(S)
 *
 * TODO
 */
class TrapezoidalMap {

public:
    TrapezoidalMap();
    ~TrapezoidalMap();
    void clear();

    size_t addTrapezoid(Trapezoid trapezoid);
    size_t addSegment(cg3::Segment2d segment);
    size_t addPoint(cg3::Point2d point);
    const std::vector<Trapezoid>& getTrapezoids() const;
    size_t getTrapezoidsNumber();
    inline Trapezoid& trapezoid(size_t id);
    inline cg3::Segment2d& segment(size_t id);
    inline cg3::Point2d& point(size_t id);

private:
    std::vector<Trapezoid> trapezoids;
    std::vector<cg3::Segment2d> segments;
    std::vector<cg3::Point2d> points;

};

#endif // TRAPEZOIDALMAP_H
