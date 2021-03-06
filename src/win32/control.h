// control.h - trafficsim/win32
#ifndef CONTROL_H
#define CONTROL_H
#include "simulation.h"
#include <string>
#include <unordered_map>
#include <Windows.h>
#include <Commctrl.h>
#include <gl/GL.h>

namespace gui
{

    class Control
    {
    public:
        Control();
        ~Control();

        void create(HWND hParent);
        void change(int x,int y);
        void change(int x,int y,int w,int h);
        void change_size(int w,int h);
        void get_size(int& w,int& h);
        char* get_text(char* buffer,size_t size);
        void set_text(const std::string& text,bool resize = true);
        void enable(bool enabled);
        void send_message(UINT message,WPARAM wParam,LPARAM lParam)
        { SendMessage(hWnd,message,wParam,lParam); }
        static void wm_command(HWND hwnd,WPARAM wParam);

        HWND get_hwnd() const
        { return hWnd; }
    protected:
        static void set_text_impl(HWND hwnd,const char* text,int length,bool resize);
        static void add_handle(HWND handle,Control* obj);
        static Control* lookup(HWND handle);
    private:
        HWND hWnd;
        static std::unordered_map<HWND,Control*> inst;

        virtual HWND oncreate(HWND hParent) = 0;
        virtual void command(WPARAM wParam) {};
    };

    class DrawArea : public Control
    {
    public:
        DrawArea(trafficsim::simulation& simul);
        ~DrawArea();

        void render();
    private:
        HDC hDC;
        HGLRC hContext;
        trafficsim::simulation& sim;

        virtual HWND oncreate(HWND hParent);
        void create_context();
        void config();

        static int regCnt;
        static VOID RegisterWndClass();
        static LRESULT CALLBACK WindowProc(HWND,UINT,WPARAM,LPARAM);
    };

    class Label : public Control
    {
    public:
        Label(const std::string& initialText);

    private:
        std::string lblText;

        virtual HWND oncreate(HWND hParent);
    };

    // forward declare window class so that we can create event
    // handles that are member functions of window
    class window;
    typedef void (window::*event_handler)(Control*);

    class Button : public Control
    {
    public:
        Button(const std::string& t,window* w,event_handler eh);

    private:
        std::string lblText;
        window* win;
        event_handler handler;

        virtual HWND oncreate(HWND hParent);
        virtual void command(WPARAM wParam);
    };

    class TrackBar : public Control
    {
    public:
        TrackBar(window* w,event_handler eh,
            int vmin,int vmax,bool orient = true); // true is H, false is V

        void set_value(int v);
        int get_value() const;
        void set_buddy(Control& ctrl,bool side = true);
    private:
        window* win;
        event_handler handler;
        int mn, mx;
        bool o;

        virtual HWND oncreate(HWND hParent);
        virtual void command(WPARAM wParam);
    };
}

#endif
