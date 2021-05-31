#ifndef ALGORITHMS_H
#define ALGORITHMS_H

#include "data_structures/trapezoidalmap.h"
#include "data_structures/dag.h"

namespace Algorithms {
    void addSegmentToTrapezoidalMap(TrapezoidalMap& tmap, Dag& dag, cg3::Segment2d segment);
    std::vector<size_t> FollowSegment(TrapezoidalMap& tmap, Dag& dag, cg3::Segment2d segment);
    size_t queryTrapezoidalMap(cg3::Point2d point, TrapezoidalMap& tmap, Dag& dag);
}

#endif // ALGORITHMS_H
