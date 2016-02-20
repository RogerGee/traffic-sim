// window.h - trafficsim/gtkx
#ifndef WINDOW_H
#define WINDOW_H
#include "simulation.h"
#include <unordered_map>
#include <stdexcept>
#include <gtk/gtk.h>
#include <gdk/gdkx.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glext.h>

namespace gui
{

    // window - represents the GUI interface to the application; each window
    // provides a single traffic simulation
    class window
    {
    public:
        window(GtkApplication*,int id);
        ~window();

        // entry point to application should call this
        static int run_application(int argc,char* argv[]);
    private:
        const int id; // refer to object in window::windows
        int touttag; // timeout tag
        GtkWidget* frame; // Gtk widgets...
        GtkWidget* btnSimul, *btnPause;
        GtkWidget* scaleSimulSpeed;
        GtkWidget* scaleSpawnRate;
        GtkWidget* scaleLightSpeed;
        GtkWidget* dispAvgWaitTime;
        GtkWidget* dispLowWaitTime;
        GtkWidget* dispHighWaitTime;
        GtkWidget* dispAvgWaitCars;
        GtkWidget* drawingArea;
        GtkWidget* box, *drawBox, *controlBox;
        GLXContext context; // OpenGL context
        Window handle; // xid of draw area
        trafficsim::simulation sim; // traffic simulation

        window(const window&) = delete;
        window& operator=(const window&) = delete;
        void resize(int w,int h);
        void redraw();
        void update_stats();
        void reset_stats();
        void sync_scales();

        static int ref;
        static int idTop;
        static const float TIMEOUT;
        static Display* display; // X-display connection
        static XVisualInfo* visual; // visual parameters for GLX contexts
        static std::unordered_map<int,window> windows; // store windows in use
                                                       // by application

        // application events
        static void on_app_activate(GApplication*,gpointer);
        static void close_application(GtkApplication*);

        // window events
        static gboolean on_window_delete(GtkWidget* widget,GdkEvent* event,window* win);
        static void on_configure_drawarea(GtkWidget* inst,GdkEvent* event,window* win);
        static void on_draw_drawarea(GtkWidget* inst,cairo_t* ctx,window* win);
        static void on_simul_btn_clicked(GtkButton* btn,window* win);
        static void on_pause_btn_clicked(GtkButton* btn,window* win);
        static void on_scale_value_changed(GtkRange* widget,window* win);

        // this is the main loop for the simulation
        static gboolean loop(window* win);
    };

} // gui

#endif
