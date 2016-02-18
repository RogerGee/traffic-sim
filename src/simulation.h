// simulation.h - traffic-sim
#ifndef SIMULATION_H
#define SIMULATION_H
#include <vector>

namespace traffic_sim
{

    class simulation {
    public:
        simulation(int steps, int spawnrate, int lightrate);
        void step();
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
