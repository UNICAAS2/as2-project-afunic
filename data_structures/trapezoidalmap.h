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

private:
    std::vector<Trapezoid> trapezoids;

};

#endif // TRAPEZOIDALMAP_H
