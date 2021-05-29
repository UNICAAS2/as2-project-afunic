#include "trapezoidalmap.h"

TrapezoidalMap::TrapezoidalMap() {
 // TODO
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
}

/**
 * @brief TrapezoidalMap::addNode add a trapezoid in the map
 * @return the direct reference to the new trapezoid
 */
size_t TrapezoidalMap::addTrapezoid(Trapezoid trapezoid) {
    trapezoids.push_back(trapezoid);
    return trapezoids.size()-1;
}

/**
 * @brief TrapezoidalMap::getTrapezoids
 * @return
 */
const std::vector<Trapezoid>& TrapezoidalMap::getTrapezoids() const
{
    return trapezoids;
}
