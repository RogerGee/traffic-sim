// intersection.h - traffic-sim
#ifndef INTERSECTION_H
#define INTERSECTION_H
#include <vector>
#include "types.h"
#include "light.h"

namespace trafficsim
{

    class simulation;

    class intersection {
    public:
        intersection();
        void step();
        void draw();
        light* getlight(direction d, int p);
        friend class simulation;
    private:
        //location and size of intersection
        point loc;
        size sz;
        //lights
        light ns_light;
        light ew_light;
    };

} // trafficsim

#endif
