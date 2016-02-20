// intersection.h - traffic-sim
#ifndef INTERSECTION_H
#define INTERSECTION_H
#include <vector>
#include "types.h"
#include "light.h"

namespace trafficsim
{

    class intersection {
    public:
        bool checkspace(int x, int y);
        light* getlight(int x, int y);
        void move(vehicle* v, int x, int y);
    private:
        //location and size of intersection
        point loc;
        size sz;
        //sorted lists of vehicle pointers
        std::vector<vehicle*> xaxis;
        std::vector<vehicle*> yaxis;
        //lights
        light ns_light;
        light ew_light;
    };

} // trafficsim

#endif
