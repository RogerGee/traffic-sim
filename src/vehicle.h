// vehicle.h - traffic-sim
#ifndef VEHICLE_H
#define VEHICLE_H
#include "types.h"

namespace trafficsim
{

    class vehicle
    {
    public:
        vehicle();

        void translate(int cx,int cy);
    private:
        point pos;
        int legnth;

        vehicle(const vehicle&);
        vehicle& operator=(const vehicle&);
    };

} // trafficsim

#endif
