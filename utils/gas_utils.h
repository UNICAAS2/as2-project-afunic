#ifndef GAS_UTILS_H
#define GAS_UTILS_H

#include <cg3/geometry/segment2.h>

/**
 * some small utility functions...
 */
namespace GasUtils {

double getIntersectionYCoord(double xSlab, cg3::Segment2d segment);
double pointDirection(cg3::Point2d p, cg3::Segment2d s);
bool isAbove(cg3::Point2d p, cg3::Point2d* sec_point, cg3::Segment2d s);
bool areTheSame(double f1, double f2);
bool areTheSame(cg3::Point2d p1, cg3::Point2d p2);
bool areEqual(cg3::Point2d p1, cg3::Point2d p2);
bool areEqual(cg3::Segment2d s1, cg3::Segment2d s2);

}

#endif // GAS_UTILS_H
