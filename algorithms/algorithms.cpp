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
        dag.addNode(Node(Node::LEAF, id_bb), true); // set new node as root node
        // tmap.trapezoid(id_bb).setLeafNode(); // id_bb = 0 and ag.addNode(...)=0
    }
    // is p1 the left endpoint? If no swap endoints
    if (segment.p1().x()>segment.p2().x()) {
        cg3::Point2d buf = segment.p2();
        segment.setP2(segment.p1());
        segment.setP1(buf);
    }
    // trapezoids intersecting the segment
    // std::vector<size_t> traps_intersect=FollowSegment(tmap, dag, segment);
    std::vector<Node*> traps_intersect=FollowSegment(tmap, dag, segment);
    if (traps_intersect.size()==1) {
        // oneTrapezoidIntersection

    } else {
        // twoOrMoreTrapezoidsIntersection

    }
}

std::vector<Node*> FollowSegment(TrapezoidalMap& tmap, Dag& dag, cg3::Segment2d segment) {
    std::vector<Node*> trap_nodes;
    trap_nodes.push_back(queryTrapezoidalMap(segment.p1(), tmap, dag));

    // todo, andare vs destra x trovare gli altri

    return trap_nodes;
}

/**
 * @brief Algorithms::queryTrapezoidalMap perform the point query
 * @return the pointer of the Leaf that store the trapezoid ID (return->getValue())
 */
Node* queryTrapezoidalMap(cg3::Point2d point, TrapezoidalMap& tmap, Dag& dag) {
    Node* node=dag.getRootNode();
    while (node->getType()!=Node::LEAF) {
        if (node->getType()==Node::XNODE) { // X node
            if (point.x()<tmap.point(node->getValue()).x())
                node=node->getLeftChild();
            else
                node=node->getRightChild();
        } else { // Segment node
            size_t id_seg=node->getValue();
            double d=(point.x()-tmap.segment(id_seg).p1().x()) * (tmap.segment(id_seg).p2().y()-tmap.segment(id_seg).p1().y()) -
                     (point.y()-tmap.segment(id_seg).p1().y()) * (tmap.segment(id_seg).p2().x()-tmap.segment(id_seg).p1().x()); // d=(x−x1)(y2−y1)−(y−y1)(x2−x1)
            if (d>0) // if is above
                node=node->getLeftChild();
            else
                node=node->getRightChild();
        }
    }
    return node;
}

/**
 * @brief Algorithms::oneTrapezoidIntersection Split a trapezoid in 4
 * the segment lies entirely in a trapezoid
 *
 * @param id_trap id of the trapezoid to split
 * TODO
 */
void oneTrapezoidIntersection(Node* leaf, TrapezoidalMap& tmap, Dag& dag, cg3::Segment2d segment) {
    size_t id_trap=leaf->getValue();

    // add the 4 trapezoids in tmap
    size_t leftT=tmap.addTrapezoid(Trapezoid(tmap.trapezoid(id_trap).getLeftPoint(), tmap.trapezoid(id_trap).getTopSegment(), segment.p1(), tmap.trapezoid(id_trap).getBottomSegment()));
    size_t topT=tmap.addTrapezoid(Trapezoid(segment.p1(), tmap.trapezoid(id_trap).getTopSegment(), segment.p2(), segment));
    size_t rightT=tmap.addTrapezoid(Trapezoid(segment.p2(), tmap.trapezoid(id_trap).getTopSegment(), tmap.trapezoid(id_trap).getRightPoint(), tmap.trapezoid(id_trap).getBottomSegment()));
    size_t bottomT=tmap.addTrapezoid(Trapezoid(segment.p2(), tmap.trapezoid(id_trap).getTopSegment(), tmap.trapezoid(id_trap).getRightPoint(), tmap.trapezoid(id_trap).getBottomSegment()));

    // create the sub graph: +2 XNodes, +1 YNode, +4 Leafs, bottom to top...
    Node* bottomL=dag.addNode(Node(Node::LEAF,id_trap)); //bottomT)); important! not bottomT but id_trap, see swapAndDeleteTrapezoid
    Node* topL=dag.addNode(Node(Node::LEAF,topT));
    Node* s1N=dag.addNode(Node(Node::YNODE,tmap.addSegment(segment), topL, bottomL));
    Node* rightL=dag.addNode(Node(Node::LEAF,rightT));
    Node* q1N=dag.addNode(Node(Node::XNODE,tmap.addPoint(segment.p2()), s1N, rightL));
    Node* leftL=dag.addNode(Node(Node::LEAF,leftT));
    // link the new subgraph
    leaf->update(Node::XNODE,tmap.addPoint(segment.p1()),leftL, q1N);

    // delete the original trapezoid
    tmap.swapAndDeleteTrapezoid(id_trap);
}

}
