// types.h - trafficsim
#ifndef TYPES_H
#define TYPES_H

namespace trafficsim
{

    union point
    {
        int x;
        int y;
    };

    struct size
    {
        int width;
        int height;
    };

} // trafficsim

#endif
