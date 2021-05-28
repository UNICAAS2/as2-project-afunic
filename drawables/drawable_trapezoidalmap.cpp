#include "drawable_trapezoidalmap.h"
#include <cg3/viewer/opengl_objects/opengl_objects2.h>

DrawableTrapezoidalMap::DrawableTrapezoidalMap() {

}

void DrawableTrapezoidalMap::draw() const {
    std::array<cg3::Point2d, 4> trapverts;
    TODO iterare i trapezoidi usando computeVertices...
    for (const Trapezoid& seg : tape) {
            cg3::opengl::drawLine2(seg.p1(), seg.p2(), segmentColor, static_cast<int>(segmentSize));
    }
    /*for (const cg3::Segment2d& seg : getSegments()) {
        cg3::opengl::drawLine2(seg.p1(), seg.p2(), segmentColor, static_cast<int>(segmentSize));
    }*/
}

cg3::Point3d DrawableTrapezoidalMap::sceneCenter() const
{
    // const cg3::BoundingBox2& boundingBox = this->getBoundingBox();
    return cg3::Point3d(0,0,0); // cg3::Point3d(boundingBox.center().x(), boundingBox.center().y(), 0);
}

double DrawableTrapezoidalMap::sceneRadius() const
{
    // const cg3::BoundingBox2& boundingBox = this->getBoundingBox();
    return 0; // boundingBox.diag();
}
