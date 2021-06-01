#include "trapezoid.h"

Trapezoid::Trapezoid(cg3::Point2d left_p, cg3::Segment2d top_s, cg3::Point2d right_p, cg3::Segment2d bottom_s):
    left_point(left_p),
    top_segment(top_s),
    right_point(right_p),
    bottom_segment(bottom_s)/*,
    leaf_node_id(SIZE_MAX)*/
{}

void Trapezoid::setNeighbors(size_t top_left, size_t top_right, size_t bottom_right, size_t bottom_left) {
    top_left_neighbor=top_left;
    top_right_neighbor=top_right;
    bottom_right_neighbor=bottom_right;
    bottom_left_neighbor=bottom_left;
}

/*
void Trapezoid::setLeafNode(size_t node_id) {
    leaf_node_id=node_id;
}
*/

/**
 * @brief Trapezoid::getVertices
 * @param
 * @return
 */
const std::array<cg3::Point2d, 4> Trapezoid::getVertices() const {
    std::array<cg3::Point2d, 4> vertices;
    vertices[0].set(left_point.x(),getIntersectionYCoord(left_point.x(), top_segment));      // top left
    vertices[1].set(right_point.x(),getIntersectionYCoord(right_point.x(), top_segment));    // top right
    vertices[2].set(right_point.x(),getIntersectionYCoord(right_point.x(), bottom_segment)); // bottom right
    vertices[3].set(left_point.x(),getIntersectionYCoord(left_point.x(), bottom_segment));   // bottom left
    return vertices;
};

cg3::Point2d Trapezoid::getLeftPoint() {
    return left_point;
}

cg3::Segment2d Trapezoid::getTopSegment(){
    return top_segment;
}

cg3::Point2d Trapezoid::getRightPoint(){
    return right_point;
}

cg3::Segment2d Trapezoid::getBottomSegment(){
    return bottom_segment;
}

/*
size_t Trapezoid::getLeafNode(){
    return leaf_node_id;
}
*/

/**
 * @brief rapezoid::getIntersectionYCoord
 * @param
 * @return la y dell'intersezione tra la slap (linea verticale) e il segment
 */
double Trapezoid::getIntersectionYCoord(double xSlab, cg3::Segment2d segment) const {
    if (segment.p1().x() != segment.p2().x()) {
        double pdz = (segment.p2().y() - segment.p1().y()) / (segment.p2().x() - segment.p1().x());
        return (pdz*(xSlab - segment.p1().x()) + segment.p1().y());
    } else
        return segment.p1().y();
}
