#include "trapezoidalmap.h"

TrapezoidalMap::TrapezoidalMap() {
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

/**
 * @brief TrapezoidalMap::swapAndDeleteTrapezoid delete a trapezoid
 * Swap the trapezoid with last element and then delete the last element, all in costant time.
 * Pay attention: data references to [id_to_delete] should be updated previously,
 * an example of use in Algorithms::oneTrapezoidIntersection
 */
void TrapezoidalMap::swapAndDeleteTrapezoid(size_t id_to_delete) {
    trapezoids[id_to_delete]=trapezoids.back(); // O(1)
    trapezoids.pop_back(); // O(1)
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

/**
 * @brief TrapezoidalMap::trapezoid inline: execution should be faster
 * @param id: an index of a trapezoid
 * @return the reference for the trapezoid id
 */
inline Trapezoid& TrapezoidalMap::trapezoid(size_t id) {
    return trapezoids[id];
}

inline cg3::Segment2d& TrapezoidalMap::segment(size_t id){
    return segments[id];
}

inline cg3::Point2d& TrapezoidalMap::point(size_t id){
    return points[id];
}
