// control.cpp - trafficsim/win32
#include "control.h"
#include <iostream>
using namespace std;
using namespace gui;
using namespace trafficsim;

// I ripped this off SO; it's nice for debugging
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

// Control

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
void Control::change(int x,int y)
{
    // just change the location
    SetWindowPos(
        hWnd,
        HWND_TOP, // masking this out
        x, y,
        0, 0, // masking these out
        SWP_NOZORDER | SWP_NOSIZE );
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
void Control::change_size(int w,int h)
{
    SetWindowPos(
        hWnd,
        HWND_TOP, // masking this out
        0, 0, // masking these out
        w, h,
        SWP_NOZORDER | SWP_NOMOVE );
}
void Control::get_size(int& w,int& h)
{
    RECT bounds;
    ZeroMemory(&bounds,sizeof(RECT));

    GetClientRect(get_hwnd(),&bounds);
    w = bounds.right - bounds.left;
    h = bounds.bottom - bounds.top;
}
void Control::set_text(const std::string& text,bool resize)
{
    set_text_impl(get_hwnd(),text.c_str(),text.length(),resize);
}
void Control::enable(bool enabled)
{
    EnableWindow(get_hwnd(),enabled);
}
/*static*/ void Control::set_text_impl(HWND hwnd,const char* text,int length,bool resize)
{
    HDC hdc;
    SIZE sz;

    if (resize) {
        hdc = GetDC(hwnd);
        GetTextExtentPoint32(hdc,text,length,&sz);
        sz.cx += 10; sz.cy += 10; // padding
        SetWindowPos(hwnd,HWND_TOP,0,0,sz.cx,sz.cy,SWP_NOZORDER | SWP_NOMOVE);
        ReleaseDC(hwnd,hdc);
    }

    SetWindowText(hwnd,text);
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
/*static*/ void Control::wm_command(HWND hwnd,WPARAM wParam)
{
    Control* ctrl = lookup(hwnd);
    if (ctrl != NULL)
        ctrl->command(wParam);
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
    w -= EDGE_PADDING*2; h -= EDGE_PADDING*2;
    glViewport(EDGE_PADDING,EDGE_PADDING,w,h);
    glClearColor(1.0,1.0,1.0,1.0);
    glColor3f(0.0,0.0,0.0);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glScaled(double(h)/w,1.0,1.0);
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

// Label

Label::Label(const string& initialText)
    : lblText(initialText)
{
}
HWND Label::oncreate(HWND hParent)
{
    HWND hwnd = CreateWindow(WC_STATIC,"",WS_CHILD | WS_VISIBLE,
            0,0,100,50,hParent,NULL,GetModuleHandle(NULL),NULL);
    set_text_impl(hwnd,lblText.c_str(),lblText.length(),true);

    return hwnd;
}

// Button

Button::Button(const std::string& t,window* w,event_handler eh)
    : lblText(t), win(w), handler(eh)
{
}
HWND Button::oncreate(HWND hParent)
{
    HWND hwnd = CreateWindow(WC_BUTTON,"",WS_CHILD | WS_VISIBLE,
            0,0,100,50,hParent,NULL,GetModuleHandle(NULL),NULL);
    set_text_impl(hwnd,lblText.c_str(),lblText.length(),true);

    return hwnd;
}
void Button::command(WPARAM wParam)
{
    (win->*handler)(this);
}

// TrackBar

TrackBar::TrackBar(window* w,event_handler eh,int vmin,int vmax,bool orient)
    : win(w), handler(eh), mn(vmin), mx(vmax), o(orient)
{
}
void TrackBar::set_value(int v)
{
    SendMessage(get_hwnd(),TBM_SETPOS,TRUE,v);
}
int TrackBar::get_value() const
{
    DWORD dwPos = SendMessage(get_hwnd(),TBM_GETPOS,0,0);
    return (int)dwPos;
}
void TrackBar::set_buddy(Control& ctrl,bool side)
{
    SendMessage(get_hwnd(),TBM_SETBUDDY,(WPARAM)side,(LPARAM)ctrl.get_hwnd());
}
HWND TrackBar::oncreate(HWND hParent)
{
    HWND hwnd = CreateWindow(TRACKBAR_CLASS,"",
        WS_CHILD|WS_VISIBLE|TBS_ENABLESELRANGE|(o?TBS_HORZ:TBS_VERT),
        0,0,100,150,hParent,NULL,GetModuleHandle(NULL),NULL);

    // set the range for the trackbar
    SendMessage(hwnd,TBM_SETRANGE,TRUE,MAKELONG(mn,mx));
    return hwnd;
}
void TrackBar::command(WPARAM wParam)
{
    (win->*handler)(this);
}
