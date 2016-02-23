// opengl.h - trafficsim
#ifndef TRAFFICSIM_OPENGL_H
#define TRAFFICSIM_OPENGL_H

// include OpenGL platform headers
#ifdef TRAFFICSIM_GTKX
#include <GL/gl.h>
#include <GL/glu.h>

#elif defined(TRAFFICSIM_WIN32)
#include <gl/GL.h>
#include <gl/glu.h>

#endif

#endif
