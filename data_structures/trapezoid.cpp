#include "trapezoid.h"
#include "utils/gas_utils.h"

Trapezoid::Trapezoid(cg3::Point2d left_p, cg3::Segment2d top_s, cg3::Point2d right_p, cg3::Segment2d bottom_s):
    left_point(left_p),
    top_segment(top_s),
    right_point(right_p),
    bottom_segment(bottom_s),
    top_left_neighbor(SIZE_MAX),
    top_right_neighbor(SIZE_MAX),
    bottom_right_neighbor(SIZE_MAX),
    bottom_left_neighbor(SIZE_MAX),
    leaf_node(nullptr)
{}

void Trapezoid::setNeighbors(size_t top_left, size_t top_right, size_t bottom_right, size_t bottom_left) {
    top_left_neighbor=top_left;
    top_right_neighbor=top_right;
    bottom_right_neighbor=bottom_right;
    bottom_left_neighbor=bottom_left;
}


void Trapezoid::setLeafNode(Node* node) {
    leaf_node=node;
}


/**
 * @brief Trapezoid::getVertices returns the 4 vertices of the trapezoid, used only to draw the trapezoid
 * @param
 * @return
 */
const std::array<cg3::Point2d, 4> Trapezoid::getVertices() const {
    std::array<cg3::Point2d, 4> vertices;
    vertices[0].set(left_point.x(),GasUtils::getIntersectionYCoord(left_point.x(), top_segment));      // top left
    vertices[1].set(right_point.x(),GasUtils::getIntersectionYCoord(right_point.x(), top_segment));    // top right
    vertices[2].set(right_point.x(),GasUtils::getIntersectionYCoord(right_point.x(), bottom_segment)); // bottom right
    vertices[3].set(left_point.x(),GasUtils::getIntersectionYCoord(left_point.x(), bottom_segment));   // bottom left
    return vertices;
};

cg3::Point2d Trapezoid::getLeftPoint() {
    return left_point;
}

cg3::Segment2d Trapezoid::getTopSegment() {
    return top_segment;
}

cg3::Point2d Trapezoid::getRightPoint() {
    return right_point;
}

cg3::Segment2d Trapezoid::getBottomSegment() {
    return bottom_segment;
}

size_t Trapezoid::getNeighbor(NeighborType pos) {
    switch (pos) {
        case TL: return top_left_neighbor; break;
        case TR: return top_right_neighbor; break;
        case BR: return bottom_right_neighbor; break;
        case BL: return bottom_left_neighbor; break;
    }
    return SIZE_MAX;
}

void Trapezoid::setNeighbor(NeighborType pos, size_t id_neighbor) {
    switch (pos) {
        case TL: top_left_neighbor=id_neighbor; break;
        case TR: top_right_neighbor=id_neighbor; break;
        case BR: bottom_right_neighbor=id_neighbor; break;
        case BL: bottom_left_neighbor=id_neighbor; break;
    }
}

Node* Trapezoid::getLeafNode(){
    return leaf_node;
}

void Trapezoid::setRightPoint(cg3::Point2d p) {
    right_point=p;
}
