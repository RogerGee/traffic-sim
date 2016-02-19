// window.cpp - trafficsim/win32
#include "window.h"
#include <stdexcept>
#define EDGE_PADDING 10
#define WNDCLASS_NAME "IHateBeingStuckInTraffic"
using namespace std;
using namespace gui;
using namespace trafficsim;

/*static*/ int window::run_application(int argc,char* argv[])
{
	window win;
	const DWORD ms = 1000/60;
	
	ShowWindow(win.hWnd,SW_SHOWNORMAL);

	while (win.update()) {
		Sleep(ms);
	}
	
	return 0;
}

/*static*/ int window::ref = 0;
/*static*/ window* window::singleton = NULL;
window::window()
{
	RECT winrect;
	DWORD dwStyle;
	HINSTANCE hInst;
	int pixelFormat;
	PIXELFORMATDESCRIPTOR pfd = {
        sizeof(PIXELFORMATDESCRIPTOR),
        1,
        PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
        PFD_TYPE_RGBA,
        32,
        0, 0, 0, 0, 0, 0,
        0,
        0,
        0,
        0, 0, 0, 0,
        24,
        8,
        0,
        PFD_MAIN_PLANE,
        0,
        0, 0, 0
    };

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
		
	// setup device context
	hDC = GetDC(hWnd);
	pixelFormat = ChoosePixelFormat(hDC,&pfd);
	if (!pixelFormat)
		throw runtime_error("fail ChoosePixelFormat()");
	if (!SetPixelFormat(hDC,pixelFormat,&pfd))
		throw runtime_error("fail SetPixelFormat()");
	
	// create OpenGL context
	hContext = wglCreateContext(hDC);
	if (!hContext)
		throw runtime_error("fail wglCreateContext()");
}
window::~window()
{
	wglDeleteContext(hContext);
	ReleaseDC(hWnd,hDC);
	DestroyWindow(hWnd);
	if (--ref <= 0) {
		UnregisterClass(WNDCLASS_NAME,GetModuleHandle(NULL));
		singleton = NULL;
	}
}
bool window::update()
{
	if (!message())
		return false;
	
	render();
	
	return true;
}
bool window::message()
{
	MSG msg;
	if (PeekMessage(&msg,NULL,0,0,PM_REMOVE)) {
		if (msg.message == WM_QUIT)
			return false;
	
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return true;
}
void window::render()
{
	if (!sim.is_running())
		return;

	wglMakeCurrent(hDC,hContext);
	glClear(GL_COLOR_BUFFER_BIT);
	
	// calls to rendering here
    sim.render();
	
	SwapBuffers(hDC);
}
void window::config()
{
	RECT bounds;
	int w, h;

	GetClientRect(hWnd,&bounds);
	w = bounds.right - bounds.left;
	h = bounds.bottom - bounds.top;

	wglMakeCurrent(hDC,hContext);
	glViewport(EDGE_PADDING,EDGE_PADDING,w-EDGE_PADDING*2,h-EDGE_PADDING*2);
	glClearColor(1.0,1.0,1.0,1.0);
	glColor3f(0.0,0.0,0.0);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glScaled(double(h)/w,1.0,1.0);
}
/*static*/ VOID window::RegisterWndClass(HINSTANCE hInst)
{
	WNDCLASS wc;
	wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hbrBackground = NULL;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hIcon = NULL; /* TODO: load a beautiful icon */
    wc.hInstance = hInst;
    wc.lpfnWndProc = window::WindowProc;
    wc.lpszClassName = WNDCLASS_NAME;
    wc.lpszMenuName = NULL; 
    wc.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
    if (!RegisterClass(&wc))
        throw runtime_error("fail RegisterClass()");
}
/*static*/ LRESULT CALLBACK window::WindowProc(HWND hWnd,UINT msg,
										WPARAM wParam,LPARAM lParam)
{
	switch (msg) {
	case WM_CREATE:
		
		break;

	case WM_CLOSE:
		PostQuitMessage(0);
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
