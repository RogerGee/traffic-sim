// simulation.cpp - trafficsim
#include "simulation.h"
#include "opengl.h"
#include <iostream>
using namespace std;
using namespace trafficsim;

static const float DEFAULT_STEP_TIME = 2.5;
static const int DEFAULT_SPAWNRATE = 20;

simulation::simulation()
    : state(simul_state_stopped), stepTime(DEFAULT_STEP_TIME),
      steps(0), ticks(0), offset(0.0), spawnrate(DEFAULT_SPAWNRATE),
      elapsedTime(0.0), waitTime(0.0), numCars(0), waitCars(0),
      minWait(0.0), maxWait(0.0)
{

}
void simulation::start()
{
    if (state == simul_state_stopped) {
        // assume the simulation has already been reset
        //reset();
        state = simul_state_running;
    }
}
void simulation::pause(bool resume)
{
    if (state == simul_state_running && !resume) {
        state = simul_state_paused;
    }
    else if (state == simul_state_paused && resume) {
        state = simul_state_running;
    }
}
void simulation::stop()
{
    if (state == simul_state_paused || state == simul_state_running) {
        state = simul_state_stopped;
        reset();
    }
}
void simulation::set_param(simul_param kind,int value)
{
    switch (kind) {
    case simul_steptime_param:
        // 'value' is steps per minute
        stepTime = 60.0 / value;
        break;
    case simul_spawnrate_param:
        spawnrate = value;
        break;
    case simul_lightspeed_param:
        // TODO: tell intersection to adjust light duration

        break;
    }
}
int simulation::get_param(simul_param kind) const
{
    switch (kind) {
    case simul_steptime_param:
        // solve for 'value'
        return 1.0 / stepTime * 60.0;
    case simul_spawnrate_param:
        return spawnrate;
    case simul_lightspeed_param:

        return 0;
    }

    return -1;
}
float simulation::get_statistic(simul_statistic stat) const
{
    switch (stat) {
    case simul_stat_mean_wait_time:
        if (numCars == 0)
            return 0.0;
        return waitTime / numCars;
    case simul_stat_low_wait_time:
        return minWait;
    case simul_stat_high_wait_time:
        return maxWait;
    case simul_stat_mean_cars_waiting:
        if (elapsedTime == 0.0)
            return 0.0;
        return waitCars / elapsedTime;
    }

    return -1.0;
}
void simulation::tick(float tm)
{
    // a game tick has occurred; 'tm' is the amount of time that has occurred
    // since the last tick; update the tick counter (and possible the step
    // counter)
    if (tm > 0.0) {
        ticks += 1;

        // see if we have elapsed one step
        if (offset > 1.0) {
            steps += 1;
            ticks = 0;
            offset = 0.0;
        }
        else
            // update offset based on tick time
            offset += tm / stepTime;
    }
}
void simulation::step()
{
    // advance the simulation forward by one step

}
void simulation::reset()
{
    for (int i = 0; i < 4; i++)
        cars[i].clear();
    offset = 0.0;
    ticks = 0;
    steps = 0;
    stepTime = DEFAULT_STEP_TIME;
}
void simulation::render()
{
    //test
    // glBegin(GL_LINE_LOOP);
    // {
        // glVertex2f(-0.5+offset,-0.5);
        // glVertex2f(0.5,-0.5);
        // glVertex2f(0.5-offset,0.5);
        // glVertex2f(-0.5,0.5);
    // }
    // glEnd();
    glPushMatrix();
    intr.draw(offset);
    glPopMatrix();
}
