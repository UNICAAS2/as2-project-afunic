#include "drawable_trapezoidalmap.h"
#include <cg3/viewer/opengl_objects/opengl_objects2.h>

DrawableTrapezoidalMap::DrawableTrapezoidalMap():TrapezoidalMap() {

}

void DrawableTrapezoidalMap::draw() const {
    cg3::Color randCol;
    for (const Trapezoid& trap : getTrapezoids()) {
            randCol=(rand(), rand(), rand(),255);
            cg3::opengl::drawQuad2(trap.getVertices(), randCol, 1, true);
    }
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
