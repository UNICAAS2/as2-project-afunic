#ifndef ALGORITHMS_H
#define ALGORITHMS_H

#include "data_structures/trapezoidalmap.h"
#include "data_structures/dag.h"

namespace Algorithms {
    void addSegmentToTrapezoidalMap(TrapezoidalMap &tmap, Dag &dag, cg3::Segment2d segment);

}

#endif // ALGORITHMS_H
