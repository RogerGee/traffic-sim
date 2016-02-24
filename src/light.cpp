// light.cpp - trafficsim
#include "light.h"
#include "opengl.h"
using namespace trafficsim;

light::light(int greentime, bool on) : green_time(greentime), step_counter(0)
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

void light::draw(point pos, bool vert)
{
    glColor3f(.4,.4,.4);
    glBegin(GL_POLYGON);
        glVertex2d(pos.x, pos.y);
        glVertex2d(pos.x+1, pos.y);
        glVertex2d(pos.x+1, pos.y-1);
        glVertex2d(pos.x, pos.y-1);
    glEnd();
    switch(get_state())
    {
    case light_state_green:
        glColor3f(0,1,0);
        break;
    case light_state_yellow:
        glColor3f(1,1,0);
        break;
    case light_state_red:
        glColor3f(1,0,0);
        break;
    }
    glBegin(GL_POLYGON);
        glVertex2d(pos.x+.1, pos.y-.1);
        glVertex2d(pos.x+.9, pos.y-.1);
        glVertex2d(pos.x+.9, pos.y-.9);
        glVertex2d(pos.x+.1, pos.y-.9);
    glEnd();
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

