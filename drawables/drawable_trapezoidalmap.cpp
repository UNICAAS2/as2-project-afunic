#include "drawable_trapezoidalmap.h"
#include <cg3/viewer/opengl_objects/opengl_objects2.h>

DrawableTrapezoidalMap::DrawableTrapezoidalMap():TrapezoidalMap() {

}

void DrawableTrapezoidalMap::draw() const {
    //std::array<cg3::Point2d, 4> vertices;
    size_t i=0;
    for (const Trapezoid& trap : getTrapezoids()) {
        if (!trap.isDeleted()) {
            if (i!=id_trapezoid_found)
                cg3::opengl::drawQuad2(trap.getVertices(), (cg3::Color(rand()%128,rand()%128,rand()%128)), 1, true);
                //cg3::opengl::drawLine2(vertices[0], vertices[3], (cg3::Color(0,200,0)), 1);
                //cg3::opengl::drawLine2(vertices[1], vertices[2], (cg3::Color(0,200,0)), 1);
            else
                cg3::opengl::drawQuad2(trap.getVertices(), (cg3::Color(64,255,64)), 1, true);
        }
        i++;
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
