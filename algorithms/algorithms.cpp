#include "algorithms.h"
#include "utils/gas_utils.h"

#define BOUNDINGBOX 1e+6

namespace Algorithms {

/**
 * @brief addSegmentToTrapezoidalMap add a segment to the Trapezoidl Map
 * @param tmap
 * @param dag
 * @param segment
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
    cg3::Segment2d orineted_seg=segment;
    if (segment.p1().x()>segment.p2().x()) {
        orineted_seg.setP1(segment.p2());
        orineted_seg.setP2(segment.p1());
    }

    // trapezoids intersecting the segment
    std::vector<size_t> intersected=FollowSegment(tmap, dag, orineted_seg);
    if (intersected.size()==1)
        oneTrapezoidIntersection(intersected[0], tmap, dag, orineted_seg);
    else
        twoOrMoreTrapezoidsIntersection(intersected, tmap, dag, orineted_seg);
    // clear id_trapezoid_found for the point query
    tmap.id_trapezoid_found=SIZE_MAX;
}

/**
 * @brief FollowSegment find all trapezoids intersected by the segment
 * @param tmap
 * @param dag
 * @param segment
 * @return
 */
std::vector<size_t> FollowSegment(TrapezoidalMap& tmap, Dag& dag, cg3::Segment2d segment) {
    std::vector<size_t> id_traps;
    cg3::Point2d sec_point=segment.p2();
    Node* leaf=queryTrapezoidalMap(segment.p1(), &sec_point, tmap, dag);
    Trapezoid* currtrap=tmap.getTrapezoid(leaf->getValue());
    id_traps.push_back(currtrap->getLeafNode()->getValue());

    // while intersect at least one more to the right of currtrap
    while (segment.p2().x()>currtrap->getRightPoint().x()) {
        // which one TRnieg o BRneig?
        if (currtrap->getNeighbor(Trapezoid::TR)!=SIZE_MAX) {
            if (currtrap->getNeighbor(Trapezoid::BR)!=SIZE_MAX) {
                // it can be one of the two, check the position of rightP respect to the segment
                if (GasUtils::getIntersectionYCoord(currtrap->getRightPoint().x(), segment)<currtrap->getRightPoint().y())
                    currtrap=tmap.getTrapezoid(currtrap->getNeighbor(Trapezoid::BR));
                else
                    currtrap=tmap.getTrapezoid(currtrap->getNeighbor(Trapezoid::TR));
            } else
                currtrap=tmap.getTrapezoid(currtrap->getNeighbor(Trapezoid::TR));
        } else
            currtrap=tmap.getTrapezoid(currtrap->getNeighbor(Trapezoid::BR));
        id_traps.push_back(currtrap->getLeafNode()->getValue());
    }
    return id_traps;
}

/**
 * @brief Algorithms::queryTrapezoidalMap perform the point query
 * @return the pointer of the Leaf that stores the trapezoid ID (return->getValue())
 * and set tmap.id_trapezoid_found for drawableTrapezoidalMap.
 *
 * @param sec_point used only when inserting a new segment, used to solve the problem of degenerate points
 * in the simple query phase it is not used (since there is no second point, it is superfluous to solve the ambiguity)
 */
Node* queryTrapezoidalMap(cg3::Point2d p, cg3::Point2d* sec_point, TrapezoidalMap& tmap, Dag& dag) {
    Node* node=dag.getRootNode();
    while (node->getType()!=Node::LEAF) {
        if (node->getType()==Node::XNODE) { // X node
            node=(p.x()<tmap.point(node->getValue()).x() ? node->getLeftChild() : node->getRightChild());
        } else { // Segment node
            node=(GasUtils::isAbove(p, sec_point, tmap.segment(node->getValue())) ? node->getLeftChild() : node->getRightChild());
        }
    }
    tmap.id_trapezoid_found=node->getValue();
    return node;
}

/**
 * @brief Algorithms::oneTrapezoidIntersection used when the segment lies entirely in a trapezoid, all in one,
 * 1. add new traps (recycling the element <vector>[id_intersected] to avoid delete operations on <vector>
 *    and to keep the structure clean without holes)
 * 2. set neighbors, update adjacency (the neighbors of the neighbors of the trap intersected),
 * 3. update dag
 */
void oneTrapezoidIntersection(size_t id_intersected, TrapezoidalMap& tmap, Dag& dag, cg3::Segment2d segment) {
    Trapezoid intersected = *tmap.getTrapezoid(id_intersected);

    //=================[ Add new trapezoids: 2,3 or 4 ]=================
    // left trap only if the segment.p1.x > leftP.x of the trap
    size_t leftT=SIZE_MAX;
    if (segment.p1().x()>intersected.getLeftPoint().x())
        leftT=tmap.addTrapezoid(Trapezoid(intersected.getLeftPoint(), intersected.getTopSegment(), segment.p1(), intersected.getBottomSegment()));
    // top always exists:
    size_t topT=tmap.addTrapezoid(Trapezoid(segment.p1(), intersected.getTopSegment(), segment.p2(), segment));
    // right trap only if the segment.p2.x < rightP.x of the trap
    size_t rightT=SIZE_MAX;
    if (segment.p2().x()<intersected.getRightPoint().x())
        rightT=tmap.addTrapezoid(Trapezoid(segment.p2(), intersected.getTopSegment(), intersected.getRightPoint(), intersected.getBottomSegment()));
    // bottom always exists: I recycle the element of the <vector> that I should delete, this avoids the delete operation
    size_t bottomT=tmap.setTrapezoid(id_intersected, Trapezoid(segment.p1(), segment, segment.p2(), intersected.getBottomSegment()));

    //=================[ Set neighbors ]=================
    // if left trap exists
    if (leftT!=SIZE_MAX)
        tmap.trapezoid(leftT).setNeighbors(intersected.getNeighbor(Trapezoid::TL), topT, bottomT, intersected.getNeighbor(Trapezoid::BL));
    // top trap always exists, but the neighbors depend on the existence of the left & right trap and if top or bot tra are tringles
    tmap.trapezoid(topT).setNeighbors(
                (GasUtils::areTheSame(segment.p1().x(),intersected.getLeftPoint().x()) && GasUtils::areEqual(segment.p1(),intersected.getTopSegment().p1())) ? SIZE_MAX : (GasUtils::areTheSame(segment.p1().x(),intersected.getLeftPoint().x()) ? intersected.getNeighbor(Trapezoid::TL) : leftT),
                (GasUtils::areTheSame(segment.p2().x(),intersected.getRightPoint().x()) && GasUtils::areEqual(segment.p2(),intersected.getTopSegment().p2())) ? SIZE_MAX : (GasUtils::areTheSame(segment.p2().x(),intersected.getRightPoint().x()) ? intersected.getNeighbor(Trapezoid::TR) : rightT),
                SIZE_MAX,
                SIZE_MAX);
    // if right trap exists
    if (rightT!=SIZE_MAX)
        tmap.trapezoid(rightT).setNeighbors(topT, intersected.getNeighbor(Trapezoid::TR), intersected.getNeighbor(Trapezoid::BR), bottomT);
    // bottom trap always exists, but the neighbors depend on the existence of the left & right trap and if top or bot tra are tringles
    tmap.trapezoid(bottomT).setNeighbors(
                SIZE_MAX,
                SIZE_MAX,
                (GasUtils::areTheSame(segment.p2().x(),intersected.getRightPoint().x()) && GasUtils::areEqual(segment.p2(),intersected.getBottomSegment().p2())) ? SIZE_MAX : (GasUtils::areTheSame(segment.p2().x(),intersected.getRightPoint().x()) ? intersected.getNeighbor(Trapezoid::BR) : rightT),
                (GasUtils::areTheSame(segment.p1().x(),intersected.getLeftPoint().x()) && GasUtils::areEqual(segment.p1(),intersected.getBottomSegment().p1())) ? SIZE_MAX : (GasUtils::areTheSame(segment.p1().x(),intersected.getLeftPoint().x()) ? intersected.getNeighbor(Trapezoid::BL) : leftT));

    //=================[ adjacent neighbors: update the neighbors of the neighbors of the trapzoid intersected ]=================
    // Update the neighbors of the trapezoid to the top left of intersected
    if (intersected.getNeighbor(Trapezoid::TL)!=SIZE_MAX) {
        size_t idbuf=intersected.getNeighbor(Trapezoid::TL);
        // if share the same segment, this must be updated with new one (if exixts)
        if (leftT!=SIZE_MAX) // if exists this is the correct right neighb.
            tmap.trapezoid(idbuf).setNeighbor(Trapezoid::TR,leftT);
        else
            if (GasUtils::areTheSame(segment.p1().x(),intersected.getLeftPoint().x()) && GasUtils::areEqual(segment.p1(),intersected.getTopSegment().p1())) // if triangle => no right neighbor
                tmap.trapezoid(idbuf).setNeighbor(Trapezoid::TR,SIZE_MAX);
            else
                tmap.trapezoid(idbuf).setNeighbor(Trapezoid::TR,topT); // if no left and no triangle => top right neighbor is top trap...
    }
    // Update the neighbors of the trapezoid to the bot left of intersected
    if (intersected.getNeighbor(Trapezoid::BL)!=SIZE_MAX) {
        size_t idbuf=intersected.getNeighbor(Trapezoid::BL);
        // if share the same segment, this must be updated with new one (if exixts)
        if (leftT!=SIZE_MAX) // if exists this is the correct right neighb.
            tmap.trapezoid(idbuf).setNeighbor(Trapezoid::BR,leftT);
        else
            if (GasUtils::areTheSame(segment.p1().x(),intersected.getLeftPoint().x()) && GasUtils::areEqual(segment.p1(),intersected.getBottomSegment().p1())) // if triangle => no right neighbor
                tmap.trapezoid(idbuf).setNeighbor(Trapezoid::BR,SIZE_MAX);
            else
                tmap.trapezoid(idbuf).setNeighbor(Trapezoid::BR,bottomT); // if no left and no triangle => bot right neighbor is bot trap...
    }
    // Update the neighbors of the trapezoid to the top right of intersected
    if (intersected.getNeighbor(Trapezoid::TR)!=SIZE_MAX) {
        size_t idbuf=intersected.getNeighbor(Trapezoid::TR);
        // if share the same segment, this must be updated with new one (if exixts)
        if (rightT!=SIZE_MAX) // if exists this is the correct left neighb.
            tmap.trapezoid(idbuf).setNeighbor(Trapezoid::TL,rightT);
        else
            if (GasUtils::areTheSame(segment.p2().x(),intersected.getRightPoint().x()) && GasUtils::areEqual(segment.p2(),intersected.getTopSegment().p2())) // if triangle => no right neighbor
                tmap.trapezoid(idbuf).setNeighbor(Trapezoid::TL,SIZE_MAX);
            else
                tmap.trapezoid(idbuf).setNeighbor(Trapezoid::TL,topT); // if no right and no triangle => top left neighbor is top trap...
    }
    // Update the neighbors of the trapezoid to the bot right of intersected
    if (intersected.getNeighbor(Trapezoid::BR)!=SIZE_MAX) {
        size_t idbuf=intersected.getNeighbor(Trapezoid::BR);
        // if share the same segment, this must be updated with new one (if exixts)
        if (rightT!=SIZE_MAX) // if exists this is the correct left neighb.
            tmap.trapezoid(idbuf).setNeighbor(Trapezoid::BL,rightT);
        else
            if (GasUtils::areTheSame(segment.p2().x(),intersected.getRightPoint().x()) && GasUtils::areEqual(segment.p2(),intersected.getBottomSegment().p2())) // if triangle => no right neighbor
                tmap.trapezoid(idbuf).setNeighbor(Trapezoid::BL,SIZE_MAX);
            else
                tmap.trapezoid(idbuf).setNeighbor(Trapezoid::BL,bottomT); // if no right and no triangle => bot left neighbor is bot trap...
    }

    //=================[ Update the dag ]=================
    // update the dag, create and link the new subgraph for all cases: not degen 4 traps, degen 2 or 3 traps
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
        intersected.getLeafNode()->update(Node::XNODE,tmap.addPoint(segment.p1()),leftL, q1N);
    } else
        if (rightT!=SIZE_MAX) { // without left trap
            Node* s1N=dag.addNode(Node(Node::YNODE,tmap.addSegment(segment), topL, bottomL));
            Node* rightL=dag.addNode(Node(Node::LEAF,rightT));
            tmap.trapezoid(rightT).setLeafNode(rightL);
            // link the new subgraph
            intersected.getLeafNode()->update(Node::XNODE,tmap.addPoint(segment.p2()), s1N, rightL);
        } else
            if (leftT!=SIZE_MAX) { // without right trap
                Node* s1N=dag.addNode(Node(Node::YNODE,tmap.addSegment(segment), topL, bottomL));
                Node* leftL=dag.addNode(Node(Node::LEAF,leftT));
                tmap.trapezoid(leftT).setLeafNode(leftL);
                // link the new subgraph
                intersected.getLeafNode()->update(Node::XNODE,tmap.addPoint(segment.p1()),leftL, s1N);
            } else // only top and bot
                intersected.getLeafNode()->update(Node::YNODE,tmap.addSegment(segment), topL, bottomL);
}

/**
 * @brief twoOrMoreTrapezoidsIntersection used when a segment intersects 2 or more trapezoids.
 * 1. split the leftmost intersected trapezoid (recycling the element <vector>[id_intersected] to avoid delete
 *    operations on <vector> and to keep the structure clean without holes)
 *    - add new traps, set neighbors, update adjacency (neigs of neigs), update dag
 * 2. continue if present with internal crossed trapezoids
  *    - add new traps, set neighbors, update adjacency (neigs of neigs), update dag
 * 3. adds the trapezoids obtained from the rightmost trapezoid (with 1 recycle)
  *    - add new traps, set neighbors, update adjacency (neigs of neigs), update dag
 */
void twoOrMoreTrapezoidsIntersection(std::vector<size_t> traps_intersect, TrapezoidalMap& tmap, Dag& dag, cg3::Segment2d segment) {
    size_t idbuf, newTopMerge, newBotMerge;

    if (traps_intersect.size()<2) return; // error, the point is not inside the bounding box, throw "MMM click on the bbox...";

    //=======================[ First trap intersected (leftmost) ]=======================
    size_t id_intersected = traps_intersect[0];
    Trapezoid intersected = *tmap.getTrapezoid(id_intersected);

    // add the new traps, left trap only if the segment.p1.x > leftP.x of the trap
    size_t leftT=SIZE_MAX;
    if (segment.p1().x()>intersected.getLeftPoint().x())
        leftT=tmap.addTrapezoid(Trapezoid(intersected.getLeftPoint(), intersected.getTopSegment(), segment.p1(), intersected.getBottomSegment()));
    // top always exists
    size_t topMerge=tmap.addTrapezoid(Trapezoid(segment.p1(), intersected.getTopSegment(), intersected.getRightPoint(), segment));
    // bottom always exists: recycle..
    // size_t botMerge=tmap.addTrapezoid(
    size_t botMerge=tmap.setTrapezoid(id_intersected, Trapezoid(segment.p1(), segment, intersected.getRightPoint(), intersected.getBottomSegment()));

    // set the neighbors of the new traps
    // if left trap exists
    if (leftT!=SIZE_MAX)
        tmap.trapezoid(leftT).setNeighbors(intersected.getNeighbor(Trapezoid::TL), topMerge, botMerge, intersected.getNeighbor(Trapezoid::BL));
    // top trap always exists, but the neighbors depend on the existence of the left trap and if are tringles
    tmap.trapezoid(topMerge).setNeighbors(
                (GasUtils::areTheSame(segment.p1().x(),intersected.getLeftPoint().x()) && GasUtils::areEqual(segment.p1(),intersected.getTopSegment().p1())) ? SIZE_MAX : (GasUtils::areTheSame(segment.p1().x(),intersected.getLeftPoint().x()) ? intersected.getNeighbor(Trapezoid::TL) : leftT),
                intersected.getNeighbor(Trapezoid::TR),// (GasUtils::areTheSame(segment.p2().x(),intersected.getRightPoint().x()) && GasUtils::areEqual(segment.p2(),intersected.getTopSegment().p2())) ? SIZE_MAX : (GasUtils::areTheSame(segment.p2().x(),intersected.getRightPoint().x()) ? intersected.getNeighbor(Trapezoid::TR) : rightT),
                SIZE_MAX,
                SIZE_MAX);
    // bottom trap always exists, but the neighbors depend on the existence of the left and if are tringles
    tmap.trapezoid(botMerge).setNeighbors(
                SIZE_MAX,
                SIZE_MAX,
                intersected.getNeighbor(Trapezoid::BR), // (GasUtils::areTheSame(segment.p2().x(),intersected.getRightPoint().x()) && GasUtils::areEqual(segment.p2(),intersected.getTopSegment().p2())) ? SIZE_MAX : (GasUtils::areTheSame(segment.p2().x(),intersected.getRightPoint().x()) ? intersected.getNeighbor(Trapezoid::BR) : rightT),
                (GasUtils::areTheSame(segment.p1().x(),intersected.getLeftPoint().x()) && GasUtils::areEqual(segment.p1(),intersected.getBottomSegment().p1())) ? SIZE_MAX : (GasUtils::areTheSame(segment.p1().x(),intersected.getLeftPoint().x()) ? intersected.getNeighbor(Trapezoid::BL) : leftT));

    // Neighbors of neighbors, update the neighbors of the trapezoid to the top left of intersected
    if (intersected.getNeighbor(Trapezoid::TL)!=SIZE_MAX) {
        idbuf=intersected.getNeighbor(Trapezoid::TL);
        // if share the same segment, this must be updated with new one (if exixts)
        if (leftT!=SIZE_MAX) // if exists this is the correct right neighb.
            tmap.trapezoid(idbuf).setNeighbor(Trapezoid::TR,leftT);
        else
            if (GasUtils::areTheSame(segment.p1().x(),intersected.getLeftPoint().x()) && GasUtils::areEqual(segment.p1(),intersected.getTopSegment().p1())) // if triangle => no right neighbor
                tmap.trapezoid(idbuf).setNeighbor(Trapezoid::TR,SIZE_MAX);
            else
                tmap.trapezoid(idbuf).setNeighbor(Trapezoid::TR,topMerge); // if no left and no triangle => top right neighbor is top trap...
    }
    // Update the neighbors of the trapezoid to the bot left of intersected
    if (intersected.getNeighbor(Trapezoid::BL)!=SIZE_MAX) {
        idbuf=intersected.getNeighbor(Trapezoid::BL);
        // if share the same segment, this must be updated with new one (if exixts)
        if (leftT!=SIZE_MAX) // if exists this is the correct right neighb.
            tmap.trapezoid(idbuf).setNeighbor(Trapezoid::BR,leftT);
        else
            if (GasUtils::areTheSame(segment.p1().x(),intersected.getLeftPoint().x()) && GasUtils::areEqual(segment.p1(),intersected.getBottomSegment().p1())) // if triangle => no right neighbor
                tmap.trapezoid(idbuf).setNeighbor(Trapezoid::BR,SIZE_MAX);
            else
                tmap.trapezoid(idbuf).setNeighbor(Trapezoid::BR,botMerge); // if no left and no triangle => bot right neighbor is bot trap...
    }
    // Update the neighbors of the trapezoid to the top right of intersected
    if (intersected.getNeighbor(Trapezoid::TR)!=SIZE_MAX) {
        idbuf=intersected.getNeighbor(Trapezoid::TR);
        if (tmap.trapezoid(idbuf).getNeighbor(Trapezoid::TL)==id_intersected) tmap.trapezoid(idbuf).setNeighbor(Trapezoid::TL,topMerge);
    }
    // Update the neighbors of the trapezoid to the bot right of intersected
    if (intersected.getNeighbor(Trapezoid::BR)!=SIZE_MAX) {
        idbuf=intersected.getNeighbor(Trapezoid::BR);
        if (tmap.trapezoid(idbuf).getNeighbor(Trapezoid::BL)==id_intersected) tmap.trapezoid(idbuf).setNeighbor(Trapezoid::BL,botMerge);
    }

    // update the Dag
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
        intersected.getLeafNode()->update(Node::XNODE,tmap.addPoint(segment.p1()),leftL, s1N);
    } else // only top and bot
        intersected.getLeafNode()->update(Node::YNODE,id_segment, topMergeNode, botMergeNode);

    // Now tmap & dag are consistent, only
    // - topMerge.neighbor(BR) and
    // - botMerge.neighbor(TR)
    // need to be updated based on mergers

    //=======================[ Internal trapezoids intersected ]=======================
    for (size_t id=1; id<traps_intersect.size()-1; id++) {
        id_intersected = traps_intersect[id];
        intersected = *tmap.getTrapezoid(id_intersected);
        bool already_recycled=false;

        newTopMerge=SIZE_MAX;
        newBotMerge=SIZE_MAX;

        // Add 2 new traps, top & bot, top & bot could be merged with previus top & bot traps (topMerge & botMerge)
        // new top trap merge with topMerge?
        if (GasUtils::areEqual(tmap.trapezoid(topMerge).getTopSegment(),intersected.getTopSegment()) && GasUtils::areEqual(tmap.trapezoid(topMerge).getBottomSegment(),segment)) {
            // merge, no new trap in tmap
            tmap.trapezoid(topMerge).setRightPoint(intersected.getRightPoint());
            tmap.trapezoid(topMerge).setNeighbor(Trapezoid::TR,intersected.getNeighbor(Trapezoid::TR));
            // Update the neighbors of the trapezoid to the top right of id_intersected
            if (intersected.getNeighbor(Trapezoid::TR)!=SIZE_MAX) {
                idbuf=intersected.getNeighbor(Trapezoid::TR);
                if (tmap.trapezoid(idbuf).getNeighbor(Trapezoid::TL)==id_intersected) tmap.trapezoid(idbuf).setNeighbor(Trapezoid::TL,topMerge);
            }
            // & to the top left
            if (intersected.getNeighbor(Trapezoid::TL)!=SIZE_MAX) {
                idbuf=intersected.getNeighbor(Trapezoid::TL);
                if (tmap.trapezoid(idbuf).getNeighbor(Trapezoid::TR)==id_intersected) tmap.trapezoid(idbuf).setNeighbor(Trapezoid::TR,topMerge);
            }
            // topMergeNode remains OK, contains topMerge
        } else {
            // no merge, "close" topMerge and add 1 new trap in tmap
            newTopMerge=tmap.setTrapezoid(id_intersected,Trapezoid(intersected.getLeftPoint(), intersected.getTopSegment(), intersected.getRightPoint(), segment));
            already_recycled=true;
            // neighbors of the new top trap
            tmap.trapezoid(newTopMerge).setNeighbors(intersected.getNeighbor(Trapezoid::TL), intersected.getNeighbor(Trapezoid::TR), SIZE_MAX/*to be set later*/, topMerge);
            // close old top merge (set neighbors)
            tmap.trapezoid(topMerge).setNeighbor(Trapezoid::BR,newTopMerge);

            // Update the neighbors of the trapezoid to the top right of id_intersected
            if (intersected.getNeighbor(Trapezoid::TR)!=SIZE_MAX) {
                idbuf=intersected.getNeighbor(Trapezoid::TR);
                if (tmap.trapezoid(idbuf).getNeighbor(Trapezoid::TL)==id_intersected) tmap.trapezoid(idbuf).setNeighbor(Trapezoid::TL,newTopMerge);
            }
            // & to the top left
            if (intersected.getNeighbor(Trapezoid::TL)!=SIZE_MAX) {
                idbuf=intersected.getNeighbor(Trapezoid::TL);
                if (tmap.trapezoid(idbuf).getNeighbor(Trapezoid::TR)==id_intersected) tmap.trapezoid(idbuf).setNeighbor(Trapezoid::TR,newTopMerge);
            }

            // no merge => I need a new leaf to store the new trap
            topMergeNode=dag.addNode(Node(Node::LEAF,newTopMerge));
            tmap.trapezoid(newTopMerge).setLeafNode(topMergeNode);
            topMerge=newTopMerge;
        }

        // new bot trap merge with botMerge?
        if (GasUtils::areEqual(tmap.trapezoid(botMerge).getTopSegment(),segment) && GasUtils::areEqual(tmap.trapezoid(botMerge).getBottomSegment(),intersected.getBottomSegment())) {
            // merge, no new trap in tmap
            tmap.trapezoid(botMerge).setRightPoint(intersected.getRightPoint());
            tmap.trapezoid(botMerge).setNeighbor(Trapezoid::BR,intersected.getNeighbor(Trapezoid::BR));
            // Update the neighbors of the trapezoid to the bot right of id_intersected
            if (intersected.getNeighbor(Trapezoid::BR)!=SIZE_MAX) {
                idbuf=intersected.getNeighbor(Trapezoid::BR);
                if (tmap.trapezoid(idbuf).getNeighbor(Trapezoid::BL)==id_intersected) tmap.trapezoid(idbuf).setNeighbor(Trapezoid::BL,botMerge);
            }
            // & to bot left
            if (intersected.getNeighbor(Trapezoid::BL)!=SIZE_MAX) {
                idbuf=intersected.getNeighbor(Trapezoid::BL);
                if (tmap.trapezoid(idbuf).getNeighbor(Trapezoid::BR)==id_intersected) tmap.trapezoid(idbuf).setNeighbor(Trapezoid::BR,botMerge);
            }
            // botMergeNode remains OK, contains botMerge
        } else {
            // no merge, "close" botMerge and add 1 new trap in tmap
            if (already_recycled)
                newBotMerge=tmap.addTrapezoid(Trapezoid(intersected.getLeftPoint(), segment, intersected.getRightPoint(), intersected.getBottomSegment()));
            else {
                newBotMerge=tmap.setTrapezoid(id_intersected,Trapezoid(intersected.getLeftPoint(), segment, intersected.getRightPoint(), intersected.getBottomSegment()));
                already_recycled=true;
            }
            // neighbors of the new top trap
            tmap.trapezoid(newBotMerge).setNeighbors(botMerge, SIZE_MAX/*to be set later*/, intersected.getNeighbor(Trapezoid::BR), intersected.getNeighbor(Trapezoid::BL));

            // close old bot merge (set neighbors)
            tmap.trapezoid(botMerge).setNeighbor(Trapezoid::TR,newBotMerge);

            // Update the neighbors of the trapezoid to the bot right of id_intersected
            if (intersected.getNeighbor(Trapezoid::BR)!=SIZE_MAX) {
                idbuf=intersected.getNeighbor(Trapezoid::BR);
                if (tmap.trapezoid(idbuf).getNeighbor(Trapezoid::BL)==id_intersected) tmap.trapezoid(idbuf).setNeighbor(Trapezoid::BL,newBotMerge);
            }
            // & to bot left
            if (intersected.getNeighbor(Trapezoid::BL)!=SIZE_MAX) {
                idbuf=intersected.getNeighbor(Trapezoid::BL);
                if (tmap.trapezoid(idbuf).getNeighbor(Trapezoid::BR)==id_intersected) tmap.trapezoid(idbuf).setNeighbor(Trapezoid::BR,newBotMerge);
            }

            // no merge => I need a new leaft to store the new trap
            botMergeNode=dag.addNode(Node(Node::LEAF,newBotMerge));
            tmap.trapezoid(newBotMerge).setLeafNode(botMergeNode);
            botMerge=newBotMerge;
        }

        // link the new subgraph, I need a new segment node, this sub graph replace the leaf node
        intersected.getLeafNode()->update(Node::YNODE,id_segment, topMergeNode, botMergeNode);
    }

    //=======================[ Last trap intersected (rightmost) ]=======================
    id_intersected = traps_intersect.back();
    intersected = *tmap.getTrapezoid(id_intersected);
    newTopMerge=SIZE_MAX;
    newBotMerge=SIZE_MAX;
    bool recycled=false;

    // new top trap merge with topMerge?
    if (GasUtils::areEqual(tmap.trapezoid(topMerge).getTopSegment(),intersected.getTopSegment()) && GasUtils::areEqual(tmap.trapezoid(topMerge).getBottomSegment(),segment)) {
        // merge, no new trap in tmap
        tmap.trapezoid(topMerge).setRightPoint(segment.p2());
        tmap.trapezoid(topMerge).setNeighbor(Trapezoid::TR,SIZE_MAX); /*to be set later*/
        tmap.trapezoid(topMerge).setNeighbor(Trapezoid::BR,SIZE_MAX);
        // topMergeNode remains OK, contains topMerge
    } else {
        // no merge, "close" topMerge and add 1 new trap in tmap
        newTopMerge=tmap.setTrapezoid(id_intersected,Trapezoid(intersected.getLeftPoint(), intersected.getTopSegment(), segment.p2(), segment));
        recycled=true;
        // neighbors of the new top trap
        tmap.trapezoid(newTopMerge).setNeighbors(intersected.getNeighbor(Trapezoid::TL), SIZE_MAX/*to be set later*/, SIZE_MAX, topMerge);
        // close old top merge (set neighbors)
        tmap.trapezoid(topMerge).setNeighbor(Trapezoid::BR,newTopMerge);
        // & update the TL neighbor
        if (intersected.getNeighbor(Trapezoid::TL)!=SIZE_MAX) {
            idbuf=intersected.getNeighbor(Trapezoid::TL);
            if (tmap.trapezoid(idbuf).getNeighbor(Trapezoid::TR)==id_intersected) tmap.trapezoid(idbuf).setNeighbor(Trapezoid::TR,newTopMerge);
        }

        // no merge => I need a new leaft to store the new trap
        topMergeNode=dag.addNode(Node(Node::LEAF,newTopMerge));
        tmap.trapezoid(newTopMerge).setLeafNode(topMergeNode);
        topMerge=newTopMerge; // mah!
    }

    // new bot trap merge with botMerge?
    if (GasUtils::areEqual(tmap.trapezoid(botMerge).getTopSegment(),segment) && GasUtils::areEqual(tmap.trapezoid(botMerge).getBottomSegment(),intersected.getBottomSegment())) {
        // merge, no new trap in tmap
        tmap.trapezoid(botMerge).setRightPoint(segment.p2());
        tmap.trapezoid(botMerge).setNeighbor(Trapezoid::TR,SIZE_MAX);
        tmap.trapezoid(botMerge).setNeighbor(Trapezoid::BR,SIZE_MAX); /*to be set later*/
        // botMergeNode remains OK, contains botMerge
    } else {
        // no merge, "close" botMerge and add 1 new trap in tmap
        if (recycled)
            newBotMerge=tmap.addTrapezoid(Trapezoid(intersected.getLeftPoint(), segment, segment.p2(), intersected.getBottomSegment()));
        else {
            newBotMerge=tmap.setTrapezoid(id_intersected,Trapezoid(intersected.getLeftPoint(), segment, segment.p2(), intersected.getBottomSegment()));
            recycled=true;
        }
        // neighbors of the new top trap
        tmap.trapezoid(newBotMerge).setNeighbors(botMerge, SIZE_MAX, SIZE_MAX/*to be set later*/, intersected.getNeighbor(Trapezoid::BL));
        // close old bot merge (set neighbors)
        tmap.trapezoid(botMerge).setNeighbor(Trapezoid::TR,newBotMerge);
        // & update the BL neighbor
        if (intersected.getNeighbor(Trapezoid::BL)!=SIZE_MAX) {
            idbuf=intersected.getNeighbor(Trapezoid::BL);
            if (tmap.trapezoid(idbuf).getNeighbor(Trapezoid::BR)==id_intersected) tmap.trapezoid(idbuf).setNeighbor(Trapezoid::BR,newBotMerge);
        }

        // no merge => i need a new leaft to store the new trap
        botMergeNode=dag.addNode(Node(Node::LEAF,newBotMerge));
        tmap.trapezoid(newBotMerge).setLeafNode(botMergeNode);
        botMerge=newBotMerge;
    }

    // the new rightmost trpezoid
    size_t rightT=SIZE_MAX;
    if (segment.p2().x()<intersected.getRightPoint().x()) {
        if (recycled)
            rightT=tmap.addTrapezoid(Trapezoid(segment.p2(), intersected.getTopSegment(), intersected.getRightPoint(), intersected.getBottomSegment()));
        else {
            rightT=tmap.setTrapezoid(id_intersected,Trapezoid(segment.p2(), intersected.getTopSegment(), intersected.getRightPoint(), intersected.getBottomSegment()));
            recycled=true;
        }
    }

    // The neighbors, if right trap exists
    if (rightT!=SIZE_MAX)
        tmap.trapezoid(rightT).setNeighbors(topMerge, intersected.getNeighbor(Trapezoid::TR), intersected.getNeighbor(Trapezoid::BR), botMerge);
    // close merge traps
    tmap.trapezoid(topMerge).setNeighbor(Trapezoid::TR,(GasUtils::areTheSame(segment.p2().x(),intersected.getRightPoint().x()) && GasUtils::areEqual(segment.p2(),intersected.getTopSegment().p2())) ? SIZE_MAX : (GasUtils::areTheSame(segment.p2().x(),intersected.getRightPoint().x()) ? intersected.getNeighbor(Trapezoid::TR) : rightT));
    tmap.trapezoid(botMerge).setNeighbor(Trapezoid::BR,(GasUtils::areTheSame(segment.p2().x(),intersected.getRightPoint().x()) && GasUtils::areEqual(segment.p2(),intersected.getBottomSegment().p2())) ? SIZE_MAX : (GasUtils::areTheSame(segment.p2().x(),intersected.getRightPoint().x()) ? intersected.getNeighbor(Trapezoid::BR) : rightT));

    // Update the neighbors of the trapezoid to the top right of id_intersected
    if (intersected.getNeighbor(Trapezoid::TR)!=SIZE_MAX) {
        idbuf=intersected.getNeighbor(Trapezoid::TR);
        // if share the same segment, this must be updated with new one (if exixts)
        if (rightT!=SIZE_MAX) // if exists this is the correct left neighb.
            tmap.trapezoid(idbuf).setNeighbor(Trapezoid::TL,rightT);
        else
            if (GasUtils::areTheSame(segment.p2().x(),intersected.getRightPoint().x()) && GasUtils::areEqual(segment.p2(),intersected.getTopSegment().p2())) // if triangle => no right neighbor
                tmap.trapezoid(idbuf).setNeighbor(Trapezoid::TL,SIZE_MAX);
            else
                tmap.trapezoid(idbuf).setNeighbor(Trapezoid::TL,topMerge); // if no right and no triangle => top left neighbor is top trap...
    }
    // Update the neighbors of the trapezoid to the bot right of id_intersected
    if (intersected.getNeighbor(Trapezoid::BR)!=SIZE_MAX) {
        idbuf=intersected.getNeighbor(Trapezoid::BR);
        // if share the same segment, this must be updated with new one (if exixts)
        if (rightT!=SIZE_MAX) // if exists this is the correct left neighb.
            tmap.trapezoid(idbuf).setNeighbor(Trapezoid::BL,rightT);
        else
            if (GasUtils::areTheSame(segment.p2().x(),intersected.getRightPoint().x()) && GasUtils::areEqual(segment.p2(),intersected.getBottomSegment().p2())) // if triangle => no right neighbor
                tmap.trapezoid(idbuf).setNeighbor(Trapezoid::BL,SIZE_MAX);
            else
                tmap.trapezoid(idbuf).setNeighbor(Trapezoid::BL,botMerge); // if no right and no triangle => bot left neighbor is bot trap...
    }

    // update the dag
    if (rightT!=SIZE_MAX) { // with right trap
        Node* rightL=dag.addNode(Node(Node::LEAF,rightT));
        tmap.trapezoid(rightT).setLeafNode(rightL);
        Node* s2N=dag.addNode(Node(Node::YNODE,id_segment, topMergeNode, botMergeNode));
        // link the new subgraph
        intersected.getLeafNode()->update(Node::XNODE,tmap.addPoint(segment.p2()), s2N, rightL);
    } else // no right trap => YNode for only top & bot leafs
        intersected.getLeafNode()->update(Node::YNODE,id_segment, topMergeNode, botMergeNode);
}

}
