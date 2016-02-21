// control.h - trafficsim/win32
#ifndef CONTROL_H
#define CONTROL_H
#include "simulation.h"
#include <unordered_map>
#include <Windows.h>
#include <gl/GL.h>

namespace gui
{

	class Control
	{
	public:
		Control();
		~Control();
	
		void create(HWND hParent);
		void change(int x,int y,int w,int h);
	protected:
		HWND get_hwnd() const
		{ return hWnd; }
	
		static void add_handle(HWND handle,Control* obj);
		static Control* lookup(HWND handle);
	private:
		HWND hWnd;
		static std::unordered_map<HWND,Control*> inst;

		virtual HWND oncreate(HWND hParent) = 0;
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

}

#endif
