// light.h - traffic-sim
#ifndef LIGHT_H
#define LIGHT_H

namespace trafficsim
{

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
        light();

        light_state get_state() const;
        void step();
    private:
        int step_counter;

        light(const light&);
        light& operator =(const light&);
    };

} // trafficsim

#endif
