#ifndef ALGORITHMS_H
#define ALGORITHMS_H

#include "data_structures/trapezoidalmap.h"
#include "data_structures/dag.h"

namespace Algorithms {
    void addSegmentToTrapezoidalMap(TrapezoidalMap& tmap, Dag& dag, cg3::Segment2d segment);    
    std::vector<Trapezoid*> FollowSegment(TrapezoidalMap& tmap, Dag& dag, cg3::Segment2d segment);
    Node* queryTrapezoidalMap(cg3::Point2d point, cg3::Point2d* sec_point, TrapezoidalMap& tmap, Dag& dag);

    // ok the next 5 functions could be moved to an external file like misc.h, util.h etc.
    double pointDirection(cg3::Point2d p, cg3::Segment2d s);
    bool isAbove(cg3::Point2d p, cg3::Point2d* sec_point, cg3::Segment2d s);
    bool areEqual(double f1, double f2);
    bool areEqual(cg3::Point2d p1, cg3::Point2d p2);
    bool areEqual(cg3::Segment2d s1, cg3::Segment2d s2);
}

#endif // ALGORITHMS_H
