// control.cpp - trafficsim/win32
#include "control.h"
#include <iostream>
using namespace std;
using namespace gui;
using namespace trafficsim;

static std::string GetLastErrorAsString()
{
    //Get the error message, if any.
    DWORD errorMessageID = ::GetLastError();
    if(errorMessageID == 0)
        return std::string(); //No error message has been recorded

    LPSTR messageBuffer = nullptr;
    size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                                 NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

    std::string message(messageBuffer, size);

    //Free the buffer.
    LocalFree(messageBuffer);

    return message;
}

/*static*/ unordered_map<HWND,Control*> Control::inst;
Control::Control()
	: hWnd(NULL)
{
}
Control::~Control()
{
	if (hWnd) {
		inst.erase(hWnd);
		DestroyWindow(hWnd);
	}
}
void Control::change(int x,int y,int w,int h)
{
	SetWindowPos(
		hWnd,
		HWND_TOP, // masking this out
		x, y,
		w, h,
		SWP_NOZORDER );
}
/*static*/ void Control::add_handle(HWND handle,Control* obj)
{
	obj->hWnd = handle;
	inst.emplace(handle,obj);
}
/*static*/ Control* Control::lookup(HWND handle)
{
	auto it = inst.find(handle);
	if (it == inst.end())
		return NULL;
	return it->second;
}
void Control::create(HWND hParent)
{
	HWND hControl;
	if (hWnd != NULL)
		throw runtime_error("Child window is already created");

	hControl = oncreate(hParent);
	add_handle(hControl,this);
}

// DrawArea

/*static*/ int DrawArea::regCnt = 0;
DrawArea::DrawArea(simulation& simul)
	: hDC(NULL), hContext(NULL), sim(simul)
{
}
DrawArea::~DrawArea()
{
	wglDeleteContext(hContext);
	ReleaseDC(get_hwnd(),hDC);

	if (--regCnt <= 0)
		UnregisterClass("DrawArea",GetModuleHandle(NULL));
}
void DrawArea::render()
{
	wglMakeCurrent(hDC,hContext);
	glClear(GL_COLOR_BUFFER_BIT);
	
	// calls to rendering here
	sim.render();

	SwapBuffers(hDC);
	
	cout << "render" << endl;
}
HWND DrawArea::oncreate(HWND hParent)
{
	HWND hResult;
	RegisterWndClass();

	// create the window
	hResult = CreateWindow(
				"DrawArea",
				"",
				WS_CHILD | WS_VISIBLE,
				0, 0, 300, 200,
				hParent,
				NULL,
				GetModuleHandle(NULL),
				this );
	if (!hResult)
		throw runtime_error("fail CreateWindowEx()");

	return hResult;
}
void DrawArea::create_context()
{
	int pixelFormat;
	static PIXELFORMATDESCRIPTOR pfd = {
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

	if (hContext != NULL)
		throw runtime_error("Context is already created");

	// setup device context
	hDC = GetDC(get_hwnd());
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
void DrawArea::config()
{
	static const int EDGE_PADDING = 10;
	RECT bounds;
	int w, h;
	ZeroMemory(&bounds,sizeof(RECT));

	GetClientRect(get_hwnd(),&bounds);
	w = bounds.right - bounds.left;
	h = bounds.bottom - bounds.top;

	wglMakeCurrent(hDC,hContext);
	glViewport(EDGE_PADDING,EDGE_PADDING,w-EDGE_PADDING*2,h-EDGE_PADDING*2);
	glClearColor(1.0,1.0,1.0,1.0);
	glColor3f(0.0,0.0,0.0);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glScaled(double(h)/w,1.0,1.0);

	cout << "config" << endl;
}
/*static*/ VOID DrawArea::RegisterWndClass()
{
	if (regCnt++ > 0)
		return; // already registered

	WNDCLASS wc;
	wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hbrBackground = NULL;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hIcon = NULL; /* TODO: load a beautiful icon */
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpfnWndProc = WindowProc;
    wc.lpszClassName = "DrawArea";
    wc.lpszMenuName = NULL;
    wc.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
    if (!RegisterClass(&wc))
        throw runtime_error("fail RegisterClass()");
}
/*static*/ LRESULT CALLBACK DrawArea::WindowProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	RECT rect;
	DrawArea* drawArea;
	drawArea = static_cast<DrawArea*>(lookup(hWnd));
	
	if (msg == WM_CREATE) {
		CREATESTRUCT* cs = reinterpret_cast<CREATESTRUCT*>(lParam);
		drawArea = reinterpret_cast<DrawArea*>(cs->lpCreateParams);
		add_handle(hWnd,drawArea);
		drawArea->create_context(); // make sure context ready by first WM_SIZE
		return 0;
	}

	if (drawArea != NULL) {
		switch (msg) {
		case WM_SIZE:
			drawArea->config();
			return 0;

		case WM_PAINT:
			drawArea->render();
			GetClientRect(hWnd,&rect);
			ValidateRect(hWnd,&rect);
			return 0;
		}
	}

	return DefWindowProc(hWnd,msg,wParam,lParam);
}
