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
    bool operator==(const point&,const point&);
    bool operator!=(const point&,const point&);

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

	struct color {
		float r, g, b;
	};
	
} // trafficsim

#endif
