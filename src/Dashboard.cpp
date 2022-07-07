#include "ConsoleViewer.h"
#include "Dashboard.h"
#include "HelpViewer.h"
#include "JsonViewer.h"

// TODO - use pointer to string for help message
//   try again with writing file
//   keep track of whether there are changes and enable save button accordingly
//   implement save as
//   try sending and receivimg msgs
//   keep a queue of received messages and scroll through them
// bugs
//  tourney id can be changed only once
//  empty topic id for subscribe
//  template changes don't persist

Dashboard::Dashboard(Parameters* pParms, Logger* pLogger) :
    pParms_(pParms),
    pLogger_(pLogger),
    pWidgets_((WidgetVault*) g_malloc (sizeof (WidgetVault)))
{
    instantiate_widgets();
    arrange_widgets();
    wire_signals();

    // populate template combo box
    g_signal_emit_by_name( G_OBJECT( pWidgets_->publishTopicChooser_ ), "changed" );
}

Dashboard::~Dashboard()
{
    g_free( pWidgets_ );
}

// private methods
void Dashboard::instantiate_widgets()
{
    // main window
    pWidgets_->mainWindow_ = gtk_window_new( GTK_WINDOW_TOPLEVEL );

    // panes
    pWidgets_->verticalPane_ = gtk_paned_new( GTK_ORIENTATION_VERTICAL );
    pWidgets_->horizontalPane_ = gtk_paned_new( GTK_ORIENTATION_HORIZONTAL );
    pWidgets_->jsonViewerPane_ = gtk_paned_new( GTK_ORIENTATION_HORIZONTAL );
 
    // viewers
    pWidgets_->subscribeJsonViewer_ = json_viewer_new( pLogger_, TRUE );
    pWidgets_->publishJsonViewer_ = json_viewer_new( pLogger_, FALSE );
    pWidgets_->consoleViewer_ = console_viewer_new( pLogger_ ); 
    pWidgets_->helpViewer_ = help_viewer_new();

    // choosers
    pWidgets_->subscribeTopicChooser_ = topic_chooser_new( pParms_, pLogger_, STATE_TB );
    pWidgets_->publishTopicChooser_ = topic_chooser_new(pParms_, pLogger_, EVENT_TB );
    pWidgets_->templateChooser_ = template_chooser_new( pLogger_, pParms_->get_working_directory() );
    pWidgets_->tourneyIdChooser_ = tourney_id_chooser_new( pParms_->get_tourney_id() );

    // buttons
    pWidgets_->subscribeButton_ = gtk_toggle_button_new_with_label( "Subscribe" );
    pWidgets_->publishButton_ = gtk_button_new_with_label( "Publish" );
    pWidgets_->saveTemplateButton_ = gtk_button_new_with_label( "Save" );

    // panels
    pWidgets_->subscribePanel_ = subscribe_panel_new();    
    pWidgets_->publishPanel_ = publish_panel_new();    
    pWidgets_->controlPanel_ = control_panel_new();
}

void Dashboard::arrange_widgets()
{
    // main window
    gtk_window_set_title( GTK_WINDOW( pWidgets_->mainWindow_ ), "Backend Tester" );
    gtk_container_set_border_width( GTK_CONTAINER( pWidgets_->mainWindow_ ), 10 ); 
    gtk_container_add( GTK_CONTAINER( pWidgets_->mainWindow_ ), pWidgets_->verticalPane_ );

    // vertical pane
    gtk_widget_set_size_request( pWidgets_->verticalPane_, 1200, 800 ); 
    gtk_paned_add1( GTK_PANED( pWidgets_->verticalPane_ ), pWidgets_->horizontalPane_ );
    gtk_paned_add2( GTK_PANED( pWidgets_->verticalPane_  ), pWidgets_->consoleViewer_ );
    gtk_paned_set_position( GTK_PANED( pWidgets_->verticalPane_ ) , 647 );

    // two left panes and one right pane
    gtk_paned_add1( GTK_PANED( pWidgets_->horizontalPane_ ), pWidgets_->jsonViewerPane_ );
    gtk_paned_add2( GTK_PANED( pWidgets_->horizontalPane_  ), pWidgets_->controlPanel_ );
    gtk_paned_set_position( GTK_PANED( pWidgets_->horizontalPane_ ) , 820 );  
 
    // left panes
    gtk_paned_add1( GTK_PANED( pWidgets_->jsonViewerPane_ ), pWidgets_->subscribePanel_ );
    gtk_paned_add2( GTK_PANED( pWidgets_->jsonViewerPane_  ), pWidgets_->publishPanel_ );
    gtk_paned_set_position( GTK_PANED( pWidgets_->jsonViewerPane_ ) , 400 );     
}

void Dashboard::wire_signals()
{
    g_signal_connect( G_OBJECT( pWidgets_->subscribeTopicChooser_ ), "changed", G_CALLBACK( OnSubscribeTopicChanged ), gpointer(this) );
    g_signal_connect( G_OBJECT( pWidgets_->publishTopicChooser_ ), "changed", G_CALLBACK( OnPublishTopicChanged ), gpointer(this) );
    g_signal_connect( G_OBJECT( pWidgets_->templateChooser_ ), "changed", G_CALLBACK( OnMessageTemplateChanged ), gpointer(this) ); 
    g_signal_connect( G_OBJECT( pWidgets_->tourneyIdChooser_), "changed", G_CALLBACK( OnTourneyIdChanged ), gpointer(this) ); 
 
 
    g_signal_connect( G_OBJECT( pWidgets_->subscribeButton_ ), "toggled", G_CALLBACK( OnSubscribeButtonClicked ), gpointer(this) );
    g_signal_connect( G_OBJECT( pWidgets_->publishButton_ ), "clicked", G_CALLBACK( OnPublishButtonClicked ), gpointer(this) );
    g_signal_connect( G_OBJECT( pWidgets_->saveTemplateButton_ ), "clicked", G_CALLBACK( OnSaveButtonClicked ), gpointer(this) );
 
    g_signal_connect( G_OBJECT( pWidgets_->mainWindow_ ), "destroy", G_CALLBACK( OnMainWindowDestroy ), gpointer( pLogger_ ) ); 
}

GtkWidget* Dashboard::subscribe_panel_new()
{
    GtkWidget* _pSubscribePanel = gtk_box_new( GTK_ORIENTATION_VERTICAL, 10 ); 
    gtk_container_set_border_width( GTK_CONTAINER( _pSubscribePanel ), 10 );
    
    gtk_box_pack_start( GTK_BOX( _pSubscribePanel ), pWidgets_->subscribeButton_, FALSE, TRUE, 0 );
    gtk_box_pack_start( GTK_BOX( _pSubscribePanel ), pWidgets_->subscribeTopicChooser_, FALSE, TRUE, 0 );
    gtk_box_pack_start( GTK_BOX( _pSubscribePanel ), pWidgets_->subscribeJsonViewer_, TRUE, TRUE, 0 );

    return _pSubscribePanel;
}

GtkWidget* Dashboard::publish_panel_new()
{
    GtkWidget* _pPublishPanel = gtk_box_new( GTK_ORIENTATION_VERTICAL, 10 ); 
    gtk_container_set_border_width( GTK_CONTAINER( _pPublishPanel ), 10 );
;
    gtk_box_pack_start( GTK_BOX( _pPublishPanel ), pWidgets_->publishButton_, FALSE, TRUE, 0 );
    gtk_box_pack_start( GTK_BOX( _pPublishPanel ), pWidgets_->publishTopicChooser_, FALSE, TRUE, 0 );
    gtk_box_pack_start( GTK_BOX( _pPublishPanel ), pWidgets_->publishJsonViewer_, TRUE, TRUE, 0 );

    return _pPublishPanel;
}

GtkWidget* Dashboard::control_panel_new()
{
    GtkWidget* _pControlPanel = gtk_box_new( GTK_ORIENTATION_VERTICAL, 10 ); 
    gtk_container_set_border_width( GTK_CONTAINER( _pControlPanel ), 10 );

    gtk_box_pack_start( GTK_BOX( _pControlPanel ), pWidgets_->tourneyIdChooser_, FALSE, TRUE, 0 );
    gtk_box_pack_start( GTK_BOX( _pControlPanel ), pWidgets_->templateChooser_, FALSE, TRUE, 0 );
    gtk_box_pack_start( GTK_BOX( _pControlPanel ), pWidgets_->saveTemplateButton_, FALSE, TRUE, 0 );
    gtk_box_pack_start( GTK_BOX( _pControlPanel ), gtk_label_new( "Help" ), FALSE, TRUE, 0 );
    gtk_box_pack_start( GTK_BOX( _pControlPanel ), pWidgets_->helpViewer_, TRUE, TRUE, 0 ); 

    return _pControlPanel;
}

// callbacks
void Dashboard::OnSubscribeTopicChanged( TopicChooser* pTopicChooser, Dashboard* pDashboard )
{

}

void Dashboard::OnPublishTopicChanged( TopicChooser* pTopicChooser, Dashboard* pDashboard )
{
    gchar* _topic = topic_chooser_get_topic_prefix( pTopicChooser );
    g_object_set( G_OBJECT( pDashboard->pWidgets_->templateChooser_ ), "topic", _topic, NULL );
    g_free( _topic );
}

void Dashboard::OnMessageTemplateChanged( TemplateChooser* pTemplateChooser, Dashboard* pDashboard )
{
 
    int _id = -1;
    gchar* _templateName = template_chooser_get_template_name( TEMPLATE_CHOOSER( pTemplateChooser ) );
    gchar* _template = template_chooser_get_template( TEMPLATE_CHOOSER( pTemplateChooser ), &_id );
    gchar* _helpText = template_chooser_get_help_text( TEMPLATE_CHOOSER( pTemplateChooser ), NULL );

    if ( _id >= 0 )
    { 
        pDashboard->pLogger_->Debug( g_strdup_printf( "Template changed to %s", _templateName ) );
        pDashboard->set_publish_message( _template, _id );
        pDashboard->set_help_message( _helpText, _id );
    }
    else
    {
        pDashboard->set_publish_message( _template, _id );
        pDashboard->set_help_message( _helpText, _id );       
    }

    g_free( _templateName );
    g_free( _template );
    g_free( _helpText );
}

void Dashboard::OnTourneyIdChanged( TourneyIdChooser* pTourneyIdChooser, Dashboard* pDashboard )
{
    pDashboard->pParms_->set_tourney_id( tourney_id_chooser_get_tourney_id( pTourneyIdChooser ) );;
    pDashboard->pLogger_->Info( g_strdup_printf( "Tourney id changed to %s", pDashboard->pParms_->get_tourney_id() ) );
}

void Dashboard::OnSubscribeButtonClicked( GtkToggleButton* pButton, Dashboard* pDashboard )
{
    gchar* _topic = topic_chooser_get_topic( TOPIC_CHOOSER( pDashboard->pWidgets_->subscribeTopicChooser_ ) );
    if ( _topic )
    {
        if ( gtk_toggle_button_get_active( pButton ) )
        {
            gtk_button_set_label( GTK_BUTTON( pButton ), "Unsubscribe" );
            if (!pDashboard->ignoreSubscribeButtonClick_) // if TRUE, we set the state -- user did not click
            {
                // TODO subscribe
                pDashboard->subscribedTopics_.insert( _topic );
                pDashboard->pLogger_->Info( g_strdup_printf( "Subscribed to ", _topic ) );
            }
        }
        else
        {
            gtk_button_set_label( GTK_BUTTON( pButton ), "Subscribe" );
            if ( !pDashboard->ignoreSubscribeButtonClick_ ) // if TRUE, we set the state -- user did not click
            {
                // TODO unsubscribe
                pDashboard->subscribedTopics_.erase( _topic );
                pDashboard->pLogger_->Info( g_strdup_printf( "Unsubscribed from ", _topic ) );
            }
        }
    }

   g_free( _topic );
}

void Dashboard::OnPublishButtonClicked( GtkButton* pButton, Dashboard* pDashboard )
{

}

void Dashboard::OnSaveButtonClicked( GtkButton* pButton, Dashboard* pDashboard )
{
  
}

void Dashboard::OnMainWindowDestroy( GtkWidget* window, Logger* pLogger )
{
    pLogger->set_text_buffer( NULL );
    gtk_main_quit();
}

// public methods
void Dashboard::set_publish_message( const gchar* text, int id ) 
{ 
    json_viewer_set_json_string( JSON_VIEWER( pWidgets_->publishJsonViewer_ ), id, text ); 
}

void Dashboard::set_subscribe_message( const gchar* text ) 
{ 
    json_viewer_set_json_string( JSON_VIEWER( pWidgets_->subscribeJsonViewer_ ), -1, text ); 
} 

void Dashboard::set_help_message( const gchar* text, int id )  
{ 
    help_viewer_set_text( HELP_VIEWER( pWidgets_->helpViewer_ ), id, text ); 
} 

void Dashboard::run()
{
    gtk_widget_show_all( pWidgets_->mainWindow_ );  
    gtk_main();
}
