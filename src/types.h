// types.h - trafficsim
#ifndef TYPES_H
#define TYPES_H

namespace trafficsim
{

    struct point
    {
        int x;
        int y;
    };

    struct size
    {
        int width;
        int height;
    };

    enum direction
    {
        north,
        east,
        south,
        west
    };

} // trafficsim

#endif
