// types.cpp - trafficsim
#include "types.h"
using namespace trafficsim;

bool trafficsim::operator==(const point& a,const point& b)
{
    return a.x == b.x && a.y == b.y;
}
bool trafficsim::operator!=(const point& a,const point& b)
{
    return a.x != b.x || a.y != b.y;
}
