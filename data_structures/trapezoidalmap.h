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
    //void swapAndDeleteTrapezoid(size_t id_to_delete);
    void deleteTrapezoid(size_t id_to_delete);
    const std::vector<Trapezoid>& getTrapezoids() const;
    size_t getTrapezoidsNumber();
    Trapezoid* getTrapezoid(size_t id);
    Trapezoid& trapezoid(size_t id);
    cg3::Segment2d& segment(size_t id);
    cg3::Point2d& point(size_t id);

    size_t id_trapezoid_found;

private:
    std::vector<Trapezoid> trapezoids;

    // the segments and points vectors are used only for support the DAG, may contain duplicate elements, but space
    // complexity is the same and I avoid preventing duplicate elements and the consequent time complexity, +space,-time!
    std::vector<cg3::Segment2d> segments;
    std::vector<cg3::Point2d> points;

};

/**
 * @brief TrapezoidalMap::trapezoid inline: execution should be faster
 * @param id: an index of a trapezoid
 * @return the reference for the trapezoid id
 */
inline Trapezoid& TrapezoidalMap::trapezoid(size_t id) {
    return trapezoids[id];
}

inline cg3::Segment2d& TrapezoidalMap::segment(size_t id) {
    return segments[id];
}

inline cg3::Point2d& TrapezoidalMap::point(size_t id) {
    return points[id];
}

#endif // TRAPEZOIDALMAP_H
