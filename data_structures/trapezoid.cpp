#include "trapezoid.h"

Trapezoid::Trapezoid(cg3::Point2d leftP, cg3::Segment2d top, cg3::Point2d rightP, cg3::Segment2d bottom) {
    _leftP=leftP;
    _top=top;
    _rightP=rightP;
    _bottom=bottom;
    _node=SIZE_MAX; // like nullptr, SIZE_MAX => null value
}

void Trapezoid::setNeighbors(size_t tl, size_t tr, size_t br, size_t bl) {
    _tln=tl;
    _trn=tr;
    _brn=br;
    _bln=bl;
}
