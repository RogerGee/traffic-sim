// light.h - traffic-sim
#ifndef LIGHT_H
#define LIGHT_H

#include "types.h"

namespace trafficsim
{

    //yellow time period is predefined (for now at least)
    constexpr int YELLOW_TIME = 8;

    // light_state: enumerate light states
    enum light_state
    {
        light_state_green,
        light_state_yellow,
        light_state_red
    };

    // light: represents the light state machine
    class light
    {
    public:
        light(bool on = true);

        light_state get_state() const;
        void step();
        void draw(point pos, size sz, direction d);
        void update_rate(int greentime);
		int get_rate() const;

        static constexpr int DEFAULT_LIGHTSPEED()
        { return 10; }
    private:
        int step_counter;
        int green_time;

        light(const light&);
        light& operator =(const light&);
    };

} // trafficsim

#endif
