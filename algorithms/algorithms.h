#ifndef ALGORITHMS_H
#define ALGORITHMS_H

#include "data_structures/trapezoidalmap.h"
#include "data_structures/dag.h"

namespace Algorithms {
    void addSegmentToTrapezoidalMap(TrapezoidalMap& tmap, Dag& dag, cg3::Segment2d segment);
    std::vector<Node*> FollowSegment(TrapezoidalMap& tmap, Dag& dag, cg3::Segment2d segment);
    Node* queryTrapezoidalMap(cg3::Point2d point, TrapezoidalMap& tmap, Dag& dag);
}

#endif // ALGORITHMS_H
