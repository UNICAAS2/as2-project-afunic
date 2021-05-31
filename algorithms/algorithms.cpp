#include "algorithms.h"

#define BOUNDINGBOX 1e+6

namespace Algorithms {

/**
 * @brief addSegmentToTrapezoidalMap TODO
 *
 * TODO
 *
 * @param TODO
 */
void addSegmentToTrapezoidalMap(TrapezoidalMap& tmap, Dag& dag, cg3::Segment2d segment) {
    // no segments? no panic, add the bounding box...
    if (tmap.getTrapezoidsNumber()<1) {
        cg3::Point2d ltp=cg3::Point2d(-BOUNDINGBOX, BOUNDINGBOX); // left top point
        cg3::Point2d rbp=cg3::Point2d(BOUNDINGBOX, -BOUNDINGBOX); // right bottom point
        size_t id_bb = tmap.addTrapezoid(Trapezoid(ltp, cg3::Segment2d(ltp, cg3::Point2d(BOUNDINGBOX, BOUNDINGBOX)), rbp, cg3::Segment2d(cg3::Point2d(-BOUNDINGBOX, -BOUNDINGBOX), rbp)));
        dag.addNode(Node(Node::LEAF, id_bb));
    }
    // is p1 the left endpoint? If no swap endoints
    if (segment.p1().x()>segment.p2().x()) {
        cg3::Point2d buf = segment.p2();
        segment.setP2(segment.p1());
        segment.setP1(buf);
    }
    // trapezoids intersecting the segment
    std::vector<size_t> traps=FollowSegment(tmap, dag, segment);
    if (traps.size()==1) {
    } else {
    }
}

std::vector<size_t> FollowSegment(TrapezoidalMap& tmap, Dag& dag, cg3::Segment2d segment) {
    std::vector<size_t> traps;
    traps.push_back(queryTrapezoidalMap(segment.p1(), tmap, dag));

    return traps;
}

size_t queryTrapezoidalMap(cg3::Point2d point, TrapezoidalMap& tmap, Dag& dag) {
    size_t idnode = 0; // root node
    while (dag.node(idnode).getType()!=Node::LEAF) {
        if (dag.node(idnode).getType()==Node::XNODE) { // X node
            if (point.x()<tmap.point(dag.node(idnode).getValue()).x())
                idnode=dag.node(idnode).getLeftChild();
            else
                idnode=dag.node(idnode).getRightChild();
        } else { // Segment node
            // todo
        }
    }
    return idnode;
}

}
