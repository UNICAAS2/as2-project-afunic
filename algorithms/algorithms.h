#ifndef ALGORITHMS_H
#define ALGORITHMS_H

#include "data_structures/trapezoidalmap.h"
#include "data_structures/dag.h"

namespace Algorithms {

    void addSegmentToTrapezoidalMap(TrapezoidalMap& tmap, Dag& dag, cg3::Segment2d segment);
    // std::vector<Trapezoid*> FollowSegment(TrapezoidalMap& tmap, Dag& dag, cg3::Segment2d segment);
    std::vector<size_t> FollowSegment(TrapezoidalMap& tmap, Dag& dag, cg3::Segment2d segment);
    Node* queryTrapezoidalMap(cg3::Point2d point, cg3::Point2d* sec_point, TrapezoidalMap& tmap, Dag& dag);

    //void oneTrapezoidIntersection(Trapezoid* intersected, TrapezoidalMap& tmap, Dag& dag, cg3::Segment2d segment);
    void oneTrapezoidIntersection(size_t id_intersected, TrapezoidalMap& tmap, Dag& dag, cg3::Segment2d segment);
    //void twoOrMoreTrapezoidsIntersection(std::vector<Trapezoid*> traps_intersect, TrapezoidalMap& tmap, Dag& dag, cg3::Segment2d segment);
    void twoOrMoreTrapezoidsIntersection(std::vector<size_t> traps_intersect, TrapezoidalMap& tmap, Dag& dag, cg3::Segment2d segment);

}

#endif // ALGORITHMS_H
