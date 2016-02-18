// window.h - trafficsim/gtkx
#ifndef WINDOW_H
#define WINDOW_H
#include <unordered_map>
#include <stdexcept>
#include <gtk/gtk.h>
#include <gdk/gdkx.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glext.h>

namespace gui
{

    class window
    {
    public:
        window(GtkApplication*,int id);
        ~window();

        static int run_application(int argc,char* argv[]);
    private:
        const int id;
        GtkWidget* frame;
        GtkWidget* drawingArea;
        GtkWidget* box, *drawBox, *controlBox;
        GLXContext context;
        Window handle; // xid of draw area

        window(const window&) = delete;
        window& operator=(const window&) = delete;

        static int ref;
        static int idTop;
        static Display* display;
        static XVisualInfo* visual;
        static std::unordered_map<int,window> windows;

        static void on_app_activate(GApplication*,gpointer);
        static void close_application(GtkApplication*);

        // window events
        static void on_configure_drawarea(GtkWidget* inst,GdkEvent* event,window* win);
        static void on_draw_drawarea(GtkWidget* inst,cairo_t* ctx,window* win);
    };

} // gui

#endif
