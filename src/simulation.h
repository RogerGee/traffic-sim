// simulation.h - traffic-sim
#ifndef SIMULATION_H
#define SIMULATION_H
#include "vehicle.h"
#include <vector>

namespace trafficsim
{

    enum simul_param
    {
        simul_steps_param, // number of steps in total simulation
        simul_spawnrate_param, // number of steps new cars added to simulation
        simul_lightstep_param // number of steps for red/green light duration
    };

    class simulation {
    public:
        simulation();
        void set_param(simul_param kind,int value);
        void step();
        bool is_running() const { return true; }
        void render();
    private:
        //cars
        std::vector<vehicle> cars;
        //parameters
        int steps;
        int curstep;
        int spawnrate;
    };

} // trafficsim

#endif
