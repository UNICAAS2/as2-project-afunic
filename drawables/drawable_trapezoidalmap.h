#ifndef DRAWABLETRAPEZOIDALMAP_H
#define DRAWABLETRAPEZOIDALMAP_H

#include "data_structures/trapezoidalmap.h"
#include <cg3/viewer/interfaces/drawable_object.h>

/**
 * @brief Class to draw the trapezoids, copy & paste & edit from DrawableTrapezoidalMapDataset
 */
class DrawableTrapezoidalMap : public TrapezoidalMap,  public cg3::DrawableObject {

public:
    DrawableTrapezoidalMap();
    void draw() const;
    cg3::Point3d sceneCenter() const;
    double sceneRadius() const;

private:
    // cg3::BoundingBox2 boundingBox;

};

#endif // DRAWABLETRAPEZOIDALMAP_H
