// vehicle.h - traffic-sim
#ifndef VEHICLE_H
#define VEHICLE_H
#include "types.h"
#include "intersection.h"
#include "light.h"

namespace trafficsim
{

    class intersection;
    class light;

    class vehicle
    {
    public:
        vehicle(point p, int l);
        void tick(float tm);
        bool step(direction d, intersection& i, vehicle* next);
        void draw(direction d, float offs);
        bool is_waiting() const { return pos == prev; }
        float get_wait_time() const { return tmWait; }
        float get_cycle_wait_time() const { return tmWaitCycle; }
    private:
        point pos;
        point prev;
        int length;
        float tmWait, tmWaitLast;
        float tmWaitCycle;
    };

} // trafficsim

#endif
