#include "gas_utils.h"

namespace GasUtils {

/**
 * @brief getIntersectionYCoord returns the y coord of the intersection between the segment and
 * the slab x (vertical line)
 */
double getIntersectionYCoord(double xSlab, cg3::Segment2d segment) {
    if (segment.p1().x() != segment.p2().x()) {
        double pdz = (segment.p2().y() - segment.p1().y()) / (segment.p2().x() - segment.p1().x());
        return (pdz*(xSlab - segment.p1().x()) + segment.p1().y());
    } else
        return segment.p1().y();
}

/**
 * @brief pointDirection cross product to check if the point lies to the right or to the left of s
 * if 0 => point and segment are collinear (same direction), use areEqual to check
 */
double pointDirection(cg3::Point2d p, cg3::Segment2d s) {
    return (p.y()-s.p1().y()) * (s.p2().x()-s.p1().x()) - (p.x()-s.p1().x()) * (s.p2().y()-s.p1().y()); // (y−y1)(x2−x1)-(x−x1)(y2−y1)
}

/**
 * @brief isAbove return true if the point lies above the segment s, false otherwise
 * @param sec_point used when the point p is degenerate, used only when inserting a new segment, for the point query it
 * is not necessary (with only one point is imnpossible to solve the ambiguity).
 */
bool isAbove(cg3::Point2d p, cg3::Point2d* sec_point, cg3::Segment2d s) {
    double d=pointDirection(p,s);
    if (areEqual(d,0) && (sec_point!=nullptr)) d=pointDirection(*sec_point,s);
    return d>0;
}

/**
 * @brief areTheSame() for simple double comparisons using Epsilons (without adaptive)
 */
bool areTheSame(double f1, double f2) {
    const double epsilon = 1.0e-5f;
    return std::abs(f1-f2) < epsilon; // std::numeric_limits<double>::epsilon();
}

bool areTheSame(cg3::Point2d p1, cg3::Point2d p2) {
    // return p1==p2;
    return (areTheSame(p1.x(),p2.x()) && areTheSame(p1.y(),p2.y()));
}

bool areEqual(cg3::Point2d p1, cg3::Point2d p2) {
    return p1==p2;
    // return (areTheSame(p1.x(),p2.x()) && areTheSame(p1.y(),p2.y()));
}

bool areEqual(cg3::Segment2d s1, cg3::Segment2d s2) {
    // return (s1.p1()==s2.p1()) && (s1.p2()==s2.p2());
    return areEqual(s1.p1(),s2.p1()) && (areEqual(s1.p2(),s2.p2()));
}

}
