#include "Dashboard.h"

// TODO - use pointer to string for help message
//   try again with writing file
//   keep track of whether there are changes and enable save button accordingly
//   implement save as
//   try sending and receivimg msgs
//   keep a queue of received messages and scroll through them

Dashboard::Dashboard(Parameters* pParms, Logger* pLogger) :
    pParms_(pParms),
    pLogger_(pLogger),
    pParent_(gtk_paned_new( GTK_ORIENTATION_VERTICAL )),
    subscribeTopicChooser_(topic_chooser_new( pParms, pLogger, STATE_TB )),
    subscribeViewer_(json_viewer_new( pLogger, TRUE )),
    publishTopicChooser_(topic_chooser_new(pParms, pLogger, EVENT_TB )),
    publishViewer_(json_viewer_new( pLogger, FALSE )),
    pTourneyIdEntry_(gtk_entry_new()),
    templateChooser_(template_chooser_new( pLogger, pParms->get_working_directory() ))
{
    gtk_widget_set_size_request( pParent_, 1200, 800 );  

    // top panel will contain three horizontal panels
    GtkWidget* _panels = gtk_paned_new( GTK_ORIENTATION_HORIZONTAL );
    gtk_paned_add1( GTK_PANED( pParent_ ), _panels );

    // two leftmost panels contain tree views
    GtkWidget* _treeViewPanels = gtk_paned_new( GTK_ORIENTATION_HORIZONTAL );
    gtk_paned_add1( GTK_PANED( _treeViewPanels ), subscribe_panel_new() );
    gtk_paned_add2( GTK_PANED( _treeViewPanels ), publish_panel_new() );
    gtk_paned_set_position(GTK_PANED( _treeViewPanels ), 400); 

    // rightmost panel contains controls
    gtk_paned_add1( GTK_PANED( _panels ), _treeViewPanels);
    gtk_paned_add2( GTK_PANED( _panels ), control_panel_new() );
    gtk_paned_set_position( GTK_PANED( _panels ), 820 ); 

    // bottom panel will be console window
    gtk_paned_add2( GTK_PANED( pParent_ ),  console_window_new() );
    gtk_paned_set_position( GTK_PANED( pParent_) , 650 ); 

    // populate template combo box
    g_signal_emit_by_name( G_OBJECT( publishTopicChooser_ ), "changed" );

}

Dashboard::~Dashboard()
{
}

GtkWidget* Dashboard::console_window_new()
{
    GtkWidget* _pScrolledConsoleWindow = gtk_scrolled_window_new( NULL, NULL );
    GtkWidget* _pConsoleWindow = gtk_text_view_new();
    gtk_scrolled_window_set_policy ( GTK_SCROLLED_WINDOW( _pScrolledConsoleWindow ), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC );
    gtk_container_add( GTK_CONTAINER( _pScrolledConsoleWindow ), _pConsoleWindow );
    pConsoleBuffer_ = gtk_text_view_get_buffer( GTK_TEXT_VIEW( _pConsoleWindow ) );
    pLogger_->set_text_buffer( pConsoleBuffer_ );

    return _pScrolledConsoleWindow;
}

GtkWidget* Dashboard::subscribe_panel_new()
{
    GtkWidget* _pSubscribePanel = gtk_box_new( GTK_ORIENTATION_VERTICAL, 10 ); 
    gtk_container_set_border_width( GTK_CONTAINER( _pSubscribePanel ), 10 );

    // add topic controls
    GtkWidget* _pSubscribeButton = gtk_toggle_button_new_with_label( "Subscribe" );
    g_signal_connect( G_OBJECT( _pSubscribeButton ), "toggled", G_CALLBACK( OnSubscribeButtonToggled ), gpointer(this) );
    gtk_box_pack_start( GTK_BOX( _pSubscribePanel ), _pSubscribeButton, FALSE, TRUE, 0 );

    gtk_box_pack_start( GTK_BOX( _pSubscribePanel ), subscribeTopicChooser_, FALSE, TRUE, 0 );
    gtk_box_pack_start( GTK_BOX( _pSubscribePanel ), subscribeViewer_, TRUE, TRUE, 0 );

    g_signal_connect( G_OBJECT( subscribeTopicChooser_ ), "changed", G_CALLBACK( OnSubscribeTopicSelectionChanged ), gpointer(this) );
    return _pSubscribePanel;
}

GtkWidget* Dashboard::publish_panel_new()
{
    GtkWidget* _pPublishPanel = gtk_box_new( GTK_ORIENTATION_VERTICAL, 10 ); 
    gtk_container_set_border_width( GTK_CONTAINER( _pPublishPanel ), 10 );

    // add topic controls
    GtkWidget* _pPublishButton = gtk_button_new_with_label( "Publish" );
    gtk_box_pack_start( GTK_BOX( _pPublishPanel ), _pPublishButton, FALSE, TRUE, 0 );

    gtk_box_pack_start( GTK_BOX( _pPublishPanel ), publishTopicChooser_, FALSE, TRUE, 0 );
    gtk_box_pack_start( GTK_BOX( _pPublishPanel ), publishViewer_, TRUE, TRUE, 0 );

    g_signal_connect( G_OBJECT( publishTopicChooser_ ), "changed", G_CALLBACK( OnPublishTopicChanged ), gpointer(this) );
    return _pPublishPanel;
}

GtkWidget* Dashboard::control_panel_new()
{
    GtkWidget* _pControlPanel = gtk_box_new( GTK_ORIENTATION_VERTICAL, 10 ); 
    gtk_container_set_border_width( GTK_CONTAINER( _pControlPanel ), 10 );

    // add TourneyId chooser
    GtkWidget* _pTourneyIdChooser = gtk_box_new( GTK_ORIENTATION_HORIZONTAL, 10 );
    gtk_box_pack_start( GTK_BOX( _pTourneyIdChooser ), gtk_label_new( "Tourney Id: " ), FALSE, TRUE, 0 );
    gtk_box_pack_start( GTK_BOX( _pTourneyIdChooser ), pTourneyIdEntry_, TRUE, TRUE, 0);
    GtkWidget* _pUpdate = gtk_button_new_with_label( "Update" );
    gtk_box_pack_start( GTK_BOX( _pTourneyIdChooser ), _pUpdate, FALSE, TRUE, 0 );

    gtk_entry_set_text( GTK_ENTRY( pTourneyIdEntry_ ), pParms_->get_tourney_id() );
    g_signal_connect( G_OBJECT( _pUpdate ), "clicked", G_CALLBACK( OnUpdateTourneyIdButtonClicked ), gpointer(this) );
    gtk_box_pack_start( GTK_BOX( _pControlPanel ), _pTourneyIdChooser, FALSE, TRUE, 0 );
 
    // add TemplateChooser with label
    GtkWidget* _pBoxTemplateChooser = gtk_box_new( GTK_ORIENTATION_HORIZONTAL, 10 );
    gtk_box_pack_start( GTK_BOX( _pBoxTemplateChooser ), gtk_label_new( "Template: " ), FALSE, FALSE, 0 );
    g_signal_connect( G_OBJECT( templateChooser_ ), "changed", G_CALLBACK( OnTemplateChooserChanged ), gpointer(this) );  
    gtk_box_pack_start( GTK_BOX( _pBoxTemplateChooser) , templateChooser_, TRUE, TRUE, 0 );
    gtk_box_pack_start( GTK_BOX( _pControlPanel ), _pBoxTemplateChooser, FALSE, TRUE, 0 );

    // add buttons
    GtkWidget* _pBoxButtons = gtk_box_new( GTK_ORIENTATION_HORIZONTAL, 10 );
    GtkWidget* _pButtonSave = gtk_button_new_with_label( "Save" );
    g_signal_connect( G_OBJECT( _pButtonSave ), "clicked", G_CALLBACK( OnSaveButtonClicked ), gpointer(this) );
    gtk_box_pack_start( GTK_BOX( _pBoxButtons) , _pButtonSave, TRUE, TRUE, 0 );  
    gtk_box_pack_start( GTK_BOX( _pControlPanel ), _pBoxButtons, FALSE, TRUE, 0 );  

    // add help window
    GtkWidget* _pScrolledHelpWindow = gtk_scrolled_window_new( NULL, NULL );
    GtkWidget* _pHelpWindow = gtk_text_view_new();
    gtk_scrolled_window_set_policy ( GTK_SCROLLED_WINDOW ( _pScrolledHelpWindow ), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC );
    gtk_container_add( GTK_CONTAINER( _pScrolledHelpWindow ), _pHelpWindow );
    pHelpBuffer_ = gtk_text_view_get_buffer( GTK_TEXT_VIEW( _pHelpWindow ) );
    gtk_box_pack_start( GTK_BOX( _pControlPanel ), gtk_label_new( "Help" ), FALSE, TRUE, 0 );
    gtk_box_pack_start( GTK_BOX( _pControlPanel ), _pScrolledHelpWindow, TRUE, TRUE, 0 ); 

    return _pControlPanel;
}

void Dashboard::OnTemplateChooserChanged( GtkComboBox* pComboBox, Dashboard* pDashboard )
{
 
    int _id = -1;
    gchar* _templateName = template_chooser_get_template_name( TEMPLATE_CHOOSER( pComboBox ) );
    gchar* _template = template_chooser_get_template( TEMPLATE_CHOOSER( pComboBox ), &_id );
    gchar* _helpText = template_chooser_get_help_text( TEMPLATE_CHOOSER( pComboBox ), NULL );

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

void Dashboard::OnUpdateTourneyIdButtonClicked( GtkButton* pButton, Dashboard* pDashboard )
{
    pDashboard->pParms_->set_tourney_id( gtk_entry_get_text( GTK_ENTRY( pDashboard->pTourneyIdEntry_ ) ) );;
    pDashboard->pLogger_->Info( g_strdup_printf( "Tourney id changed to %s", gtk_entry_get_text( GTK_ENTRY( pDashboard->pTourneyIdEntry_ ) ) ) );
}

void Dashboard::OnSubscribeButtonToggled( GtkToggleButton* pButton, Dashboard* pDashboard )
{
    gchar* _topic = topic_chooser_get_topic( TOPIC_CHOOSER( pDashboard->subscribeTopicChooser_ ) );
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

void Dashboard::OnPublishTopicChanged( TopicChooser* pTopicChooser, Dashboard* pDashboard )
{
    gchar* _topic = topic_chooser_get_topic_prefix( pTopicChooser );
    g_object_set( G_OBJECT( pDashboard->templateChooser_), "topic", _topic, NULL );
    g_free( _topic );
}

void Dashboard::OnSaveButtonClicked( GtkButton* pButton, Dashboard* pDashboard )
{
  
}

void Dashboard::OnSubscribeTopicSelectionChanged( TopicChooser* pTopicChooser, Dashboard* pDashboard )
{

}
