// window.cpp - trafficsim
#include "window.h"
#define VIEWPORT_PADDING 10
#define DEFAULT_COLOR 0.8,0.8,0.8
using namespace std;
using namespace gui;

/*static*/ int window::ref = 0;
/*static*/ int window::idTop = 1;
/*static*/ Display* window::display = NULL;
/*static*/ XVisualInfo* window::visual = NULL;
/*static*/ unordered_map<int,window> window::windows;
window::window(GtkApplication* app,int wid)
    : id(wid)
{
    if (ref++ == 0) {
        static int GLX_VISUAL[] = {
            GLX_RGBA,
            GLX_DEPTH_SIZE,24,
            GLX_DOUBLEBUFFER,None
        };
        /* get connection to X server and create visual */
        display = gdk_x11_get_default_xdisplay();
        if (!glXQueryExtension(display,NULL,NULL))
            throw runtime_error("OpenGL is not supported for the specified display");
        visual = glXChooseVisual(display,gdk_x11_get_default_screen(),GLX_VISUAL);
        if (visual == NULL)
            throw runtime_error("fail glXChooseVisual()");
    }

    // create and setup Gtk widgets
    frame = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(frame),"trafficsim");
    box = gtk_box_new(GTK_ORIENTATION_VERTICAL,5);
    drawBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);
    controlBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);
    drawingArea = gtk_drawing_area_new();
    gtk_widget_set_size_request(drawingArea,800,600);
    gtk_widget_set_double_buffered(drawingArea,false);
    g_signal_connect(drawingArea,"configure-event",G_CALLBACK(on_configure_drawarea),this);
    g_signal_connect(drawingArea,"draw",G_CALLBACK(on_draw_drawarea),this);
    gtk_box_pack_start(GTK_BOX(drawBox),drawingArea,TRUE,TRUE,0);
    gtk_box_pack_start(GTK_BOX(box),drawBox,TRUE,TRUE,0);
    gtk_box_pack_start(GTK_BOX(box),controlBox,FALSE,FALSE,0);
    gtk_window_set_resizable(GTK_WINDOW(frame),FALSE);

    // add widgets to main frame
    gtk_container_add(GTK_CONTAINER(frame),box);
    gtk_application_add_window(app,GTK_WINDOW(frame));

    // create OpenGL context for use with the drawing area
    context = glXCreateContext(display,visual,None,True);
    if (context == NULL)
        throw runtime_error("cannot create OpenGL context");
    handle = None;
}
window::~window()
{
    // cleanup OpenGL
    glXDestroyContext(display,context);
    if (--ref <= 0) {
        XFree(visual);
    }
}
/*static*/ int window::run_application(int argc,char* argv[])
{
    int status;
    GtkApplication* app = gtk_application_new("com.trafficsim",G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app,"activate",G_CALLBACK(window::on_app_activate),NULL);
    status = g_application_run(G_APPLICATION(app),argc,argv);
    close_application(app);
    return status;
}
/*static*/ void window::on_app_activate(GApplication* app,gpointer)
{
    // launch a new application window
    int id = idTop++;
    auto& win = windows.emplace(
        std::piecewise_construct,
        std::forward_as_tuple(id),
        std::forward_as_tuple(GTK_APPLICATION(app),id)).first->second;
    gtk_widget_show_all(win.frame);
    win.handle = gdk_x11_window_get_xid(gtk_widget_get_window(win.drawingArea));
}
/*static*/ void window::close_application(GtkApplication* app)
{
    windows.clear();
    g_object_unref(app);
}
/*static*/ void window::on_configure_drawarea(GtkWidget* inst,GdkEvent* event,window* win)
{
    int w, h;
    // bind the context for the window to the thread
    if (!glXMakeCurrent(display,win->handle,win->context))
        throw runtime_error("fail glXMakeCurrent");

    // setup viewport to draw area (minus padding); 'inst' should be the draw
    // area
    w = gtk_widget_get_allocated_width(inst);
    h = gtk_widget_get_allocated_height(inst);
    glViewport(
        VIEWPORT_PADDING,VIEWPORT_PADDING,
        w-VIEWPORT_PADDING*2,
        h-VIEWPORT_PADDING*2 );
    glMatrixMode(GL_PROJECTION);

    // setup state machine
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    glEnableClientState(GL_VERTEX_ARRAY);
}
/*static*/ void window::on_draw_drawarea(GtkWidget* inst,cairo_t* ctx,window* win)
{
    // bind the context for the window to the thread
    if (!glXMakeCurrent(display,win->handle,win->context))
        throw runtime_error("fail glXMakeCurrent");

    // calls to rendering here

    glXSwapBuffers(display,win->handle);
}
