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
    // clear id_trapezoid_found for the point query
    tmap.id_trapezoid_found=SIZE_MAX;
}

std::vector<Trapezoid*> FollowSegment(TrapezoidalMap& tmap, Dag& dag, cg3::Segment2d segment) {
    std::vector<Trapezoid*> traps;
    cg3::Point2d sec_point=segment.p2();
    Node* leaf=queryTrapezoidalMap(segment.p1(), &sec_point, tmap, dag);
    Trapezoid* currtrap=tmap.getTrapezoid(leaf->getValue());
    traps.push_back(currtrap);

    // mentre ne interseco ancora almeno un altro a destra di currtrap
    while (segment.p2().x()>currtrap->getRightPoint().x()) {
        // quale prendere TRnieg o BRneig?
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
    }
    return traps;
}

/**
 * @brief Algorithms::queryTrapezoidalMap perform the point query
 * @return the pointer of the Leaf that store the trapezoid ID (return->getValue())
 * and set tmap.id_trapezoid_found for drawableTrapezoidalMap.
 *
 * @param sec_point used only when inserting a new segment, used to solve the problem of degenerate points
 * in the simple query phase it is not used (since there is no second point, it is superfluous to solve the ambiguity)
 */
Node* queryTrapezoidalMap(cg3::Point2d point, cg3::Point2d* sec_point, TrapezoidalMap& tmap, Dag& dag) {
    Node* node=dag.getRootNode();
    while (node->getType()!=Node::LEAF) {
        if (node->getType()==Node::XNODE) { // X node
            if (point.x()<tmap.point(node->getValue()).x())
                node=node->getLeftChild();
            else
                node=node->getRightChild();
        } else { // Segment node
            size_t id_seg=node->getValue();
            // if insertion phase and degenerate point=segment.p1 => I use the second point of the segment to insert
            if ((sec_point!=nullptr) && areEqual(point, tmap.segment(id_seg).p1())) {
                double d=(sec_point->x()-tmap.segment(id_seg).p1().x()) * (tmap.segment(id_seg).p2().y()-tmap.segment(id_seg).p1().y()) -
                        (sec_point->y()-tmap.segment(id_seg).p1().y()) * (tmap.segment(id_seg).p2().x()-tmap.segment(id_seg).p1().x()); // d=(x−x1)(y2−y1)−(y−y1)(x2−x1)
                if (d>0 || areEqual(d,0))
                    node=node->getRightChild();
                else
                    node=node->getLeftChild();
            } else { // normal case point<>segment.p1 or without second point
                double d=(point.x()-tmap.segment(id_seg).p1().x()) * (tmap.segment(id_seg).p2().y()-tmap.segment(id_seg).p1().y()) -
                        (point.y()-tmap.segment(id_seg).p1().y()) * (tmap.segment(id_seg).p2().x()-tmap.segment(id_seg).p1().x()); // d=(x−x1)(y2−y1)−(y−y1)(x2−x1)
                if (d>0 || areEqual(d,0))
                    node=node->getRightChild();
                else
                    node=node->getLeftChild();
            }
        }
    }
    tmap.id_trapezoid_found=node->getValue();
    return node;
}

/**
 * @brief Algorithms::setNeighborsOfNewTrapezoids set the neighbors of the new trapezoids inserted in TMap, this is used
 * in all cases: not degen, degen, 2, 3 or 4 traps, one method for all. I've used the shrot conditional operator, for
 * the (similar) "if else.." version see Algorithms::updateNeighborsOfNeighbors
 */
void setNeighborsOfNewTrapezoids(size_t leftT, size_t topT, size_t rightT, size_t bottomT, Trapezoid* intersected, TrapezoidalMap& tmap, cg3::Segment2d segment) {
    // if left trap exists
    if (leftT!=SIZE_MAX)
        tmap.trapezoid(leftT).setNeighbors(intersected->getNeighbor(Trapezoid::TL), topT, bottomT, intersected->getNeighbor(Trapezoid::BL));
    // top trap always exists, but the neighbors depend on the existence of the left & right trap and if top or bot tra are tringles
    tmap.trapezoid(topT).setNeighbors(
                (areEqual(segment.p1().x(),intersected->getLeftPoint().x()) && areEqual(segment.p1(),intersected->getTopSegment().p1())) ? SIZE_MAX : (areEqual(segment.p1().x(),intersected->getLeftPoint().x()) ? intersected->getNeighbor(Trapezoid::TL) : leftT),
                (areEqual(segment.p2().x(),intersected->getRightPoint().x()) && areEqual(segment.p2(),intersected->getTopSegment().p2())) ? SIZE_MAX : (areEqual(segment.p2().x(),intersected->getRightPoint().x()) ? intersected->getNeighbor(Trapezoid::TR) : rightT),
                SIZE_MAX,
                SIZE_MAX);
    // if right trap exists
    if (rightT!=SIZE_MAX)
        tmap.trapezoid(rightT).setNeighbors(topT, intersected->getNeighbor(Trapezoid::TR), intersected->getNeighbor(Trapezoid::BR), bottomT);
    // bottom trap always exists, but the neighbors depend on the existence of the left & right trap and if top or bot tra are tringles
    tmap.trapezoid(bottomT).setNeighbors(
                SIZE_MAX,
                SIZE_MAX,
                (areEqual(segment.p2().x(),intersected->getRightPoint().x()) && areEqual(segment.p2(),intersected->getTopSegment().p2())) ? SIZE_MAX : (areEqual(segment.p2().x(),intersected->getRightPoint().x()) ? intersected->getNeighbor(Trapezoid::BR) : rightT),
                (areEqual(segment.p1().x(),intersected->getLeftPoint().x()) && areEqual(segment.p1(),intersected->getTopSegment().p1())) ? SIZE_MAX : (areEqual(segment.p1().x(),intersected->getLeftPoint().x()) ? intersected->getNeighbor(Trapezoid::BL) : leftT));
}

/**
 * @brief Algorithms::updateNeighborsOfNeighbors update the neighbors of the neighbors of the new trapezoids inserted in
 * TMap, this is used in all cases: not degen, degen, 2, 3 or 4 traps, one method for all.
 */
void updateNeighborsOfNeighbors(size_t leftT, size_t topT, size_t rightT, size_t bottomT, Trapezoid* intersected, TrapezoidalMap& tmap, cg3::Segment2d segment) {
    size_t id_intersected=intersected->getLeafNode()->getValue();
    size_t idbuf;

    // Update the neighbors of the trapezoid to the top left of id_intersected
    if (intersected->getNeighbor(Trapezoid::TL)!=SIZE_MAX) {
        idbuf=intersected->getNeighbor(Trapezoid::TL);
        // if share the same segment, this must be updated with new one (if exixts)
        if (tmap.trapezoid(idbuf).getNeighbor(Trapezoid::TR)==id_intersected) {
            if (leftT!=SIZE_MAX) // if exists this is the correct right neighb.
                tmap.trapezoid(idbuf).setNeighbor(Trapezoid::TR,leftT);
            else
                if (areEqual(segment.p1().x(),intersected->getLeftPoint().x()) && areEqual(segment.p1(),intersected->getTopSegment().p1())) // if triangle => no right neighbor
                    tmap.trapezoid(idbuf).setNeighbor(Trapezoid::TR,SIZE_MAX);
                else
                    tmap.trapezoid(idbuf).setNeighbor(Trapezoid::TR,topT); // if no left and no triangle => top right neighbor is top trap...
        }
    }
    // Update the neighbors of the trapezoid to the bot left of id_intersected
    if (intersected->getNeighbor(Trapezoid::BL)!=SIZE_MAX) {
        idbuf=intersected->getNeighbor(Trapezoid::BL);
        // if share the same segment, this must be updated with new one (if exixts)
        if (tmap.trapezoid(idbuf).getNeighbor(Trapezoid::BR)==id_intersected) {
            if (leftT!=SIZE_MAX) // if exists this is the correct right neighb.
                tmap.trapezoid(idbuf).setNeighbor(Trapezoid::BR,leftT);
            else
                if (areEqual(segment.p1().x(),intersected->getLeftPoint().x()) && areEqual(segment.p1(),intersected->getBottomSegment().p1())) // if triangle => no right neighbor
                    tmap.trapezoid(idbuf).setNeighbor(Trapezoid::BR,SIZE_MAX);
                else
                    tmap.trapezoid(idbuf).setNeighbor(Trapezoid::BR,bottomT); // if no left and no triangle => bot right neighbor is bot trap...
        }
    }
    // Update the neighbors of the trapezoid to the top right of id_intersected
    if (intersected->getNeighbor(Trapezoid::TR)!=SIZE_MAX) {
        idbuf=intersected->getNeighbor(Trapezoid::TR);
        // if share the same segment, this must be updated with new one (if exixts)
        if (tmap.trapezoid(idbuf).getNeighbor(Trapezoid::TL)==id_intersected) {
            if (rightT!=SIZE_MAX) // if exists this is the correct left neighb.
                tmap.trapezoid(idbuf).setNeighbor(Trapezoid::TL,rightT);
            else
                if (areEqual(segment.p2().x(),intersected->getRightPoint().x()) && areEqual(segment.p2(),intersected->getTopSegment().p2())) // if triangle => no right neighbor
                    tmap.trapezoid(idbuf).setNeighbor(Trapezoid::TL,SIZE_MAX);
                else
                    tmap.trapezoid(idbuf).setNeighbor(Trapezoid::TL,topT); // if no right and no triangle => top left neighbor is top trap...
        }
    }
    // Update the neighbors of the trapezoid to the bot right of id_intersected
    if (intersected->getNeighbor(Trapezoid::BR)!=SIZE_MAX) {
        idbuf=intersected->getNeighbor(Trapezoid::BR);
        // if share the same segment, this must be updated with new one (if exixts)
        if (tmap.trapezoid(idbuf).getNeighbor(Trapezoid::BL)==id_intersected) {
            if (rightT!=SIZE_MAX) // if exists this is the correct left neighb.
                tmap.trapezoid(idbuf).setNeighbor(Trapezoid::BL,rightT);
            else
                if (areEqual(segment.p2().x(),intersected->getRightPoint().x()) && areEqual(segment.p2(),intersected->getTopSegment().p2())) // if triangle => no right neighbor
                    tmap.trapezoid(idbuf).setNeighbor(Trapezoid::BL,SIZE_MAX);
                else
                    tmap.trapezoid(idbuf).setNeighbor(Trapezoid::BL,bottomT); // if no right and no triangle => bot left neighbor is bot trap...
        }
    }
}

/**
 * @brief Algorithms::updateDag update the dag creating and adding the new subgraph for all cases: not degen,
 * degen, 2, 3 or 4 traps.
 */
void updateDag(size_t leftT, size_t topT, size_t rightT, size_t bottomT, Trapezoid* intersected, TrapezoidalMap& tmap, Dag& dag, cg3::Segment2d segment) {
    Node* bottomL=dag.addNode(Node(Node::LEAF,bottomT));
    tmap.trapezoid(bottomT).setLeafNode(bottomL);
    Node* topL=dag.addNode(Node(Node::LEAF,topT));
    tmap.trapezoid(topT).setLeafNode(topL);
    if ((rightT!=SIZE_MAX) && (leftT!=SIZE_MAX)) { // with left and right trap
        Node* s1N=dag.addNode(Node(Node::YNODE,tmap.addSegment(segment), topL, bottomL));
        Node* rightL=dag.addNode(Node(Node::LEAF,rightT));
        tmap.trapezoid(rightT).setLeafNode(rightL);
        Node* q1N=dag.addNode(Node(Node::XNODE,tmap.addPoint(segment.p2()), s1N, rightL));
        Node* leftL=dag.addNode(Node(Node::LEAF,leftT));
        tmap.trapezoid(leftT).setLeafNode(leftL);
        // link the new subgraph
        intersected->getLeafNode()->update(Node::XNODE,tmap.addPoint(segment.p1()),leftL, q1N);
    } else
        if (rightT!=SIZE_MAX) { // without left trap
            Node* s1N=dag.addNode(Node(Node::YNODE,tmap.addSegment(segment), topL, bottomL));
            Node* rightL=dag.addNode(Node(Node::LEAF,rightT));
            tmap.trapezoid(rightT).setLeafNode(rightL);
            // link the new subgraph
            intersected->getLeafNode()->update(Node::XNODE,tmap.addPoint(segment.p2()), s1N, rightL);
        } else
            if (leftT!=SIZE_MAX) { // without right trap
                Node* s1N=dag.addNode(Node(Node::YNODE,tmap.addSegment(segment), topL, bottomL));
                Node* leftL=dag.addNode(Node(Node::LEAF,leftT));
                tmap.trapezoid(leftT).setLeafNode(leftL);
                // link the new subgraph
                intersected->getLeafNode()->update(Node::XNODE,tmap.addPoint(segment.p1()),leftL, s1N);
            } else // only top and bot
                intersected->getLeafNode()->update(Node::YNODE,tmap.addSegment(segment), topL, bottomL);
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

    // left trap only if the segment.p1.x > leftP.x of the trap
    size_t leftT=SIZE_MAX;
    if (segment.p1().x()>intersected->getLeftPoint().x())
        leftT=tmap.addTrapezoid(Trapezoid(intersected->getLeftPoint(), intersected->getTopSegment(), segment.p1(), intersected->getBottomSegment()));
    // top always exists
    size_t topT=tmap.addTrapezoid(Trapezoid(segment.p1(), intersected->getTopSegment(), segment.p2(), segment));
    // right trap only if the segment.p2.x < rightP.x of the trap
    size_t rightT=SIZE_MAX;
    if (segment.p2().x()<intersected->getRightPoint().x())
        rightT=tmap.addTrapezoid(Trapezoid(segment.p2(), intersected->getTopSegment(), intersected->getRightPoint(), intersected->getBottomSegment()));
    // bottom always exists
    size_t bottomT=tmap.addTrapezoid(Trapezoid(segment.p1(), segment, segment.p2(), intersected->getBottomSegment()));

    setNeighborsOfNewTrapezoids(leftT, topT, rightT, bottomT, intersected, tmap, segment);

    updateNeighborsOfNeighbors(leftT, topT, rightT, bottomT, intersected, tmap, segment);

    updateDag(leftT, topT, rightT, bottomT, intersected, tmap, dag, segment);

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
    Trapezoid* intersected=traps_intersect[0];
    size_t id_intersected=intersected->getLeafNode()->getValue();

    // left trap only if the segment.p1.x > leftP.x of the trap
    size_t leftT=SIZE_MAX;
    if (segment.p1().x()>intersected->getLeftPoint().x())
        leftT=tmap.addTrapezoid(Trapezoid(intersected->getLeftPoint(), intersected->getTopSegment(), segment.p1(), intersected->getBottomSegment()));
    // top always exists
    size_t topMerge=tmap.addTrapezoid(Trapezoid(segment.p1(), intersected->getTopSegment(), intersected->getRightPoint(), segment));
    // bottom always exists
    size_t botMerge=tmap.addTrapezoid(Trapezoid(segment.p1(), segment, intersected->getRightPoint(), intersected->getBottomSegment()));

    setNeighborsOfNewTrapezoids(leftT, topMerge, SIZE_MAX, botMerge, intersected, tmap, segment);
/*
    tmap.trapezoid(topMerge).setNeighbor(Trapezoid::BR,SIZE_MAX);
    tmap.trapezoid(botMerge).setNeighbor(Trapezoid::TR,SIZE_MAX);


    // add the 3 trapezoids in tmap: left top & bottom, top & bottom could be merged with next splitting traps
    size_t leftT=tmap.addTrapezoid(Trapezoid(intersected->getLeftPoint(), intersected->getTopSegment(), segment.p1(), intersected->getBottomSegment()));
    size_t topMerge=tmap.addTrapezoid(Trapezoid(segment.p1(), intersected->getTopSegment(), intersected->getRightPoint(), segment));
    size_t botMerge=tmap.addTrapezoid(Trapezoid(segment.p1(), segment, intersected->getRightPoint(), intersected->getBottomSegment()));

    // The neighbors of the new 3 traps
    tmap.trapezoid(leftT).setNeighbors(intersected->getNeighbor(Trapezoid::TL), topMerge, botMerge, intersected->getNeighbor(Trapezoid::BL));
    tmap.trapezoid(topMerge).setNeighbors(leftT, intersected->getNeighbor(Trapezoid::TR), SIZE_MAX TODO, SIZE_MAX);
    tmap.trapezoid(botMerge).setNeighbors(SIZE_MAX, SIZE_MAX TODO, intersected->getNeighbor(Trapezoid::BR), leftT);
*/

    updateNeighborsOfNeighbors(leftT, topMerge, SIZE_MAX, botMerge, intersected, tmap, segment);
/*
    // Update the right neighbors of the trapezoid to the left of id_intersected, anche e solo se idbuf e leftT hanno lo stesso segmento superiore/inf...
    if (intersected->getNeighbor(Trapezoid::TL)!=SIZE_MAX) {
        idbuf=intersected->getNeighbor(Trapezoid::TL);
        if (tmap.trapezoid(idbuf).getNeighbor(Trapezoid::TR)==id_intersected) tmap.trapezoid(idbuf).setNeighbor(Trapezoid::TR,leftT);
    }
    if (intersected->getNeighbor(Trapezoid::BL)!=SIZE_MAX) {
        idbuf=intersected->getNeighbor(Trapezoid::BL);
        if (tmap.trapezoid(idbuf).getNeighbor(Trapezoid::BR)==id_intersected) tmap.trapezoid(idbuf).setNeighbor(Trapezoid::BR,leftT);
    }
    // Update the left neighbors of the trapezoid to the right of id_intersected
    if (intersected->getNeighbor(Trapezoid::TR)!=SIZE_MAX) {
        idbuf=intersected->getNeighbor(Trapezoid::TR);
        if (tmap.trapezoid(idbuf).getNeighbor(Trapezoid::TL)==id_intersected) tmap.trapezoid(idbuf).setNeighbor(Trapezoid::TL,topMerge);
    }
    if (intersected->getNeighbor(Trapezoid::BR)!=SIZE_MAX) {
        idbuf=intersected->getNeighbor(Trapezoid::BR);
        if (tmap.trapezoid(idbuf).getNeighbor(Trapezoid::BL)==id_intersected) tmap.trapezoid(idbuf).setNeighbor(Trapezoid::BL,botMerge);
    }
*/
/*
    updateDag(leftT, topMerge, SIZE_MAX, botMerge, intersected, tmap, dag, segment);
    Node* topMergeNode=tmap.trapezoid(topMerge).getLeafNode();
    Node* botMergeNode=tmap.trapezoid(botMerge).getLeafNode();
*/

    Node* botMergeNode=dag.addNode(Node(Node::LEAF,botMerge));
    tmap.trapezoid(botMerge).setLeafNode(botMergeNode);
    Node* topMergeNode=dag.addNode(Node(Node::LEAF,topMerge));
    tmap.trapezoid(topMerge).setLeafNode(topMergeNode);
    size_t id_segment=tmap.addSegment(segment);
    if (leftT!=SIZE_MAX) { // with left trap?
        Node* s1N=dag.addNode(Node(Node::YNODE,id_segment, topMergeNode, botMergeNode));
        Node* leftL=dag.addNode(Node(Node::LEAF,leftT));
        tmap.trapezoid(leftT).setLeafNode(leftL);
        // link the new subgraph
        intersected->getLeafNode()->update(Node::XNODE,tmap.addPoint(segment.p1()),leftL, s1N);
    } else // only top and bot
        intersected->getLeafNode()->update(Node::YNODE,id_segment, topMergeNode, botMergeNode);

/*
    // the new sub graph: +1 XNodes, +1 YNode, +3 Leafs, bottom to top...
    Node* botMergeNode=dag.addNode(Node(Node::LEAF,botMerge));
    tmap.trapezoid(botMerge).setLeafNode(botMergeNode);
    Node* topMergeNode=dag.addNode(Node(Node::LEAF,topMerge));
    tmap.trapezoid(topMerge).setLeafNode(topMergeNode);
    Node* s1N=dag.addNode(Node(Node::YNODE,tmap.addSegment(segment), topMergeNode, botMergeNode));
    Node* leftL=dag.addNode(Node(Node::LEAF,leftT));
    tmap.trapezoid(leftT).setLeafNode(leftL);
    // link the new subgraph
    intersected->getLeafNode()->update(Node::XNODE,tmap.addPoint(segment.p1()),leftL, s1N);
*/
    // delete the original trapezoid intersected, will be recycled
    tmap.deleteTrapezoid(id_intersected);

    // Now tmap & dag are consistent, only
    // - topMerge.neighbor(BR) and
    // - botMerge.neighbor(TR)
    // need to be updated based on mergers

    //=======================[ Internal trapezoids intersected ]=======================
    for (size_t id=1; id<traps_intersect.size()-1; id++) {
        intersected=traps_intersect[id];
        id_intersected=intersected->getLeafNode()->getValue();

        newTopMerge=SIZE_MAX;
        newBotMerge=SIZE_MAX;

        // Add 2 new traps, top & bot, top & bot could be merged with previus top & bot traps (topMerge & botMerge)
        // new top trap: (intersected.p1, intersected.topseg, intersected.p2, segment)
        // new top trap merge with topMerge?
        if (areEqual(tmap.trapezoid(topMerge).getTopSegment(),intersected->getTopSegment()) && areEqual(tmap.trapezoid(topMerge).getBottomSegment(),segment)) {
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
            // tmap.trapezoid(topMerge).setNeighbor(Trapezoid::TR,SIZE_MAX);  // FORSE NOOOO!!!!
            tmap.trapezoid(topMerge).setNeighbor(Trapezoid::BR,newTopMerge);

            if (intersected->getNeighbor(Trapezoid::TL)!=SIZE_MAX) {
                idbuf=intersected->getNeighbor(Trapezoid::TL);
                if (tmap.trapezoid(idbuf).getNeighbor(Trapezoid::TR)==id_intersected) tmap.trapezoid(idbuf).setNeighbor(Trapezoid::TR,newTopMerge);
            }

            // no merge => I need a new leaf to store the new trap
            topMergeNode=dag.addNode(Node(Node::LEAF,newTopMerge));
            tmap.trapezoid(newTopMerge).setLeafNode(topMergeNode);
            topMerge=newTopMerge; // mah!
        }

        // new bot trap merge with botMerge?
        if (areEqual(tmap.trapezoid(botMerge).getTopSegment(),segment) && areEqual(tmap.trapezoid(botMerge).getBottomSegment(),intersected->getBottomSegment())) {
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
            // tmap.trapezoid(botMerge).setNeighbor(Trapezoid::BR,SIZE_MAX); // FORSE NOOOO!!!!

            if (intersected->getNeighbor(Trapezoid::BL)!=SIZE_MAX) {
                idbuf=intersected->getNeighbor(Trapezoid::BL);
                if (tmap.trapezoid(idbuf).getNeighbor(Trapezoid::BR)==id_intersected) tmap.trapezoid(idbuf).setNeighbor(Trapezoid::BR,newBotMerge);
            }

            // no merge => I need a new leaft to store the new trap
            botMergeNode=dag.addNode(Node(Node::LEAF,newBotMerge));
            tmap.trapezoid(newBotMerge).setLeafNode(botMergeNode);
            botMerge=newBotMerge; // mah!
            //**** TODO OKKIO ma restano trapezi o nodi non linkati???? intersected?????
        }

        // link the new subgraph, I need a new segment node, this sub graph replace the leaf node
        intersected->getLeafNode()->update(Node::YNODE,id_segment, topMergeNode, botMergeNode);

        // delete the original trapezoid intersected
        tmap.deleteTrapezoid(id_intersected);
    }

    //=======================[ Last trap intersected (rightmost) ]=======================
    intersected=traps_intersect.back();
    id_intersected=intersected->getLeafNode()->getValue();
    newTopMerge=SIZE_MAX;
    newBotMerge=SIZE_MAX;

    // new top trap merge with topMerge?
    if (areEqual(tmap.trapezoid(topMerge).getTopSegment(),intersected->getTopSegment()) && areEqual(tmap.trapezoid(topMerge).getBottomSegment(),segment)) {
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
        // tmap.trapezoid(topMerge).setNeighbor(Trapezoid::TR,SIZE_MAX); // NOOOO!!!
        tmap.trapezoid(topMerge).setNeighbor(Trapezoid::BR,newTopMerge);

        if (intersected->getNeighbor(Trapezoid::TL)!=SIZE_MAX) {
            idbuf=intersected->getNeighbor(Trapezoid::TL);
            if (tmap.trapezoid(idbuf).getNeighbor(Trapezoid::TR)==id_intersected) tmap.trapezoid(idbuf).setNeighbor(Trapezoid::TR,newTopMerge);
        }

        // no merge => I need a new leaft to store the new trap
        topMergeNode=dag.addNode(Node(Node::LEAF,newTopMerge));
        tmap.trapezoid(newTopMerge).setLeafNode(topMergeNode);
        topMerge=newTopMerge; // mah!
    }

    // new bot trap merge with botMerge?
    if (areEqual(tmap.trapezoid(botMerge).getTopSegment(),segment) && areEqual(tmap.trapezoid(botMerge).getBottomSegment(),intersected->getBottomSegment())) {
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
        // tmap.trapezoid(botMerge).setNeighbor(Trapezoid::BR,SIZE_MAX); // NOOOO!!!

        if (intersected->getNeighbor(Trapezoid::BL)!=SIZE_MAX) {
            idbuf=intersected->getNeighbor(Trapezoid::BL);
            if (tmap.trapezoid(idbuf).getNeighbor(Trapezoid::BR)==id_intersected) tmap.trapezoid(idbuf).setNeighbor(Trapezoid::BR,newBotMerge);
        }

        // no merge => i need a new leaft to store the new trap
        botMergeNode=dag.addNode(Node(Node::LEAF,newBotMerge));
        tmap.trapezoid(newBotMerge).setLeafNode(botMergeNode);
        botMerge=newBotMerge; // mah!
        //**** TODO OKKIO ma restano trapezi o nodi non linkati???? intersected?????
    }

    // the new rightmost trpezoid
    size_t rightT=SIZE_MAX;
    if (segment.p2().x()<intersected->getRightPoint().x())
        rightT=tmap.addTrapezoid(Trapezoid(segment.p2(), intersected->getTopSegment(), intersected->getRightPoint(), intersected->getBottomSegment()));

    // The neighbors
    if (rightT!=SIZE_MAX)
        tmap.trapezoid(rightT).setNeighbors(topMerge, intersected->getNeighbor(Trapezoid::TR), intersected->getNeighbor(Trapezoid::BR), botMerge);
    // close merge traps
    tmap.trapezoid(topMerge).setNeighbor(Trapezoid::TR,rightT);
    tmap.trapezoid(botMerge).setNeighbor(Trapezoid::BR,rightT);

    updateNeighborsOfNeighbors(SIZE_MAX, topMerge, rightT, botMerge, intersected, tmap, segment);
    /*
    // Update the neighbors of the trapezoid to the right of id_intersected
    if (intersected->getNeighbor(Trapezoid::TR)!=SIZE_MAX) {
        idbuf=intersected->getNeighbor(Trapezoid::TR);
        if (tmap.trapezoid(idbuf).getNeighbor(Trapezoid::TL)==id_intersected) tmap.trapezoid(idbuf).setNeighbor(Trapezoid::TL,rightT);
    }
    if (intersected->getNeighbor(Trapezoid::BR)!=SIZE_MAX) {
        idbuf=intersected->getNeighbor(Trapezoid::BR);
        if (tmap.trapezoid(idbuf).getNeighbor(Trapezoid::BL)==id_intersected) tmap.trapezoid(idbuf).setNeighbor(Trapezoid::BL,rightT);
    }
    */

    if (rightT!=SIZE_MAX) { // with right trap
        // create and link the new subgraph
        Node* rightL=dag.addNode(Node(Node::LEAF,rightT));
        tmap.trapezoid(rightT).setLeafNode(rightL);
        Node* s2N=dag.addNode(Node(Node::YNODE,id_segment, topMergeNode, botMergeNode));
        intersected->getLeafNode()->update(Node::XNODE,tmap.addPoint(segment.p2()), s2N, rightL);
    } else
        intersected->getLeafNode()->update(Node::YNODE,id_segment, topMergeNode, botMergeNode);
    /*
    // create and link the new subgraph
    Node* rightL=dag.addNode(Node(Node::LEAF,rightT));
    tmap.trapezoid(rightT).setLeafNode(rightL);
    Node* s2N=dag.addNode(Node(Node::YNODE,id_segment, topMergeNode, botMergeNode));
    intersected->getLeafNode()->update(Node::XNODE,tmap.addPoint(segment.p2()), s2N, rightL);
    */
    // delete the original trapezoid intersected
    tmap.deleteTrapezoid(id_intersected);
}

/**
 * @brief Algorithms::areEqual() double, points and segments comparisons with adaptive Epsilons
 * https://www.cs.technion.ac.il/users/yechiel/c++-faq/floating-point-arith.html
 */
bool areEqual(double f1, double f2)
{
    // return qAbs(f1 - f2) <= 1.0e-5f * qMax(qAbs(f1), qAbs(f2)); <== this uses qglobal
    const double epsilon = 1.0e-5f; /* some small number such as 1e-5 */;
    double m=(f1 > f2 ? f1 : f2);
    return std::abs(f1 - f2) <= epsilon * std::abs(m);
}

bool areEqual(cg3::Point2d p1, cg3::Point2d p2) {
    return (areEqual(p1.x(),p2.x()) && areEqual(p1.y(),p2.y()));
}

bool areEqual(cg3::Segment2d s1, cg3::Segment2d s2) {
    return areEqual(s1.p1(),s2.p1()) && (areEqual(s1.p2(),s2.p2()));
}

}
