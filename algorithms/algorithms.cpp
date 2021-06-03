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
    }
    // is p1 the left endpoint of the segment? If no swap endpoints
    if (segment.p1().x()>segment.p2().x()) {
        cg3::Point2d buf = segment.p2();
        segment.setP2(segment.p1());
        segment.setP1(buf);
    }
    // trapezoids intersecting the segment
    std::vector<Node*> traps_intersect=FollowSegment(tmap, dag, segment);
    if (traps_intersect.size()==1)
        oneTrapezoidIntersection(traps_intersect[0], tmap, dag, segment);
    else
        twoOrMoreTrapezoidsIntersection(traps_intersect, tmap, dag, segment);

    tmap.id_tmp=SIZE_MAX;
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
            if (d>0)
                node=node->getRightChild();
            else
                node=node->getLeftChild();
        }
    }
    tmap.id_tmp=node->getValue();
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
    size_t bottomT=tmap.addTrapezoid(Trapezoid(segment.p1(), segment, segment.p2(), tmap.trapezoid(id_trap).getBottomSegment()));

    // The neighbours (trapezoidi con spigoli verticali in comune)
    tmap.trapezoid(leftT).setNeighbors(tmap.trapezoid(id_trap).getTopLeftNeighbor(), topT, id_trap/*bottomT*/, tmap.trapezoid(id_trap).getBottomLeftNeighbor());
    tmap.trapezoid(topT).setNeighbors(leftT, rightT, rightT, leftT);
    tmap.trapezoid(rightT).setNeighbors(topT, tmap.trapezoid(id_trap).getTopRightNeighbor(), tmap.trapezoid(id_trap).getBottomRightNeighbor(), id_trap/*bottomT*/);
    tmap.trapezoid(bottomT).setNeighbors(leftT, rightT, rightT, leftT);

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

/**
 * @brief Algorithms::twoOrMoreTrapezoidsIntersection Split a trapezoid in 4
 * the segment lies entirely in a trapezoid
 *
 * @param id_trap id of the trapezoid to split
 * TODO
 */
void twoOrMoreTrapezoidsIntersection(std::vector<Node*> traps_intersect, TrapezoidalMap& tmap, Dag& dag, cg3::Segment2d segment) {
    return;

    if (traps_intersect.size()<2) return; // error, the point is not inside the bounding box, throw "MMM click on the bbox...";

    //***** First trap (leftmost)
    size_t id_trap=traps_intersect[0]->getValue();

    // add the 3 trapezoids in tmap
    size_t leftT=tmap.addTrapezoid(Trapezoid(tmap.trapezoid(id_trap).getLeftPoint(), tmap.trapezoid(id_trap).getTopSegment(), segment.p1(), tmap.trapezoid(id_trap).getBottomSegment()));
    size_t topT=tmap.addTrapezoid(Trapezoid(segment.p1(), tmap.trapezoid(id_trap).getTopSegment(), tmap.trapezoid(id_trap).getRightPoint(), segment));
    size_t bottomT=tmap.addTrapezoid(Trapezoid(segment.p1(), segment, tmap.trapezoid(id_trap).getRightPoint(), tmap.trapezoid(id_trap).getBottomSegment()));
/*
    // The neighbours (trapezoidi con spigoli verticali in comune)
    tmap.trapezoid(leftT).setNeighbors(tmap.trapezoid(id_trap).getTopLeftNeighbor(), topT, bottomT, tmap.trapezoid(id_trap).getBottomLeftNeighbor());
    //tmap.trapezoid(topT).setNeighbors(leftT, rightT, rightT, leftT);
    //tmap.trapezoid(rightT).setNeighbors(topT, tmap.trapezoid(id_trap).getTopRightNeighbor(), tmap.trapezoid(id_trap).getBottomRightNeighbor(), bottomT);
    //tmap.trapezoid(bottomT).setNeighbors(leftT, rightT, rightT, leftT);

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
    */
}

}
