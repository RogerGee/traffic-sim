// light.cpp - trafficsim
#include "light.h"
#include "opengl.h"
using namespace trafficsim;

light::light(bool on) : green_time(DEFAULT_LIGHTSPEED()), step_counter(0)
{
    if (!on)
        step_counter = green_time + YELLOW_TIME;
}

light_state light::get_state() const
{
    if (step_counter < green_time)
        return light_state_green;
    if (step_counter < green_time + YELLOW_TIME)
        return light_state_yellow;
    return light_state_red;
}

void light::step()
{
    step_counter = (step_counter + 1) % (green_time*2 + YELLOW_TIME*2);
}

void light::draw(point pos, size sz, direction d)
{
    glPushMatrix();
	glTranslated(pos.x, pos.y, 0);
	glRotated(90 * int(d),0,0,1);
	glColor3f(.4,.4,.4);
    glBegin(GL_POLYGON);
        glVertex2d(sz.width/2, sz.height/2+3);
        glVertex2d(sz.width/2+1, sz.height/2+3);
        glVertex2d(sz.width/2+1, sz.height/2);
        glVertex2d(sz.width/2, sz.height/2);
    glEnd();
	double y = 0;
    switch(get_state())
    {
    case light_state_green:
        glColor3f(0,0.8,0);
		y = 0;
        break;
    case light_state_yellow:
        glColor3f(1,1,0);
		y = 1;
        break;
    case light_state_red:
        glColor3f(1,0,0);
		y = 2;
        break;
    }
    glBegin(GL_POLYGON);
    {
        glVertex2d(sz.width/2+.1, sz.height/2+2.9-y);
        glVertex2d(sz.width/2+.9, sz.height/2+2.9-y);
        glVertex2d(sz.width/2+.9, sz.height/2+2.1-y);
        glVertex2d(sz.width/2+.1, sz.height/2+2.1-y);
    }
    glEnd();
	glPopMatrix();
}

void light::update_rate(int greentime)
{
    if (step_counter >= green_time + YELLOW_TIME)
    {
        int step = step_counter - (green_time+YELLOW_TIME);
        step_counter = (greentime+YELLOW_TIME+step) % (greentime*2+YELLOW_TIME*2);
    }
    green_time = greentime;
}

int light::get_rate() const
{
	return green_time;
}

