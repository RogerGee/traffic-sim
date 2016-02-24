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
        bool step(direction d, intersection& i, vehicle* next);
        void draw(direction d, float offs);
    private:
        point pos;
        point prev;
        int length;
    };

} // trafficsim

#endif
