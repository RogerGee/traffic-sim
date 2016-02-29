#include "intersection.h"
#include "simulation.h"
#include "opengl.h"
using namespace trafficsim;

intersection::intersection() : ns_light(), ew_light(false)
{
    loc.x=0; loc.y=0; sz.width=4; sz.height=4;
}

void intersection::step()
{
    ns_light.step();
    ew_light.step();
}

void intersection::draw()
{
    //set up projection
    gluOrtho2D(loc.x-20, loc.x+20+sz.width, loc.y-20-sz.height, loc.y+20);
    //draw the streets
    glColor3f(0,0,0);
    glBegin(GL_POLYGON);
        glVertex2d(loc.x, loc.y);
        glVertex2d(loc.x, loc.y+20);
        glVertex2d(loc.x+sz.width, loc.y+20);
        glVertex2d(loc.x+sz.width, loc.y);
        glVertex2d(loc.x+sz.width+20, loc.y);
        glVertex2d(loc.x+sz.width+20, loc.y-sz.height);
        glVertex2d(loc.x+sz.width, loc.y-sz.height);
        glVertex2d(loc.x+sz.width, loc.y-sz.height-20);
        glVertex2d(loc.x, loc.y-sz.height-20);
        glVertex2d(loc.x, loc.y-sz.height);
        glVertex2d(loc.x-20, loc.y-sz.height);
        glVertex2d(loc.x-20, loc.y);
    glEnd();
    //draw a dashed line down the middle of each street
    glColor3f(.9,.9,0);
    glLineStipple(1, 0xf0f0);
    glEnable(GL_LINE_STIPPLE);
    glBegin(GL_LINES);
        glVertex2d(loc.x+(sz.width/2), loc.y+20);
        glVertex2d(loc.x+(sz.width/2), loc.y-sz.height-20);
        glVertex2d(loc.x-20, loc.y-(sz.height/2));
        glVertex2d(loc.x+sz.width+20, loc.y-(sz.height/2));
    glEnd();
    glDisable(GL_LINE_STIPPLE);
    //tell lights to draw
    ns_light.draw({loc.x+sz.width/2,loc.y-sz.height/2}, sz, north);
    ew_light.draw({loc.x+sz.width/2,loc.y-sz.height/2}, sz, east);
	ns_light.draw({loc.x+sz.width/2,loc.y-sz.height/2}, sz, south);
    ew_light.draw({loc.x+sz.width/2,loc.y-sz.height/2}, sz, west);
}

bool intersection::isoutside(direction d, int p)
{
	switch (d)
	{
	case north:
		return p > loc.y + 20;
	case south:
		return p < loc.y - sz.height - 20;
	case east:
		return p > loc.x + sz.width + 20;
	case west:
		return p < loc.x - 20;
	}
}

light* intersection::getlight(direction d, int p)
{
    //p will be either x or y coord, whichever is relevant for the direction
    switch (d)
    {
    case north:
        if (p == loc.y - sz.height)
            return &ns_light;
        break;
    case south:
        if (p == loc.y)
            return &ns_light;
        break;
    case east:
        if (p == loc.x)
            return &ew_light;
        break;
    case west:
        if (p == loc.x + sz.width)
            return &ew_light;
        break;
    }
    //traffic light has either been passed, or been not yet reached;
    //therefore, it is not applicable
    return NULL;
}

