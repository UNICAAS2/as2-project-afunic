#ifndef ALGORITHMS_H
#define ALGORITHMS_H

#include "data_structures/trapezoidalmap.h"
#include "data_structures/dag.h"

namespace Algorithms {
    void addSegmentToTrapezoidalMap(TrapezoidalMap& tmap, Dag& dag, cg3::Segment2d segment);    
    std::vector<Trapezoid*> FollowSegment(TrapezoidalMap& tmap, Dag& dag, cg3::Segment2d segment);
    Node* queryTrapezoidalMap(cg3::Point2d point, TrapezoidalMap& tmap, Dag& dag);

    void setNeighborsOfNewTrapezoids(size_t leftT, size_t topT, size_t rightT, size_t bottomT, Trapezoid* intersected, TrapezoidalMap& tmap, Dag& dag, cg3::Segment2d segment);
    void oneTrapezoidIntersection(Trapezoid* intersected, TrapezoidalMap& tmap, Dag& dag, cg3::Segment2d segment);
    void twoOrMoreTrapezoidsIntersection(std::vector<Trapezoid*> intersected, TrapezoidalMap& tmap, Dag& dag, cg3::Segment2d segment);

    bool areEqual (cg3::Point2d p1, cg3::Point2d p2);
}

#endif // ALGORITHMS_H
