// window.h - trafficsim/win32
#ifndef WINDOW_H
#define WINDOW_H
#include "simulation.h"
#include <Windows.h>
#include <gl/GL.h>

namespace gui
{

	class window
	{
	public:
		static int run_application(int argc,char* argv[]);
	private:
		window();
		~window();
	
		HDC hDC;
		HGLRC hContext;
		HWND hWnd;
		trafficsim::simulation sim;
		
		bool update();
		bool message();
		void render();
		void config();
		
		static int ref;
		static window* singleton;
		
		static VOID RegisterWndClass(HINSTANCE hInst);
		static LRESULT CALLBACK WindowProc(HWND,UINT,WPARAM,LPARAM);
	};

} // gui

#endif
