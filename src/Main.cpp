#include "activemq.h"
#include "AMQClient.h"
#include "JsonFormatter.h"
#include "Logger.h"
#include "Parameters.h"

#include <gtk/gtk.h>
#include <iostream>
#include <memory>


int main(int argc, char** argv) 
{
     
    // build Parameters instance from command-line arguments
    auto _pParms = std::make_shared<Parameters>();
    if (0 != _pParms->ParseArguments(argc, argv))
    {
        exit(0);
    }

    gtk_init(&argc, &argv);

    GtkWidget* _window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(_window), "Backend Tester");
    gtk_container_set_border_width(GTK_CONTAINER(_window), 10);
    gtk_widget_set_size_request(_window, 1800, 800);  
    g_signal_connect(G_OBJECT(_window), "destroy", G_CALLBACK(gtk_main_quit), NULL); 

    // start Logger
    auto _pLogger = std::make_shared<Logger>(_pParms);
    _pLogger->Info("Backend Tester started");

    // start ActiveMQ session
    AMQClient _session(_pLogger);
    _session.subscribe("State.tb=lh-test-1");

    // start the connection
    _session.start();

    // wait on GUI
    gtk_widget_show_all(_window);
    gtk_main();

    // send the message and delete it
    _session.send_text_message("Event.tb=lh-test-1", "Test text message");
    _session.stop();
    _pLogger->Info("Backend Tester exited");
}