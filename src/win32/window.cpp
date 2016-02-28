// window.cpp - trafficsim/win32
#include "window.h"
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <chrono>
#include <thread>
#include <array>
#define WNDCLASS_NAME "IHateBeingStuckInTraffic"
using namespace std;
using namespace gui;
using namespace trafficsim;

/*static*/ int window::run_application(int argc,char* argv[])
{
    using namespace chrono;
    window win;

    InitCommonControls(); // XP doesn't support InitCommonControlsEx

    ShowWindow(win.hWnd,SW_SHOWNORMAL);

    steady_clock::time_point prev_t = steady_clock::now();
    steady_clock::time_point cur_t = steady_clock::now();

    while (win.update(duration_cast<duration<double>>(cur_t-prev_t).count())) {
        this_thread::sleep_for(milliseconds(4));
        prev_t = cur_t;
        cur_t = steady_clock::now();
    }

    return 0;
}

/*static*/ int window::ref = 0;
/*static*/ window* window::singleton = NULL;
window::window()
    : drawArea(sim), lblControlPanel("Control Panel"), btnSimul("Start",this,&window::onclick_simul),
        btnPause("Pause",this,&window::onclick_pause), barSimulSpeed(this,&window::ontrack_simulspeed,50,400),
        b1("Simulation Speed:"), barSpawnRate(this,&window::ontrack_spawnrate,1,20), b2("Spawn Rate:"),
        barLightSpeed(this,&window::ontrack_lightspeed,10,20), b3("Light Speed:"), l1("Total Elapsed Time:"),
        lblTotalTime("0.0"), l2("Mean Wait Time"), lblAvgWaitTime("0.0"), l3("Low Wait Time"), lblLowWaitTime("0.0"),
        l4("High Wait Time:"), lblHighWaitTime("0.0"), l5("Mean Cars Waiting:"), lblAvgWaitCars("0.0"),
        l6("Number of Cars:"), lblNumberOfCars("0"), l7("Total Cars Generated:"), lblTotalCars("0")
{
    RECT winrect;
    DWORD dwStyle;
    HINSTANCE hInst;

    // make sure that this is the only window instance we produce
    if (ref++ > 0) {
        throw runtime_error("window must be a singleton");
    }
    singleton = this;

    hInst = GetModuleHandle(NULL);
    RegisterWndClass(hInst);

    // adjust desired rectangle for actual needed size
    winrect.left = 0;
    winrect.right = 800;
    winrect.top = 0;
    winrect.bottom = 600;
    dwStyle = WS_OVERLAPPEDWINDOW;
    AdjustWindowRectEx(&winrect,dwStyle,FALSE,WS_EX_CLIENTEDGE);

    // create the window
    hWnd = CreateWindowEx(
                WS_EX_CLIENTEDGE,
                WNDCLASS_NAME,
                "Traffic Simulator",
                dwStyle,
                CW_USEDEFAULT,
                CW_USEDEFAULT,
                winrect.right - winrect.left,
                winrect.bottom - winrect.top,
                NULL,
                NULL,
                hInst,
                NULL );
    if (!hWnd)
        throw runtime_error("fail CreateWindowEx()");

    // load a font that doesn't look like garbage
    HFONT hFont;
    NONCLIENTMETRICS ncm;
    ncm.cbSize = sizeof(NONCLIENTMETRICS);
    SystemParametersInfo(SPI_GETNONCLIENTMETRICS,sizeof(NONCLIENTMETRICS),&ncm,0);
    hFont = CreateFontIndirect(&ncm.lfMessageFont);
    SendMessage(hWnd,WM_SETFONT,(WPARAM)hFont,MAKELPARAM(TRUE, 0));
}
window::~window()
{
    DestroyWindow(hWnd);
    if (--ref <= 0) {
        UnregisterClass(WNDCLASS_NAME,GetModuleHandle(NULL));
        singleton = NULL;
    }
}
bool window::update(float tout)
{
    if (!message())
        return false;

    if (sim.is_running()) {
        sim.tick(tout);
        drawArea.render();
        update_stats();
    }

    return true;
}
bool window::message()
{
    // peek and process messages for any window
    MSG msg;
    if (PeekMessage(&msg,NULL,0,0,PM_REMOVE)) {
        if (msg.message == WM_QUIT)
            return false;

        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return true;
}
void window::config()
{
    RECT rect;
    int W, w, h, r, s, t, u;
    GetClientRect(hWnd,&rect);
    w = rect.right - rect.left;
    h = rect.bottom - rect.top;

    // make the draw area occupy 3/4 of the client width
    W = w;
    w *= 3.0/4;
    W -= w;
    drawArea.change(0,0,w,h);

    w += 5;
    lblControlPanel.change(w,0); lblControlPanel.get_size(r,s);
    u = s;
    s += 5;

    btnSimul.change(w,s); btnSimul.get_size(r,t);
    btnPause.change(w+r+5,s);
    s += t+15;

    b1.change(w,s,W,t);
    s += u;
    barSimulSpeed.change(w,s,W,t*2);
    s += t*2;

    b2.change(w,s,W,t);
    s += u;
    barSpawnRate.change(w,s,W,t*2);
    s += t*2;

    b3.change(w,s,W,t);
    s += u;
    barLightSpeed.change(w,s,W,t*2);
    s += t*2;

    l1.change(w,s); l7.get_size(u,t); // l7 has longest text
    lblTotalTime.change(w+u,s);
    s += t;
    l2.change(w,s);
    lblAvgWaitTime.change(w+u,s);
    s += t;
    l3.change(w,s);
    lblLowWaitTime.change(w+u,s);
    s += t;
    l4.change(w,s);
    lblHighWaitTime.change(w+u,s);
    s += t;
    l5.change(w,s);
    lblAvgWaitCars.change(w+u,s);
    s += t;
    l6.change(w,s);
    lblNumberOfCars.change(w+u,s);
    s += t;
    l7.change(w,s);
    lblTotalCars.change(w+u,s);
}
void window::onclick_simul(Control* btn)
{
    if (sim.get_state() == simul_state_running || sim.get_state() == simul_state_paused) {
        // reset everything
        sim.stop();
        btn->set_text("Start");
        btnPause.set_text("Pause");
        btnPause.enable(false);
        reset_stats();
        sync_bars();
    }
    else if (sim.get_state() == simul_state_stopped) {
        sim.start();
        btn->set_text("Stop");
        btnPause.enable(true);
    }
    drawArea.render();
}
void window::onclick_pause(Control* btn)
{
    if (sim.get_state() == simul_state_running) {
        sim.pause();
        btn->set_text("Resume");
    }
    else if (sim.get_state() == simul_state_paused) {
        sim.pause(true);
        btn->set_text("Pause");
    }
    drawArea.render();
}
void window::ontrack_simulspeed(Control* trk)
{
    int v = barSimulSpeed.get_value();
    sim.set_param(simul_steptime_param,v);
}
void window::ontrack_spawnrate(Control* trk)
{
    int v = barSpawnRate.get_value();
    sim.set_param(simul_spawnrate_param,v);
}
void window::ontrack_lightspeed(Control* trk)
{
    int v = barLightSpeed.get_value();
    sim.set_param(simul_lightspeed_param,v);
}
void window::sync_bars()
{
    int steptime, spawnrate, lightspeed;
    steptime = sim.get_param(simul_steptime_param);
    spawnrate = sim.get_param(simul_spawnrate_param);
    lightspeed = sim.get_param(simul_lightspeed_param);

    barSimulSpeed.set_value(steptime);
    barSpawnRate.set_value(spawnrate);
    barLightSpeed.set_value(lightspeed);
}
void window::update_stats()
{
    stringstream ss;
    ss.precision(4); ss.flags(ios_base::fixed);
    ss << sim.get_statistic(simul_stat_total_time);
    lblTotalTime.set_text(ss.str().c_str());
    ss.str(""); ss << sim.get_statistic(simul_stat_mean_wait_time);
    lblAvgWaitTime.set_text(ss.str().c_str());
    ss.str(""); ss << sim.get_statistic(simul_stat_low_wait_time);
    lblLowWaitTime.set_text(ss.str().c_str());
    ss.str(""); ss << sim.get_statistic(simul_stat_high_wait_time);
    lblHighWaitTime.set_text(ss.str().c_str());
    ss.str(""); ss << sim.get_statistic(simul_stat_mean_cars_waiting);
    lblAvgWaitCars.set_text(ss.str().c_str());
    ss.str(""); ss << int(sim.get_statistic(simul_stat_number_of_cars));
    lblNumberOfCars.set_text(ss.str().c_str());
    ss.str(""); ss << int(sim.get_statistic(simul_stat_total_number_of_cars));
    lblTotalCars.set_text(ss.str().c_str());
}
void window::reset_stats()
{
    string def;
    stringstream ss;
    ss.precision(4); ss.flags(ios_base::fixed);
    ss << float(0);
    def = ss.str();
    lblTotalTime.set_text(def.c_str());
    lblAvgWaitTime.set_text(def.c_str());
    lblLowWaitTime.set_text(def.c_str());
    lblHighWaitTime.set_text(def.c_str());
    lblAvgWaitCars.set_text(def.c_str());
    lblNumberOfCars.set_text(def.c_str());
    lblTotalCars.set_text(def.c_str());
}
/*static*/ VOID window::RegisterWndClass(HINSTANCE hInst)
{
    WNDCLASS wc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hbrBackground = (HBRUSH)GetSysColorBrush(COLOR_BTNFACE);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hIcon = NULL; /* TODO: load a beautiful icon */
    wc.hInstance = hInst;
    wc.lpfnWndProc = window::WindowProc;
    wc.lpszClassName = WNDCLASS_NAME;
    wc.lpszMenuName = NULL;
    wc.style = CS_HREDRAW | CS_VREDRAW;
    if (!RegisterClass(&wc))
        throw runtime_error("fail RegisterClass()");
}
/*static*/ LRESULT CALLBACK window::WindowProc(HWND hWnd,UINT msg,
                                        WPARAM wParam,LPARAM lParam)
{
    static const array<Control*,24> a = {
        &singleton->drawArea, &singleton->lblControlPanel, &singleton->btnSimul,
        &singleton->btnPause, &singleton->barSimulSpeed, &singleton->b1,
        &singleton->barSpawnRate, &singleton->b2, &singleton->barLightSpeed,
        &singleton->b3, &singleton->l1, &singleton->lblTotalTime, &singleton->l2,
        &singleton->lblAvgWaitTime, &singleton->l3, &singleton->lblLowWaitTime,
        &singleton->l4, &singleton->lblHighWaitTime, &singleton->l5,
        &singleton->lblAvgWaitCars, &singleton->l6, &singleton->lblNumberOfCars,
        &singleton->l7, &singleton->lblTotalCars
    };

    switch (msg) {
    case WM_CREATE:
        // create child controls and set initial settings
        for (Control* ctrl : a)
            ctrl->create(hWnd);
        singleton->btnPause.enable(false);
        singleton->sync_bars();
        break;

    case WM_CLOSE:
        PostQuitMessage(0);
        break;

    case WM_COMMAND:
    case WM_HSCROLL: // for trackbars
        Control::wm_command((HWND)lParam,wParam);
        break;

    // case WM_KEYUP:

        // break;

    case WM_SIZE:
        singleton->config();
        break;

    case WM_GETMINMAXINFO:
    {
        MINMAXINFO* mmi = (MINMAXINFO*)lParam;
        mmi->ptMinTrackSize.x = 800;
        mmi->ptMinTrackSize.y = 600;
        break;
    }

    case WM_SETFONT:
        for (Control* thing : a) {
            char buffer[128];
            thing->get_text(buffer,sizeof(buffer));
            thing->send_message(WM_SETFONT,wParam,lParam);
            thing->set_text(buffer);
        }
        singleton->config();
        break;

    default:
        return DefWindowProc(hWnd,msg,wParam,lParam);
    }

    return 0;
}
