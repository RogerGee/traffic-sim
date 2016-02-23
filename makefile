################################################################################
# Makefile for 'traffic-sim' for Gtk/X #########################################
################################################################################
.PHONY: debug clean

ifeq ($(MAKECMDGOALS),debug)
MAKE_DEBUG = yes
endif

# options
SRC = src
GTKX = src/gtk-x
MACROS = TRAFFICSIM_GTKX
CFLAGS = -std=c++11
INCLUDE = -I$(SRC)
INCLUDE_GTK = $(shell pkg-config --cflags-only-I gtk+-3.0) -I$(GTKX)
LIBS = -lGL -lGLU `pkg-config --libs gtk+-3.0` -lX11
ifdef MAKE_DEBUG

PROGRAM = trafficsim-debug
OBJDIR = dobj
COMPILE = g++ -c -g $(CFLAGS) $(INCLUDE) $(addprefix -D,$(MACROS))
LINK = g++

else

PROGRAM = trafficsim
OBJDIR = obj
COMPILE = g++ -c -O3 $(CFLAGS) $(INCLUDE) $(addprefix -D,$(MACROS))
LINK = g++ -s

endif

# targets
TARGETS = trafficsim window simulation light intersection
OBJS = $(addprefix $(OBJDIR)/,$(addsuffix .o,$(TARGETS)))

# dependencies
TYPES_H = $(SRC)/types.h
OPENGL_H = $(SRC)/opengl.h
LIGHT_H = $(SRC)/light.h $(TYPES_H)
VEHICLE_H = $(SRC)/vehicle.h $(TYPES_H)
INTERSECTION_H = $(TYPES_H) $(VEHICLE_H) $(LIGHT_H)
SIMULATION_H = $(SRC)/simulation.h $(VEHICLE_H) $(INTERSECTION_H)
WINDOW_H = $(GTKX)/window.h $(SIMULATION_H) # all references must provide GTK includes

# rules
all: $(OBJDIR) $(PROGRAM)
debug: $(OBJDIR) $(PROGRAM)

$(PROGRAM): $(OBJS)
	$(LINK) -o$@ $(OBJS) $(LIBS)

$(OBJDIR)/trafficsim.o: $(SRC)/trafficsim.cpp $(WINDOW_H)
	$(COMPILE) $(INCLUDE_GTK) -o$@ $<
$(OBJDIR)/window.o: $(GTKX)/window.cpp $(WINDOW_H)
	$(COMPILE) $(INCLUDE_GTK) -o$@ $<
$(OBJDIR)/simulation.o: $(SRC)/simulation.cpp $(SIMULATION_H) $(OPENGL_H)
	$(COMPILE) -o$@ $<
$(OBJDIR)/light.o: $(SRC)/light.cpp $(LIGHT_H) $(OPENGL_H)
	$(COMPILE) -o$@ $<
$(OBJDIR)/intersection.o: $(SRC)/intersection.cpp $(INTERSECTION_H) $(OPENGL_H)
	$(COMPILE) -o$@ $<

$(OBJDIR):
	mkdir $(OBJDIR)
