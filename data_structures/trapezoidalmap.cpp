#include "trapezoidalmap.h"

TrapezoidalMap::TrapezoidalMap():id_trapezoid_found(SIZE_MAX) {
}

/**
 * @brief TrapezoidalMap::~TrapezoidalMap delete all trapezoids
 */
TrapezoidalMap::~TrapezoidalMap() {
    clear();
}

/**
 * @brief TrapezoidalMap::clear remove all elements
 */
void TrapezoidalMap::clear() {
    id_trapezoid_found=SIZE_MAX;
    trapezoids.clear();
    segments.clear();
    points.clear();
}

/**
 * @brief TrapezoidalMap::addNode add a trapezoid in the map
 * @return the direct reference to the new trapezoid
 */
size_t TrapezoidalMap::addTrapezoid(Trapezoid trapezoid) {
    trapezoids.push_back(trapezoid);
    return trapezoids.size()-1;
}

size_t TrapezoidalMap::addSegment(cg3::Segment2d segment) {
    segments.push_back(segment);
    return segments.size()-1;
}

size_t TrapezoidalMap::addPoint(cg3::Point2d point) {
    points.push_back(point);
    return points.size()-1;
}

Trapezoid* TrapezoidalMap::getTrapezoid(size_t id) {
    if (id==SIZE_MAX) return nullptr;
    return &trapezoids[id];
}

void TrapezoidalMap::deleteTrapezoid(size_t id_to_delete) {
    trapezoids[id_to_delete].deleteTrapezoid();
}

/**
 * @brief TrapezoidalMap::getTrapezoidsNumber
 * @return the number of trapezoids
 */
size_t TrapezoidalMap::getTrapezoidsNumber() {
    return trapezoids.size();
}

/**
 * @brief TrapezoidalMap::getTrapezoids
 * @return
 */
const std::vector<Trapezoid>& TrapezoidalMap::getTrapezoids() const {
    return trapezoids;
}

