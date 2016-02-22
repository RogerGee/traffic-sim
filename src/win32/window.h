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
		
		bool update(float tout);
		bool message();
		void config();
		void onclick_simul(Control* btn);
		void onclick_pause(Control* btn);
		
		static int ref;
		static window* singleton;
		
		static VOID RegisterWndClass(HINSTANCE hInst);
		static LRESULT CALLBACK WindowProc(HWND,UINT,WPARAM,LPARAM);
	};

} // gui

#endif
