#include "drawable_trapezoidalmap.h"
#include <cg3/viewer/opengl_objects/opengl_objects2.h>
#include "algorithms/algorithms.h"
#include "utils/gas_utils.h"

DrawableTrapezoidalMap::DrawableTrapezoidalMap():TrapezoidalMap() {}

/**
 * @brief DrawableTrapezoidalMap::draw draw the vertical segments and trapezoids
 * - vertical segments are red
 * - the color of the trapezoids simulates random (dark) colors: it depends on the position of the trapezoid
 *   in the trapezoidal map, it is therefore always drawn with the same color but I don't have to store it
 *   in the data structure.
 * - the fired trapezoid (by point query) are light green
 */
void DrawableTrapezoidalMap::draw() const {
    size_t i=0;
    for (const Trapezoid& trap : getTrapezoids()) {
        if (!trap.isDeleted()) {
            // vertices
            std::array<cg3::Point2d, 4> v=trap.getVertices(); // in cwo: TL, TR, BR, BL
            // fill color
            cg3::Color fc=(i==id_trapezoid_found ? cg3::Color(64,255,64): cg3::Color((i*40)%128,(i*30)%128,(i*20)%128));
            if (GasUtils::areTheSame(v[0],v[3])) {
                // TL=BL => left triangle
                cg3::opengl::drawLine2(v[1], v[2], (cg3::Color(255,0,0)), 1);
                cg3::opengl::drawTriangle2(v[0], v[1], v[2], fc, 1, true);
            } else
                if (GasUtils::areTheSame(v[1],v[2])) {
                    // TR=BR => right triangle
                    cg3::opengl::drawLine2(v[1], v[2], (cg3::Color(255,0,0)), 1);
                    cg3::opengl::drawTriangle2(v[0], v[1], v[3], fc, 1, true);
                } else {
                    // normal trapezoid
                    cg3::opengl::drawLine2(v[0], v[3], (cg3::Color(255,0,0)), 1);
                    cg3::opengl::drawLine2(v[1], v[2], (cg3::Color(255,0,0)), 1);
                    cg3::opengl::drawQuad2(v, fc, 1, true);
                }
        }
        i++;
    }
}

cg3::Point3d DrawableTrapezoidalMap::sceneCenter() const {
    // const cg3::BoundingBox2& boundingBox = this->getBoundingBox();
    return cg3::Point3d(0,0,0); // cg3::Point3d(boundingBox.center().x(), boundingBox.center().y(), 0);
}

double DrawableTrapezoidalMap::sceneRadius() const {
    // const cg3::BoundingBox2& boundingBox = this->getBoundingBox();
    return 0; // boundingBox.diag();
}
