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

void draw(point pos, bool vert)
{
    
}
