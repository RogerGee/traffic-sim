// trafficsim.cpp - entry point to application
#include "window.h"

int main(int argc,const char* argv[])
{
    // launch the gui application
    return gui::window::run_application(argc,const_cast<char**>(argv));
}
