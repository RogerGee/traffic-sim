// simulation.cpp - trafficsim
#include "simulation.h"
using namespace std;
using namespace trafficsim;

static const int DEFAULT_STEPS = 200;
static const int DEFAULT_SPAWNRATE = 20;

simulation::simulation()
    : steps(DEFAULT_STEPS), curstep(0), spawnrate(DEFAULT_SPAWNRATE)
{

}
void simulation::set_param(simul_param kind,int value)
{
    switch (kind) {
    case simul_steps_param:
        steps = value;
        break;
    case simul_spawnrate_param:
        spawnrate = value;
        break;
    case simul_lightstep_param:
        // TODO: tell intersection to adjust light duration

        break;
    }
}
void simulation::step()
{

}
