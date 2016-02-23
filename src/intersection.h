// intersection.h - traffic-sim
#ifndef INTERSECTION_H
#define INTERSECTION_H
#include <vector>
#include "types.h"
#include "vehicle.h"
#include "light.h"

namespace trafficsim
{

    class intersection {
    public:
		intersection();
		void draw(float delta);
        light* getlight(direction d, int p);
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
