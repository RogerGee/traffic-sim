// window.cpp - trafficsim
#include "window.h"
#include <sstream>
#define EDGE_PADDING 10
using namespace std;
using namespace gui;
using namespace trafficsim;

/*static*/ int window::ref = 0;
/*static*/ int window::idTop = 1;
/*static*/ const float window::TIMEOUT = 1.0/60.0; // in seconds
/*static*/ Display* window::display = NULL;
/*static*/ XVisualInfo* window::visual = NULL;
/*static*/ unordered_map<int,window> window::windows;
window::window(GtkApplication* app,int wid)
    : id(wid)
{
    GtkGrid* grid;
    GtkWidget* label;
    GtkWidget* x[10];

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
    x[0] = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,2);
    btnSimul = gtk_button_new_with_label("Start");
    btnPause = gtk_button_new_with_label("Pause");
    gtk_widget_set_sensitive(btnPause,FALSE);
    gtk_box_pack_start(GTK_BOX(x[0]),btnSimul,TRUE,TRUE,0);
    gtk_box_pack_start(GTK_BOX(x[0]),btnPause,TRUE,TRUE,0);
    scaleSimulSpeed = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL,50,200,1);
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
    gtk_grid_attach(grid,gtk_label_new("Mean Wait Time:"),0,3,1,1);
    gtk_grid_attach(grid,dispAvgWaitTime,1,3,1,1);
    gtk_grid_attach(grid,gtk_label_new("Low Wait Time:"),0,4,1,1);
    gtk_grid_attach(grid,dispLowWaitTime,1,4,1,1);
    gtk_grid_attach(grid,gtk_label_new("High Wait Time:"),0,5,1,1);
    gtk_grid_attach(grid,dispHighWaitTime,1,5,1,1);
    gtk_grid_attach(grid,gtk_label_new("Mean Cars Waiting:"),0,6,1,1);
    gtk_grid_attach(grid,dispAvgWaitCars,1,6,1,1);
    for (int i = 0;i < 2;++i) {
        for (int j = 0;j < 7;++j) {
            GtkWidget* widget = gtk_grid_get_child_at(grid,i,j);
            if (widget != NULL)
                gtk_widget_set_halign(widget,GTK_ALIGN_START);
        }
    }
    gtk_grid_set_column_spacing(grid,10);
    gtk_widget_set_double_buffered(drawingArea,false);
    gtk_box_pack_start(GTK_BOX(drawBox),drawingArea,TRUE,TRUE,0);
    gtk_box_pack_start(GTK_BOX(controlBox),gtk_label_new("Control Panel"),TRUE,FALSE,0);
    gtk_box_pack_start(GTK_BOX(controlBox),x[0],FALSE,FALSE,0);
    gtk_box_pack_start(GTK_BOX(controlBox),gtk_separator_new(GTK_ORIENTATION_HORIZONTAL),FALSE,FALSE,5);
    gtk_box_pack_start(GTK_BOX(controlBox),GTK_WIDGET(grid),FALSE,FALSE,0);
    gtk_box_pack_start(GTK_BOX(box),drawBox,TRUE,TRUE,0);
    gtk_box_pack_start(GTK_BOX(box),controlBox,FALSE,FALSE,0);
    g_signal_connect(drawingArea,"configure-event",G_CALLBACK(on_configure_drawarea),this);
    g_signal_connect(drawingArea,"draw",G_CALLBACK(on_draw_drawarea),this);
    g_signal_connect(frame,"delete-event",G_CALLBACK(on_window_delete),this);
    g_signal_connect(btnSimul,"clicked",G_CALLBACK(on_simul_btn_clicked),this);
    g_signal_connect(btnPause,"clicked",G_CALLBACK(on_pause_btn_clicked),this);
    g_signal_connect(scaleSimulSpeed,"value-changed",G_CALLBACK(on_scale_value_changed),this);
    g_signal_connect(scaleSpawnRate,"value-changed",G_CALLBACK(on_scale_value_changed),this);
    g_signal_connect(scaleLightSpeed,"value-changed",G_CALLBACK(on_scale_value_changed),this);    
    reset_stats();
    sync_scales();

    // add widgets to main frame
    gtk_container_add(GTK_CONTAINER(frame),box);
    gtk_application_add_window(app,GTK_WINDOW(frame));

    // create OpenGL context for use with the drawing area
    context = glXCreateContext(display,visual,None,True);
    if (context == NULL)
        throw runtime_error("cannot create OpenGL context");
    handle = None;

    // register a callback for the render loop; save the tag so we can remove it
    touttag = g_timeout_add(TIMEOUT*1000,(GSourceFunc)loop,this);
}
window::~window()
{
    // remove the timeout function
    g_source_remove(touttag);

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
    g_signal_connect(app,"activate",G_CALLBACK(on_app_activate),NULL);
    status = g_application_run(G_APPLICATION(app),argc,argv);
    close_application(app);
    return status;
}
void window::update_stats()
{
    stringstream ss;
    ss.precision(4); ss.flags(ios_base::fixed);
    ss << sim.get_statistic(simul_stat_mean_wait_time);
    gtk_label_set_text(GTK_LABEL(dispAvgWaitTime),ss.str().c_str());
    ss.str(""); ss << sim.get_statistic(simul_stat_low_wait_time);
    gtk_label_set_text(GTK_LABEL(dispLowWaitTime),ss.str().c_str());
    ss.str(""); ss << sim.get_statistic(simul_stat_high_wait_time);
    gtk_label_set_text(GTK_LABEL(dispHighWaitTime),ss.str().c_str());
    ss.str(""); ss << sim.get_statistic(simul_stat_mean_cars_waiting);
    gtk_label_set_text(GTK_LABEL(dispAvgWaitCars),ss.str().c_str());
}
void window::reset_stats()
{
    string def;
    stringstream ss;
    ss.precision(4); ss.flags(ios_base::fixed);
    ss << float(0);
    def = ss.str();
    gtk_label_set_text(GTK_LABEL(dispAvgWaitTime),def.c_str());
    gtk_label_set_text(GTK_LABEL(dispLowWaitTime),def.c_str());
    gtk_label_set_text(GTK_LABEL(dispHighWaitTime),def.c_str());
    gtk_label_set_text(GTK_LABEL(dispAvgWaitCars),def.c_str());
}
void window::sync_scales()
{
    int steptime, spawnrate, lightspeed;
    steptime = sim.get_param(simul_steptime_param);
    spawnrate = sim.get_param(simul_spawnrate_param);
    lightspeed = sim.get_param(simul_lightspeed_param);

    gtk_range_set_value(GTK_RANGE(scaleSimulSpeed),steptime);
    gtk_range_set_value(GTK_RANGE(scaleSpawnRate),spawnrate);
    gtk_range_set_value(GTK_RANGE(scaleLightSpeed),lightspeed);
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

    // now that we have the X handle for the draw area window, we can configure
    // the window to render the scene
    win.resize(800,600);
}
/*static*/ void window::close_application(GtkApplication* app)
{
    windows.clear();
    g_object_unref(app);
}
/*static*/ gboolean window::on_window_delete(GtkWidget* widget,GdkEvent* event,window* win)
{
    // delete the window object from its collection; this will invoke the
    // destructor
    windows.erase(win->id);

    // let the delete happen
    return FALSE;
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
/*static */ void window::on_simul_btn_clicked(GtkButton* btn,window* win)
{
    simulation& sim = win->sim;
    if (sim.get_state() == simul_state_running || sim.get_state() == simul_state_paused) {
        // reset everything
        sim.stop();
        gtk_button_set_label(btn,"Start");
        gtk_button_set_label(GTK_BUTTON(win->btnPause),"Pause");
        gtk_widget_set_sensitive(win->btnPause,FALSE);
        win->reset_stats();
        win->sync_scales();
    }
    else if (sim.get_state() == simul_state_stopped) {
        sim.start();
        gtk_button_set_label(btn,"Stop");
        gtk_widget_set_sensitive(win->btnPause,TRUE);
    }
    win->redraw();
}
/*static*/ void window::on_pause_btn_clicked(GtkButton* btn,window* win)
{
    simulation& sim = win->sim;
    if (sim.get_state() == simul_state_running) {
        sim.pause();
        gtk_button_set_label(btn,"Resume");
    }
    else if (sim.get_state() == simul_state_paused) {
        sim.pause(true);
        gtk_button_set_label(btn,"Pause");
    }
    win->redraw();
}
/*static*/ void window::on_scale_value_changed(GtkRange* widget,window* win)
{
    int value;
    simulation& sim = win->sim;
    value = int(gtk_range_get_value(widget));

    if (GTK_WIDGET(widget) == win->scaleSimulSpeed) {
        sim.set_param(simul_steptime_param,value);
    }
    else if (GTK_WIDGET(widget) == win->scaleSpawnRate) {
        sim.set_param(simul_spawnrate_param,value);
    }
    else if (GTK_WIDGET(widget) == win->scaleLightSpeed) {
        sim.set_param(simul_lightspeed_param,value);
    }
}
/*static*/ gboolean window::loop(window* win)
{
    simulation& sim = win->sim;
    if (sim.is_running()) {
        sim.tick(TIMEOUT); // update simulation
        win->redraw(); // force an expose
        win->update_stats();
    }

    // always run this function (even if it does nothing)
    return TRUE;
}
