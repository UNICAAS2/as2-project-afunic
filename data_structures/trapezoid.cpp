#include "trapezoid.h"

Trapezoid::Trapezoid(cg3::Point2d left, cg3::Segment2d top, cg3::Point2d right, cg3::Segment2d bottom) {
    left_point=left;
    top_segment=top;
    right_point=right;
    bottom_segment=bottom;
    leaf_node_id=SIZE_MAX; // like nullptr, SIZE_MAX => null value
}

void Trapezoid::setNeighbors(size_t top_left, size_t top_right, size_t bottom_right, size_t bottom_left) {
    top_left_neighbor=top_left;
    top_right_neighbor=top_right;
    bottom_right_neighbor=bottom_right;
    bottom_left_neighbor=bottom_left;
}

/**
 * @brief rapezoid::computeVertices
 * @param
 * @return
 */
void Trapezoid::computeVertices(std::array<cg3::Point2d, 4>& vertices) {
    vertices[0].set(left_point.x(),getIntersectionYCoord(left_point.x(), top_segment));      // top left
    vertices[1].set(right_point.x(),getIntersectionYCoord(right_point.x(), top_segment));    // top right
    vertices[2].set(right_point.x(),getIntersectionYCoord(right_point.x(), bottom_segment)); // bottom right
    vertices[3].set(left_point.x(),getIntersectionYCoord(left_point.x(), bottom_segment));   // bottom left
}

/**
 * @brief rapezoid::getIntersectionYCoord
 * @param
 * @return la y dell'intersezione tra la slap (linea verticale) e il segment
 */
double Trapezoid::getIntersectionYCoord(double xSlab, cg3::Segment2d segment) {
    if (segment.p1().x() != segment.p2().x()) {
        double pdz = (segment.p2().y() - segment.p1().y()) / (segment.p2().x() - segment.p1().x());
        return (pdz*(xSlab - segment.p1().x()) + segment.p1().y());
    } else
        return segment.p1().y();
}
