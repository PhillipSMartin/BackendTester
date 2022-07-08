#include "ConsoleViewer.h"
#include "Dashboard.h"
#include "HelpViewer.h"

// TODO - use pointer to string for help message
//   must copy tmp file
//   implement save as
//   try sending and receivimg msgs
//   keep a queue of received messages and scroll through them
// bugs
//   problem with second line of file
//   publish topic does not include table number

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
    g_signal_connect( G_OBJECT( pWidgets_->subscribeTopicChooser_ ), "changed", G_CALLBACK( OnSubscribeTopicChanged ), gpointer( this ) );
    g_signal_connect( G_OBJECT( pWidgets_->publishTopicChooser_ ), "changed", G_CALLBACK( OnPublishTopicChanged ), gpointer( this ) );
    g_signal_connect( G_OBJECT( pWidgets_->templateChooser_ ), "changed", G_CALLBACK( OnTemplateChanged ), gpointer( this ) ); 
    g_signal_connect( G_OBJECT( pWidgets_->tourneyIdChooser_), "changed", G_CALLBACK( OnTourneyIdChanged ), gpointer( this ) ); 
 
    subscribe_button_handler_id = g_signal_connect( G_OBJECT( pWidgets_->subscribeButton_ ), "toggled", G_CALLBACK( OnSubscribeButtonClicked ), gpointer( this ) );
    g_signal_connect( G_OBJECT( pWidgets_->subscribeButton_ ), "query-tooltip", G_CALLBACK( OnSubscribeButtonToolTip ), gpointer( this ) );
    g_signal_connect( G_OBJECT( pWidgets_->publishButton_ ), "clicked", G_CALLBACK( OnPublishButtonClicked ), gpointer( this ) );
    g_signal_connect( G_OBJECT( pWidgets_->saveTemplateButton_ ), "clicked", G_CALLBACK( OnSaveButtonClicked ), gpointer( this ) );

    g_signal_connect( G_OBJECT( pWidgets_->publishJsonViewer_ ), "json-edited", G_CALLBACK( OnJsonEdited ), gpointer( this ) );
    g_signal_connect( G_OBJECT( pWidgets_->publishJsonViewer_ ), "json-replaced", G_CALLBACK( OnJsonReplaced ), gpointer( this ) );
 
    g_signal_connect( G_OBJECT( pWidgets_->mainWindow_ ), "destroy", G_CALLBACK( OnMainWindowDestroy ), gpointer( pLogger_ ) ); 
}

void Dashboard::manage_subscribe_button_state( const gchar* topic )
{
    if ( topic )
    {
        g_signal_handler_block( G_OBJECT( pWidgets_->subscribeButton_ ), subscribe_button_handler_id );
        if ( subscribedTopics_.find( topic ) == subscribedTopics_.end() )
        {
            gtk_button_set_label( GTK_BUTTON( pWidgets_->subscribeButton_ ), "Subscribe" );
            gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON( pWidgets_->subscribeButton_ ), FALSE );
        }
        else
        {
            gtk_button_set_label( GTK_BUTTON( pWidgets_->subscribeButton_ ), "Unsubscribe" );
            gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON( pWidgets_->subscribeButton_ ), TRUE );
        }  
        g_signal_handler_unblock( G_OBJECT( pWidgets_->subscribeButton_ ), subscribe_button_handler_id );    
    }   
}

gchar* Dashboard::get_subscribed_topics_tooltip_text()
{
    std::string _tooltipText{ "Subscribed topics:" };
    if (subscribedTopics_.size() == 0)
    {
        _tooltipText += "\n   None";
    }
    else
    {
        for ( std::string _topic : subscribedTopics_ )
        {
           _tooltipText += "\n   " + _topic; 
        }
    }
    return g_strdup( _tooltipText.c_str() );
}

GtkWidget* Dashboard::subscribe_panel_new()
{
    GtkWidget* _pSubscribePanel = gtk_box_new( GTK_ORIENTATION_VERTICAL, 10 ); 
    gtk_container_set_border_width( GTK_CONTAINER( _pSubscribePanel ), 10 );
    
    gtk_box_pack_start( GTK_BOX( _pSubscribePanel ), pWidgets_->subscribeButton_, FALSE, TRUE, 0 );
    gtk_box_pack_start( GTK_BOX( _pSubscribePanel ), pWidgets_->subscribeTopicChooser_, FALSE, TRUE, 0 );
    gtk_box_pack_start( GTK_BOX( _pSubscribePanel ), pWidgets_->subscribeJsonViewer_, TRUE, TRUE, 0 );

    gtk_widget_set_has_tooltip(  pWidgets_->subscribeButton_, TRUE );

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

    gtk_widget_set_sensitive( pWidgets_->saveTemplateButton_, FALSE );

    return _pControlPanel;
}

// callbacks
void Dashboard::OnSubscribeTopicChanged( TopicChooser* pTopicChooser, Dashboard* pDashboard )
{
    gchar* _topic = topic_chooser_get_topic( pTopicChooser );
    pDashboard->manage_subscribe_button_state( _topic );
    g_free( _topic );
}

void Dashboard::OnPublishTopicChanged( TopicChooser* pTopicChooser, Dashboard* pDashboard )
{
    gchar* _topic = topic_chooser_get_topic_prefix( pTopicChooser );
    g_object_set( G_OBJECT( pDashboard->pWidgets_->templateChooser_ ), "topic", _topic, NULL );
    g_free( _topic );
}

void Dashboard::OnTemplateChanged( TemplateChooser* pTemplateChooser, Dashboard* pDashboard )
{
 
    int _id = -1;
    gchar* _templateName = template_chooser_get_template_name( TEMPLATE_CHOOSER( pTemplateChooser ) );
    gchar* _template = template_chooser_get_template( TEMPLATE_CHOOSER( pTemplateChooser ), &_id );
    gchar* _helpText = template_chooser_get_help_text( TEMPLATE_CHOOSER( pTemplateChooser ), NULL );

    if ( _id >= 0 )
    { 
        pDashboard->pLogger_->Debug( g_strdup_printf( "Template changed to '%s'", _templateName ) );
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
            // TODO subscribe
            pDashboard->subscribedTopics_.insert( _topic );
            pDashboard->pLogger_->Info( g_strdup_printf( "Subscribed to %s", _topic ) );
        }
         else
        {
            // TODO unsubscribe
            pDashboard->subscribedTopics_.erase( _topic );
            pDashboard->pLogger_->Info( g_strdup_printf( "Unsubscribed from %s", _topic ) );
        }

        pDashboard->manage_subscribe_button_state( _topic );
    }
    g_free( _topic );
}

bool Dashboard::OnSubscribeButtonToolTip( GtkWidget* pButton, gint x, gint y, gboolean keyboard_mode, GtkTooltip* pTooltip, Dashboard* pDashboard )
{
    gchar* _tooltipText = pDashboard->get_subscribed_topics_tooltip_text();
    gtk_tooltip_set_text( pTooltip,  _tooltipText );
    g_free( _tooltipText );
    return TRUE;
}

void Dashboard::OnPublishButtonClicked( GtkButton* pButton, Dashboard* pDashboard )
{

}

void Dashboard::OnSaveButtonClicked( GtkButton* pButton, Dashboard* pDashboard )
{
    template_chooser_save_changes( TEMPLATE_CHOOSER( pDashboard->pWidgets_->templateChooser_ ) );
}

void Dashboard::OnJsonEdited( JsonViewer* pJsonViewer, Dashboard* pDashboard )
{
    gtk_widget_set_sensitive( pDashboard->pWidgets_->saveTemplateButton_, TRUE );
    int _id;
    gchar* _text = json_viewer_get_json_string( pJsonViewer, &_id );
    template_chooser_update_template( TEMPLATE_CHOOSER( pDashboard->pWidgets_->templateChooser_ ), _text, _id );      
}

void Dashboard::OnJsonReplaced( JsonViewer* pJsonViewer, Dashboard* pDashboard )
{
    gtk_widget_set_sensitive( pDashboard->pWidgets_->saveTemplateButton_, FALSE );
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
