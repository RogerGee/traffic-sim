// window.cpp - trafficsim/win32
#include "window.h"
#include <iostream>
#include <stdexcept>
#define WNDCLASS_NAME "IHateBeingStuckInTraffic"
using namespace std;
using namespace gui;
using namespace trafficsim;

/*static*/ int window::run_application(int argc,char* argv[])
{
	window win;
	const DWORD ms = 1000/60;

	ShowWindow(win.hWnd,SW_SHOWNORMAL);

	while (win.update(1.0 / 60.0)) {
		Sleep(ms);
	}

	return 0;
}

/*static*/ int window::ref = 0;
/*static*/ window* window::singleton = NULL;
window::window()
	: drawArea(sim), lblControlPanel("Control Panel"), btnSimul("Start",this,&window::onclick_simul),
		btnPause("Pause",this,&window::onclick_pause)
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
				"Traffic Simulator :)",
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
	int w, h, r, s, t, u;
	GetClientRect(hWnd,&rect);
	w = rect.right - rect.left;
	h = rect.bottom - rect.top;

	// make the draw area occupy 3/4 of the client width
	w *= 3.0/4;
	drawArea.change(0,0,w,h);

	w += 5;
	lblControlPanel.change(w,0); lblControlPanel.get_size(r,s);
	s += 5;
	
	btnSimul.change(w,s); btnSimul.get_size(r,t);
	btnPause.change(w+r+5,s);
}
void window::onclick_simul(Control* btn)
{
	if (sim.get_state() == simul_state_running || sim.get_state() == simul_state_paused) {
		// reset everything
		sim.stop();
		btn->set_text("Start");
		btnPause.set_text("Pause");
		btnPause.enable(false);
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
    wc.style = 0;
    if (!RegisterClass(&wc))
        throw runtime_error("fail RegisterClass()");
}
/*static*/ LRESULT CALLBACK window::WindowProc(HWND hWnd,UINT msg,
										WPARAM wParam,LPARAM lParam)
{
	switch (msg) {
	case WM_CREATE:
		// create child controls
		singleton->drawArea.create(hWnd);
		singleton->lblControlPanel.create(hWnd);
		singleton->btnSimul.create(hWnd);
		singleton->btnPause.create(hWnd);
		singleton->btnPause.enable(false);
		break;

	case WM_CLOSE:
		PostQuitMessage(0);
		break;

	case WM_COMMAND:
		Control::wm_command((HWND)lParam,wParam);
		break;

	case WM_KEYUP:

		break;

	case WM_SIZE:
		singleton->config();
		break;

	default:
		return DefWindowProc(hWnd,msg,wParam,lParam);
	}

	return 0;
}
