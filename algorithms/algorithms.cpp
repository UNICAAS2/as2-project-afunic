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
    // no trapezoids? no panic, add the bounding box...
    if (tmap.getTrapezoidsNumber()<1) {
        cg3::Point2d ltp=cg3::Point2d(-BOUNDINGBOX, BOUNDINGBOX); // left top point
        cg3::Point2d rbp=cg3::Point2d(BOUNDINGBOX, -BOUNDINGBOX); // right bottom point
        size_t id_bb = tmap.addTrapezoid(Trapezoid(ltp, cg3::Segment2d(ltp, cg3::Point2d(BOUNDINGBOX, BOUNDINGBOX)), rbp, cg3::Segment2d(cg3::Point2d(-BOUNDINGBOX, -BOUNDINGBOX), rbp)));
        tmap.trapezoid(id_bb).setLeafNode(dag.addNode(Node(Node::LEAF, id_bb), true)); // set new node as root node
    }
    // is p1 the left endpoint of the segment? If no swap endpoints
    if (segment.p1().x()>segment.p2().x()) {
        cg3::Point2d buf = segment.p2();
        segment.setP2(segment.p1());
        segment.setP1(buf);
    }
    // trapezoids intersecting the segment
    std::vector<Trapezoid*> intersected=FollowSegment(tmap, dag, segment);
    if (intersected.size()==1)
        oneTrapezoidIntersection(intersected[0], tmap, dag, segment);
    else
        twoOrMoreTrapezoidsIntersection(intersected, tmap, dag, segment);
//    tmap.id_trapezoid_found=SIZE_MAX;
}

std::vector<Trapezoid*> FollowSegment(TrapezoidalMap& tmap, Dag& dag, cg3::Segment2d segment) {
    std::vector<Trapezoid*> traps;
    Node* leaf=queryTrapezoidalMap(segment.p1(), tmap, dag);
    Trapezoid* currtrap=tmap.getTrapezoid(leaf->getValue());
    traps.push_back(currtrap);

    while (segment.p2().x()>currtrap->getRightPoint().x()) { // ok ne interseco ancora almeno un altro a destra di intersected
        // quale prendere TRn o BRn?
        if (currtrap->getNeighbor(Trapezoid::TR)!=SIZE_MAX) {
            if (currtrap->getNeighbor(Trapezoid::BR)!=SIZE_MAX) {
                // può essere uno dei due, valuto in base alla y
                if (currtrap->getIntersectionYCoord(currtrap->getRightPoint().x(), segment)>currtrap->getRightPoint().y())
                    currtrap=tmap.getTrapezoid(currtrap->getNeighbor(Trapezoid::TR));
                else
                    currtrap=tmap.getTrapezoid(currtrap->getNeighbor(Trapezoid::BR));
            } else
                currtrap=tmap.getTrapezoid(currtrap->getNeighbor(Trapezoid::TR));
        } else
            currtrap=tmap.getTrapezoid(currtrap->getNeighbor(Trapezoid::BR));
        traps.push_back(currtrap);
tmap.id_trapezoid_found=currtrap->getLeafNode()->getValue();
    }
    return traps;
}

/**
 * @brief Algorithms::queryTrapezoidalMap perform the point query
 * @return the pointer of the Leaf that store the trapezoid ID (return->getValue())
 * and set tmap.id_trapezoid_found for drawableTrapezoidalMap...
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
    tmap.id_trapezoid_found=node->getValue();
    return node;
}

/**
 * @brief Algorithms::oneTrapezoidIntersection Split a trapezoid in 4
 * the segment lies entirely in a trapezoid
 *
 * @param id_trap id of the trapezoid to split
 * TODO
 */
void oneTrapezoidIntersection(Trapezoid* intersected, TrapezoidalMap& tmap, Dag& dag, cg3::Segment2d segment) {
    size_t id_intersected=intersected->getLeafNode()->getValue();
    size_t idbuf;

    // add the 4 trapezoids in tmap
    size_t leftT=tmap.addTrapezoid(Trapezoid(intersected->getLeftPoint(), intersected->getTopSegment(), segment.p1(), intersected->getBottomSegment()));
    size_t topT=tmap.addTrapezoid(Trapezoid(segment.p1(), intersected->getTopSegment(), segment.p2(), segment));
    size_t rightT=tmap.addTrapezoid(Trapezoid(segment.p2(), intersected->getTopSegment(), intersected->getRightPoint(), intersected->getBottomSegment()));
    size_t bottomT=tmap.addTrapezoid(Trapezoid(segment.p1(), segment, segment.p2(), intersected->getBottomSegment()));

    // The neighbors of the new 4 traps (trapezoidi con spigoli verticali in comune)
    tmap.trapezoid(leftT).setNeighbors(intersected->getNeighbor(Trapezoid::TL), topT, bottomT, intersected->getNeighbor(Trapezoid::BL));
    tmap.trapezoid(topT).setNeighbors(leftT, rightT, SIZE_MAX, SIZE_MAX);
    tmap.trapezoid(rightT).setNeighbors(topT, intersected->getNeighbor(Trapezoid::TR), intersected->getNeighbor(Trapezoid::BR), bottomT);
    tmap.trapezoid(bottomT).setNeighbors(SIZE_MAX, SIZE_MAX, rightT, leftT);

    // Update the neighbors of the trapezoid to the left of id_intersected
    if (intersected->getNeighbor(Trapezoid::TL)!=SIZE_MAX) {
        idbuf=intersected->getNeighbor(Trapezoid::TL);
        // OKKIO: anche e solo se idbuf e leftT hanno lo stesso segmento superiore...
        if (tmap.trapezoid(idbuf).getNeighbor(Trapezoid::TR)==id_intersected) tmap.trapezoid(idbuf).setNeighbor(Trapezoid::TR,leftT);
        // OKKIO: anche e solo se idbuf e leftT hanno lo stesso segmento inferiore...
        if (tmap.trapezoid(idbuf).getNeighbor(Trapezoid::BR)==id_intersected) tmap.trapezoid(idbuf).setNeighbor(Trapezoid::BR,leftT);
    }
    if (intersected->getNeighbor(Trapezoid::BL)!=SIZE_MAX) {
        idbuf=intersected->getNeighbor(Trapezoid::BL);
        if (tmap.trapezoid(idbuf).getNeighbor(Trapezoid::TR)==id_intersected) tmap.trapezoid(idbuf).setNeighbor(Trapezoid::TR,leftT);
        if (tmap.trapezoid(idbuf).getNeighbor(Trapezoid::BR)==id_intersected) tmap.trapezoid(idbuf).setNeighbor(Trapezoid::BR,leftT);
    }
    // Update the neighbors of the trapezoid to the right of id_intersected
    if (intersected->getNeighbor(Trapezoid::TR)!=SIZE_MAX) {
        idbuf=intersected->getNeighbor(Trapezoid::TR);
        if (tmap.trapezoid(idbuf).getNeighbor(Trapezoid::TL)==id_intersected) tmap.trapezoid(idbuf).setNeighbor(Trapezoid::TL,rightT);
        if (tmap.trapezoid(idbuf).getNeighbor(Trapezoid::BL)==id_intersected) tmap.trapezoid(idbuf).setNeighbor(Trapezoid::BL,rightT);
    }
    if (intersected->getNeighbor(Trapezoid::BR)!=SIZE_MAX) {
        idbuf=intersected->getNeighbor(Trapezoid::BR);
        if (tmap.trapezoid(idbuf).getNeighbor(Trapezoid::TL)==id_intersected) tmap.trapezoid(idbuf).setNeighbor(Trapezoid::TL,rightT);
        if (tmap.trapezoid(idbuf).getNeighbor(Trapezoid::BL)==id_intersected) tmap.trapezoid(idbuf).setNeighbor(Trapezoid::BL,rightT);
    }

    // create the sub graph: +2 XNodes, +1 YNode, +4 Leafs, bottom to top...
    Node* bottomL=dag.addNode(Node(Node::LEAF,bottomT));
    tmap.trapezoid(bottomT).setLeafNode(bottomL);
    Node* topL=dag.addNode(Node(Node::LEAF,topT));
    tmap.trapezoid(topT).setLeafNode(topL);
    Node* s1N=dag.addNode(Node(Node::YNODE,tmap.addSegment(segment), topL, bottomL));
    Node* rightL=dag.addNode(Node(Node::LEAF,rightT));
    tmap.trapezoid(rightT).setLeafNode(rightL);
    Node* q1N=dag.addNode(Node(Node::XNODE,tmap.addPoint(segment.p2()), s1N, rightL));
    Node* leftL=dag.addNode(Node(Node::LEAF,leftT));
    tmap.trapezoid(leftT).setLeafNode(leftL);
    // link the new subgraph
    intersected->getLeafNode()->update(Node::XNODE,tmap.addPoint(segment.p1()),leftL, q1N);

    // delete the original trapezoid intersected
    tmap.deleteTrapezoid(id_intersected);
}

/**
 * @brief Algorithms::twoOrMoreTrapezoidsIntersection Split a trapezoid in 4
 * the segment lies entirely in a trapezoid
 *
 * @param id_trap id of the trapezoid to split
 * TODO
 */
void twoOrMoreTrapezoidsIntersection(std::vector<Trapezoid*> traps_intersect, TrapezoidalMap& tmap, Dag& dag, cg3::Segment2d segment) {
    size_t idbuf, newTopMerge, newBotMerge;

    if (traps_intersect.size()<2) return; // error, the point is not inside the bounding box, throw "MMM click on the bbox...";

    //=======================[ First trap intersected (leftmost) ]=======================
    size_t id_intersected=traps_intersect[0]->getLeafNode()->getValue();
    Trapezoid* intersected=traps_intersect[0]; // tmap.getTrapezoid(id_intersected);

    // add the 3 trapezoids in tmap: left top & bottom, top & bottom could be merged with next splitting traps
    size_t leftT=tmap.addTrapezoid(Trapezoid(intersected->getLeftPoint(), intersected->getTopSegment(), segment.p1(), intersected->getBottomSegment()));
    size_t topMerge=tmap.addTrapezoid(Trapezoid(segment.p1(), intersected->getTopSegment(), intersected->getRightPoint(), segment));
    size_t botMerge=tmap.addTrapezoid(Trapezoid(segment.p1(), segment, intersected->getRightPoint(), intersected->getBottomSegment()));

    // The neighbors of the new 3 traps
    tmap.trapezoid(leftT).setNeighbors(intersected->getNeighbor(Trapezoid::TL), topMerge, botMerge, intersected->getNeighbor(Trapezoid::BL));
    tmap.trapezoid(topMerge).setNeighbors(leftT, intersected->getNeighbor(Trapezoid::TR), SIZE_MAX/*TODO*/, SIZE_MAX);
    tmap.trapezoid(botMerge).setNeighbors(SIZE_MAX, SIZE_MAX /*TODO*/, intersected->getNeighbor(Trapezoid::BR), leftT);

    // Update the right neighbors of the trapezoid to the left of id_intersected
    if (intersected->getNeighbor(Trapezoid::TL)!=SIZE_MAX) {
        idbuf=intersected->getNeighbor(Trapezoid::TL);
        // OKKIO: anche e solo se idbuf e leftT hanno lo stesso segmento superiore...
        if (tmap.trapezoid(idbuf).getNeighbor(Trapezoid::TR)==id_intersected) tmap.trapezoid(idbuf).setNeighbor(Trapezoid::TR,leftT);
        // OKKIO: anche e solo se idbuf e leftT hanno lo stesso segmento inferiore...
        if (tmap.trapezoid(idbuf).getNeighbor(Trapezoid::BR)==id_intersected) tmap.trapezoid(idbuf).setNeighbor(Trapezoid::BR,leftT);
    }
    if (intersected->getNeighbor(Trapezoid::BL)!=SIZE_MAX) {
        idbuf=intersected->getNeighbor(Trapezoid::BL);
        if (tmap.trapezoid(idbuf).getNeighbor(Trapezoid::TR)==id_intersected) tmap.trapezoid(idbuf).setNeighbor(Trapezoid::TR,leftT);
        if (tmap.trapezoid(idbuf).getNeighbor(Trapezoid::BR)==id_intersected) tmap.trapezoid(idbuf).setNeighbor(Trapezoid::BR,leftT);
    }
    // Update the left neighbors of the trapezoid to the right of id_intersected
    if (intersected->getNeighbor(Trapezoid::TR)!=SIZE_MAX) {
        idbuf=intersected->getNeighbor(Trapezoid::TR);
        if (tmap.trapezoid(idbuf).getNeighbor(Trapezoid::TL)==id_intersected) tmap.trapezoid(idbuf).setNeighbor(Trapezoid::TL,topMerge/*TODO*/);
        if (tmap.trapezoid(idbuf).getNeighbor(Trapezoid::BL)==id_intersected) tmap.trapezoid(idbuf).setNeighbor(Trapezoid::BL,topMerge/*TODO*/); // mmmm
    }
    if (intersected->getNeighbor(Trapezoid::BR)!=SIZE_MAX) {
        idbuf=intersected->getNeighbor(Trapezoid::BR);
        if (tmap.trapezoid(idbuf).getNeighbor(Trapezoid::TL)==id_intersected) tmap.trapezoid(idbuf).setNeighbor(Trapezoid::TL,botMerge/*TODO*/);
        if (tmap.trapezoid(idbuf).getNeighbor(Trapezoid::BL)==id_intersected) tmap.trapezoid(idbuf).setNeighbor(Trapezoid::BL,botMerge/*TODO*/); // mmmm
    }

    // create the sub graph: +1 XNodes, +1 YNode, +3 Leafs, bottom to top...
    Node* botMergeNode=dag.addNode(Node(Node::LEAF,botMerge));
    tmap.trapezoid(botMerge).setLeafNode(botMergeNode);
    Node* topMergeNode=dag.addNode(Node(Node::LEAF,topMerge));
    tmap.trapezoid(topMerge).setLeafNode(topMergeNode);
    Node* s1N=dag.addNode(Node(Node::YNODE,tmap.addSegment(segment), topMergeNode, botMergeNode));
    Node* leftL=dag.addNode(Node(Node::LEAF,leftT));
    tmap.trapezoid(leftT).setLeafNode(leftL);
    // link the new subgraph
    intersected->getLeafNode()->update(Node::XNODE,tmap.addPoint(segment.p1()),leftL, s1N);

    // delete the original trapezoid intersected, will be recycled
    tmap.deleteTrapezoid(id_intersected);

    // Now tmap & dag are consistent, only
    // - intersected->getNeighbor(Trapezoid::TR)->neighbor(TL & BL) and
    // - intersected->getNeighbor(Trapezoid::BR)->neighbor(TL & BL) need to be updated based on mergers

    //=======================[ Internal trapezoids intersected ]=======================
    for (size_t id=1; id<traps_intersect.size()-1; id++) {
        id_intersected=traps_intersect[id]->getLeafNode()->getValue();
        intersected=traps_intersect[id]; // tmap.getTrapezoid(id_intersected);

        newTopMerge=SIZE_MAX;
        newBotMerge=SIZE_MAX;

        // Add 2 new traps, top & bot, top & bot could be merged with previus top & bot traps (topMerge & botMerge)
        // new top trap: (intersected.p1, intersected.topseg, intersected.p2, segment)
        // new top trap merge with topMerge?
        if ((tmap.trapezoid(topMerge).getTopSegment()==intersected->getTopSegment()) && (tmap.trapezoid(topMerge).getBottomSegment()==segment)) {
            // merge, no new trap in tmap
            tmap.trapezoid(topMerge).setRightPoint(intersected->getRightPoint());
            tmap.trapezoid(topMerge).setNeighbor(Trapezoid::TR,intersected->getNeighbor(Trapezoid::TR));
            // topMergeNode remains OK, contains topMerge
            /*TODO tmap.trapezoid(topMerge).setNeighbor(Trapezoid::BR) */
        } else {
            // no merge, "close" topMerge and add 1 new trap in tmap
            newTopMerge=tmap.addTrapezoid(Trapezoid(intersected->getLeftPoint(), intersected->getTopSegment(), intersected->getRightPoint(), segment));
            // neighbors of the new top trap
            tmap.trapezoid(newTopMerge).setNeighbors(intersected->getNeighbor(Trapezoid::TL), intersected->getNeighbor(Trapezoid::TR), SIZE_MAX/*TODO*/, topMerge);
            // close old top merge (set neighbors)
            tmap.trapezoid(topMerge).setNeighbor(Trapezoid::TR,SIZE_MAX);
            tmap.trapezoid(topMerge).setNeighbor(Trapezoid::BR,newTopMerge);

            // no merge => I need a new leaft to store the new trap
            topMergeNode=dag.addNode(Node(Node::LEAF,newTopMerge));
            tmap.trapezoid(newTopMerge).setLeafNode(topMergeNode);
            topMerge=newTopMerge; // mah!
        }

        // new bot trap merge with botMerge?
        if ((tmap.trapezoid(botMerge).getTopSegment()==segment) && (tmap.trapezoid(botMerge).getBottomSegment()==intersected->getBottomSegment())) {
            // merge, no new trap in tmap
            tmap.trapezoid(botMerge).setRightPoint(intersected->getRightPoint());
            tmap.trapezoid(botMerge).setNeighbor(Trapezoid::BR,intersected->getNeighbor(Trapezoid::BR));
            // botMergeNode remains OK, contains botMerge
            /*TODO tmap.trapezoid(botMerge).setNeighbor(Trapezoid::BR) */
        } else {
            // no merge, "close" botMerge and add 1 new trap in tmap
            newBotMerge=tmap.addTrapezoid(Trapezoid(intersected->getLeftPoint(), segment, intersected->getRightPoint(), intersected->getBottomSegment()));
            // neighbors of the new top trap
            tmap.trapezoid(newBotMerge).setNeighbors(botMerge, SIZE_MAX/*TODO*/, intersected->getNeighbor(Trapezoid::BR), intersected->getNeighbor(Trapezoid::BL));
            // close old bot merge (set neighbors)
            tmap.trapezoid(botMerge).setNeighbor(Trapezoid::TR,newBotMerge);
            tmap.trapezoid(botMerge).setNeighbor(Trapezoid::BR,SIZE_MAX);

            // no merge => I need a new leaft to store the new trap
            botMergeNode=dag.addNode(Node(Node::LEAF,newBotMerge));
            tmap.trapezoid(newBotMerge).setLeafNode(botMergeNode);
            botMerge=newBotMerge; // mah!
            //**** TODO OKKIO ma restano trapezi o nodi non linkati???? intersected?????
        }

        // link the new subgraph, in any case i need a new segment node, this sub graph replace then leaf node
        intersected->getLeafNode()->update(Node::YNODE,s1N->getValue(), topMergeNode, botMergeNode);

        // delete the original trapezoid intersected
        tmap.deleteTrapezoid(id_intersected);
    }

    //=======================[ Last trap intersected (rightmost) ]=======================
    id_intersected=traps_intersect.back()->getLeafNode()->getValue();
    intersected=traps_intersect.back(); // tmap.getTrapezoid(id_intersected);

    newTopMerge=SIZE_MAX;
    newBotMerge=SIZE_MAX;

    // new top trap merge with topMerge?
    if ((tmap.trapezoid(topMerge).getTopSegment()==intersected->getTopSegment()) && (tmap.trapezoid(topMerge).getBottomSegment()==segment)) {
        // merge, no new trap in tmap
        tmap.trapezoid(topMerge).setRightPoint(segment.p2());
        tmap.trapezoid(topMerge).setNeighbor(Trapezoid::TR,SIZE_MAX); /*TODO: THE RIGHTMOST TRAP */
        tmap.trapezoid(topMerge).setNeighbor(Trapezoid::BR,SIZE_MAX);
        // topMergeNode remains OK, contains topMerge
    } else {
        // no merge, "close" topMerge and add 1 new trap in tmap
        newTopMerge=tmap.addTrapezoid(Trapezoid(intersected->getLeftPoint(), intersected->getTopSegment(), segment.p2(), segment));
        // neighbors of the new top trap
        tmap.trapezoid(newTopMerge).setNeighbors(intersected->getNeighbor(Trapezoid::TL), SIZE_MAX/*TODO:THE RIGHTMOST TRAP*/, SIZE_MAX, topMerge);
        // close old top merge (set neighbors)
        tmap.trapezoid(topMerge).setNeighbor(Trapezoid::TR,SIZE_MAX);
        tmap.trapezoid(topMerge).setNeighbor(Trapezoid::BR,newTopMerge);

        // no merge => I need a new leaft to store the new trap
        topMergeNode=dag.addNode(Node(Node::LEAF,newTopMerge));
        tmap.trapezoid(newTopMerge).setLeafNode(topMergeNode);
        topMerge=newTopMerge; // mah!
    }

    // new bot trap merge with botMerge?
    if ((tmap.trapezoid(botMerge).getTopSegment()==segment) && (tmap.trapezoid(botMerge).getBottomSegment()==intersected->getBottomSegment())) {
        // merge, no new trap in tmap
        tmap.trapezoid(botMerge).setRightPoint(segment.p2());
        tmap.trapezoid(botMerge).setNeighbor(Trapezoid::TR,SIZE_MAX);
        tmap.trapezoid(botMerge).setNeighbor(Trapezoid::BR,SIZE_MAX); /*TODO: THE RIGHTMOST TRAP */
        // botMergeNode remains OK, contains botMerge
    } else {
        // no merge, "close" botMerge and add 1 new trap in tmap
        newBotMerge=tmap.addTrapezoid(Trapezoid(intersected->getLeftPoint(), segment, segment.p2(), intersected->getBottomSegment()));
        // neighbors of the new top trap
        tmap.trapezoid(newBotMerge).setNeighbors(botMerge, SIZE_MAX, SIZE_MAX/*TODO:THE RIGHTMOST TRAP*/, intersected->getNeighbor(Trapezoid::BL));
        // close old bot merge (set neighbors)
        tmap.trapezoid(botMerge).setNeighbor(Trapezoid::TR,newBotMerge);
        tmap.trapezoid(botMerge).setNeighbor(Trapezoid::BR,SIZE_MAX);

        // no merge => i need a new leaft to store the new trap
        botMergeNode=dag.addNode(Node(Node::LEAF,newBotMerge));
        tmap.trapezoid(newBotMerge).setLeafNode(botMergeNode);
        botMerge=newBotMerge; // mah!
        //**** TODO OKKIO ma restano trapezi o nodi non linkati???? intersected?????
    }

    // the new rightmost trpezoid
    size_t rightT=tmap.addTrapezoid(Trapezoid(segment.p2(), intersected->getTopSegment(), intersected->getRightPoint(), intersected->getBottomSegment()));
    // The neighbors
    tmap.trapezoid(rightT).setNeighbors(topMerge, intersected->getNeighbor(Trapezoid::TR), intersected->getNeighbor(Trapezoid::BR), botMerge);
    // close left traps
    tmap.trapezoid(topMerge).setNeighbor(Trapezoid::TR,rightT);
    tmap.trapezoid(botMerge).setNeighbor(Trapezoid::BR,rightT);

    // Update the neighbors of the trapezoid to the right of id_intersected
    if (intersected->getNeighbor(Trapezoid::TR)!=SIZE_MAX) {
        idbuf=intersected->getNeighbor(Trapezoid::TR);
        if (tmap.trapezoid(idbuf).getNeighbor(Trapezoid::TL)==id_intersected) tmap.trapezoid(idbuf).setNeighbor(Trapezoid::TL,rightT);
        if (tmap.trapezoid(idbuf).getNeighbor(Trapezoid::BL)==id_intersected) tmap.trapezoid(idbuf).setNeighbor(Trapezoid::BL,rightT);
    }
    if (intersected->getNeighbor(Trapezoid::BR)!=SIZE_MAX) {
        idbuf=intersected->getNeighbor(Trapezoid::BR);
        if (tmap.trapezoid(idbuf).getNeighbor(Trapezoid::TL)==id_intersected) tmap.trapezoid(idbuf).setNeighbor(Trapezoid::TL,rightT);
        if (tmap.trapezoid(idbuf).getNeighbor(Trapezoid::BL)==id_intersected) tmap.trapezoid(idbuf).setNeighbor(Trapezoid::BL,rightT);
    }

    // create and link the new subgraph
    Node* rightL=dag.addNode(Node(Node::LEAF,rightT));
    tmap.trapezoid(rightT).setLeafNode(rightL);
    Node* s2N=dag.addNode(Node(Node::YNODE,s1N->getValue(), topMergeNode, botMergeNode));
    intersected->getLeafNode()->update(Node::XNODE,tmap.addPoint(segment.p2()), s2N, rightL);

    // delete the original trapezoid intersected
    tmap.deleteTrapezoid(id_intersected);
}

}
