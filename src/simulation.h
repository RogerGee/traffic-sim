// simulation.h - traffic-sim
#ifndef SIMULATION_H
#define SIMULATION_H
#include "vehicle.h"
#include "intersection.h"
#include <deque>

namespace trafficsim
{

    enum simul_param
    {
        simul_steptime_param, // number of seconds for each step
        simul_spawnrate_param, // number of steps new cars added to simulation
        simul_lightspeed_param // number of steps for red/green light duration
    };

    enum simul_statistic
    {
        simul_stat_total_time,
        simul_stat_mean_wait_time,
        simul_stat_low_wait_time,
        simul_stat_high_wait_time,
        simul_stat_mean_cars_waiting,
        simul_stat_number_of_cars,
        simul_stat_total_number_of_cars
    };

    enum simul_state
    {
        simul_state_running,
        simul_state_paused,
        simul_state_stopped
    };

    class simulation
    {
    public:
        simulation();

        void start();
        void pause(bool resume = false);
        void stop();
        void tick(float tm);
        void set_param(simul_param kind,int value);
        void render();

        int get_param(simul_param kind) const;
        float get_statistic(simul_statistic stat) const;
        bool is_running() const
        { return state == simul_state_running; }
        simul_state get_state() const
        { return state; }
    private:
        std::deque<vehicle> cars[4]; //4 queues of cars, one queue for each lane
        intersection intr; // one intersection (for now)
        simul_state state; // state of simulation
        float stepTime; // how many seconds for each step?
        int steps; // number of steps into simulation
        int ticks; // number of ticks in simulation
        float offset; // fractional offset for animation
        int spawnrate; // car spawn rate (in steps)

        float elapsedTime; // total elapsed time
        float avgWaitTime; // current average wait time
        float minWait; // shortest wait time seen for any car
        float maxWait; // longest wait time seen for any car
        int numCars; // total cars having been in simulation (updated per step)
        int waitCars; // cars having waited (updated per step)
        int curNumCars; // current number of cars in simulation

        void step();
        void addcar(direction d, int l);
        void stats_eval(float tm);
        void reset();
    };

} // trafficsim

#endif
