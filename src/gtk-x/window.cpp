// window.cpp - trafficsim
#include "window.h"
#define EDGE_PADDING 10
using namespace std;
using namespace gui;
using namespace trafficsim;

/*static*/ int window::ref = 0;
/*static*/ int window::idTop = 1;
/*static*/ Display* window::display = NULL;
/*static*/ XVisualInfo* window::visual = NULL;
/*static*/ unordered_map<int,window> window::windows;
window::window(GtkApplication* app,int wid)
    : id(wid)
{
    GtkGrid* grid;
    GtkWidget* label;

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
    gtk_window_set_title(GTK_WINDOW(frame),"Traffic Simulator");
    box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,5);
    drawBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);
    controlBox = gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
    drawingArea = gtk_drawing_area_new();
    btnSimul = gtk_button_new_with_label("Begin");
    scaleSimulSpeed = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL,1,100,1);
    scaleSpawnRate = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL,1,100,1);
    scaleLightSpeed = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL,1,100,1);
    dispAvgWaitTime = gtk_label_new("0.0");
    dispLowWaitTime = gtk_label_new("0.0");
    dispHighWaitTime = gtk_label_new("0.0");
    dispAvgWaitCars = gtk_label_new("0.0");
    gtk_widget_set_size_request(scaleSimulSpeed,135,-1);
    gtk_widget_set_size_request(scaleSpawnRate,135,-1);
    gtk_widget_set_size_request(scaleLightSpeed,135,-1);
    grid = GTK_GRID(gtk_grid_new());
    gtk_grid_attach(grid,gtk_label_new("Simulation Speed:"),0,0,1,1);
    gtk_grid_attach(grid,scaleSimulSpeed,1,0,1,1);
    gtk_grid_attach(grid,gtk_label_new("Spawn Rate:"),0,1,1,1);
    gtk_grid_attach(grid,scaleSpawnRate,1,1,1,1);
    gtk_grid_attach(grid,gtk_label_new("Light Speed:"),0,2,1,1);
    gtk_grid_attach(grid,scaleLightSpeed,1,2,1,1);
    gtk_grid_attach(grid,gtk_separator_new(GTK_ORIENTATION_HORIZONTAL),0,3,1,1);
    gtk_grid_attach(grid,gtk_label_new("Mean Wait Time:"),0,4,1,1);
    gtk_grid_attach(grid,dispAvgWaitTime,1,4,1,1);
    gtk_grid_attach(grid,gtk_label_new("Low Wait Time:"),0,5,1,1);
    gtk_grid_attach(grid,dispLowWaitTime,1,5,1,1);
    gtk_grid_attach(grid,gtk_label_new("High Wait Time:"),0,6,1,1);
    gtk_grid_attach(grid,dispHighWaitTime,1,6,1,1);
    gtk_grid_attach(grid,gtk_label_new("Mean Cars Waiting:"),0,7,1,1);
    gtk_grid_attach(grid,dispAvgWaitCars,1,7,1,1);
    for (int i = 0;i < 2;++i) {
        for (int j = 0;j < 8;++j) {
            GtkWidget* widget = gtk_grid_get_child_at(grid,i,j);
            if (widget != NULL)
                gtk_widget_set_halign(widget,GTK_ALIGN_START);
        }
    }
    gtk_grid_set_column_spacing(grid,10);
    gtk_widget_set_double_buffered(drawingArea,false);
    gtk_box_pack_start(GTK_BOX(drawBox),drawingArea,TRUE,TRUE,0);
    gtk_box_pack_start(GTK_BOX(controlBox),gtk_label_new("Control Panel"),TRUE,FALSE,0);
    gtk_box_pack_start(GTK_BOX(controlBox),btnSimul,FALSE,FALSE,0);
    gtk_box_pack_start(GTK_BOX(controlBox),GTK_WIDGET(grid),FALSE,FALSE,0);
    gtk_box_pack_start(GTK_BOX(box),drawBox,TRUE,TRUE,0);
    gtk_box_pack_start(GTK_BOX(box),controlBox,FALSE,FALSE,0);
    g_signal_connect(drawingArea,"configure-event",G_CALLBACK(on_configure_drawarea),this);
    g_signal_connect(drawingArea,"draw",G_CALLBACK(on_draw_drawarea),this);

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
    handle = None;

    // cleanup OpenGL
    glXDestroyContext(display,context);
    if (--ref <= 0) {
        XFree(visual);
    }
}
void window::resize(int w,int h)
{
    if (w == 0 && h == 0) {
        gtk_window_iconify(GTK_WINDOW(frame));
    }
    else {
        gtk_widget_set_size_request(frame,w,h);
    }
}
void window::redraw()
{
    gtk_widget_queue_draw(drawingArea);
}
/*static*/ int window::run_application(int argc,char* argv[])
{
    // run the application; it will terminate when no more top-level windows
    // exist (i.e. the use count is at zero)
    int status;
    GtkApplication* app = gtk_application_new("com.trafficsim",G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app,"activate",G_CALLBACK(window::on_app_activate),NULL);
    status = g_application_run(G_APPLICATION(app),argc,argv);
    close_application(app);
    return status;
}
/*static*/ void window::on_app_activate(GApplication* app,gpointer)
{
    // launch a new application window: this creates a simulation
    int id = idTop++;
    window& win = windows.emplace(
        std::piecewise_construct,
        std::forward_as_tuple(id), // key
        std::forward_as_tuple(GTK_APPLICATION(app),id) /*value*/ ).first->second;
    gtk_widget_show_all(win.frame);
    win.handle = gdk_x11_window_get_xid(gtk_widget_get_window(win.drawingArea));
    win.resize(800,600);
}
/*static*/ void window::close_application(GtkApplication* app)
{
    windows.clear();
    g_object_unref(app);
}
/*static*/ void window::on_configure_drawarea(GtkWidget* inst,GdkEvent* event,window* win)
{
    if (win->handle != None) {
        int w, h;
        // bind the context for the window to the thread
        if (!glXMakeCurrent(display,win->handle,win->context))
            throw runtime_error("fail glXMakeCurrent");

        // setup state machine
        glClearColor(1.0,1.0,1.0,1.0);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
        glEnableClientState(GL_VERTEX_ARRAY);
        glColor3f(0.0,0.0,0.0);

        // setup a viewport based on the current dimensions of the drawing area;
        // add some padding around the edges of the scene
        w = gtk_widget_get_allocated_width(inst);
        h = gtk_widget_get_allocated_height(inst);
        w -= EDGE_PADDING*2; h -= EDGE_PADDING*2;
        glViewport(EDGE_PADDING,EDGE_PADDING,w,h);

        // setup the projection matrix to account for distortion caused by the
        // aspect ratio of the drawing area; the user may add other projections
        // on top of this
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glScaled(double(h)/w,1.0,1.0);
    }
}
/*static*/ void window::on_draw_drawarea(GtkWidget* inst,cairo_t* ctx,window* win)
{
    if (win->handle != None) {
        // bind the context for the window to the thread
        if (!glXMakeCurrent(display,win->handle,win->context))
            throw runtime_error("fail glXMakeCurrent");

        // clear the screen
        glClear(GL_COLOR_BUFFER_BIT);

        // calls to rendering here
        win->sim.render();

        // present back buffer
        glXSwapBuffers(display,win->handle);
    }
}
/*static*/ gboolean window::loop(window* win)
{
    simulation& sim = win->sim;
    if (sim.is_running()) {
        sim.step(); // update simulation
        win->redraw(); // force an expose
        return TRUE;
    }

    // this stops Glib from calling this function
    return FALSE;
}
