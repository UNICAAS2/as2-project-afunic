#include "trapezoid.h"

Trapezoid::Trapezoid(cg3::Point2d leftP, cg3::Segment2d top, cg3::Point2d rightP, cg3::Segment2d bottom)
{
    _leftP=leftP;
    _top=top;
    _rightP=rightP;
    _bottom=bottom;
}
