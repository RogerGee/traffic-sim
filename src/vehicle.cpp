#include "vehicle.h"
#include "opengl.h"
#include <cmath>
using namespace std;
using namespace trafficsim;

vehicle::vehicle(point p, int l, color c) 
    : pos(p), prev(p), length(l), clr(c), tmWait(0.0), tmWaitLast(0.0),
      tmWaitCycle(0.0)
{
}
void vehicle::tick(float tm)
{
    if (is_waiting())
        tmWait += tm;
}
bool vehicle::step(direction d, intersection& i, vehicle* next)
{
    int* pp0, p0;
    bool stopped = is_waiting();
    prev = pos;
    switch (d)
    {
    case north:
        pp0 = &pos.y;
        p0 = *pp0;
        if (next && *pp0 > next->pos.y-next->length-2)
            return false;
        break;
    case east:
        pp0 = &pos.x;
        p0 = *pp0 + length;
        if (next && *pp0 > next->pos.x-length-2)
            return false;
        break;
    case south:
        pp0 = &pos.y;
        p0 = *pp0 - length;
        if (next && *pp0 < next->pos.y+length+2)
            return false;
        break;
    case west:
        pp0 = &pos.x;
        p0 = *pp0;
        if (next && *pp0 < next->pos.x+next->length+2)
            return false;
        break;
    }
    if (light* l = i.getlight(d, p0))
        if (l->get_state() != light_state_green)
            return false;
    *pp0 = (d == north || d == east) ? *pp0+1 : *pp0-1;
    if (stopped) { // we were stopped but are now moving
        tmWaitCycle = tmWait - tmWaitLast;
        tmWaitLast = tmWait;
    }
    return (abs(*pp0) > 50);
}
void vehicle::draw(direction d, float offs)
{
    float x = prev.x + (float(pos.x - prev.x) * offs);
    float y = prev.y + (float(pos.y - prev.y) * offs);
    glColor3f(clr.r,clr.g,clr.b);
    glBegin(GL_POLYGON);
    if (d == north || d == south)
    {
        glVertex2d(x+.5, y);
        glVertex2d(x+1.5, y);
        glVertex2d(x+1.5, y-length);
        glVertex2d(x+.5, y-length);
    }
    else
    {
        glVertex2d(x, y-.5);
        glVertex2d(x+length, y-.5);
        glVertex2d(x+length, y-1.5);
        glVertex2d(x, y-1.5);
    }
    glEnd();
}
