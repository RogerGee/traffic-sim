// window.h - trafficsim/win32
#ifndef WINDOW_H
#define WINDOW_H
#include "simulation.h"
#include "control.h" // gets Windows/GL system headers

namespace gui
{

    class window
    {
    public:
        static int run_application(int argc,char* argv[]);
    private:
        window();
        ~window();

        HWND hWnd;
        trafficsim::simulation sim;
        DrawArea drawArea;
        Label lblControlPanel;
        Button btnSimul, btnPause;
        TrackBar barSimulSpeed; Label b1;
        TrackBar barSpawnRate; Label b2;
        TrackBar barLightSpeed; Label b3;
        Label l1; Label lblTotalTime;
        Label l2; Label lblAvgWaitTime;
        Label l3; Label lblLowWaitTime;
        Label l4; Label lblHighWaitTime;
        Label l5; Label lblAvgWaitCars;
        Label l6; Label lblNumberOfCars;
        Label l7; Label lblTotalCars;

        bool update(float tout);
        bool message();
        void config();
        void onclick_simul(Control* btn);
        void onclick_pause(Control* btn);
        void ontrack_simulspeed(Control* trk);
        void ontrack_spawnrate(Control* trk);
        void ontrack_lightspeed(Control* trk);
        void sync_bars();
        void update_stats();
        void reset_stats();

        static int ref;
        static window* singleton;

        static VOID RegisterWndClass(HINSTANCE hInst);
        static LRESULT CALLBACK WindowProc(HWND,UINT,WPARAM,LPARAM);
    };

} // gui

#endif
