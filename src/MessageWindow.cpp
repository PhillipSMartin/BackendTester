#include "MessageWindow.h"

MessageWindow::MessageWindow(std::shared_ptr<Parameters> pParms, std::shared_ptr<Logger> pLogger) :
    pParms_(pParms),
    pLogger_(pLogger),
    pParent_(gtk_paned_new( GTK_ORIENTATION_VERTICAL )),
    pSubscribeTopicChooser_(std::make_shared<TopicChooser>(pParms, pLogger, TopicChooser::STATE_TB)),
    pSubscribeTreeView_(std::make_shared<JsonTreeView>(pLogger)),
    pPublishTopicChooser_(std::make_shared<TopicChooser>(pParms, pLogger, TopicChooser::EVENT_TB)),
    pPublishTreeView_(std::make_shared<JsonTreeView>(pLogger)),
    pTourneyIdEntry_(gtk_entry_new())
{
    gtk_widget_set_size_request( pParent_, 1200, 800 );  

    // top panel will contain three horizontal panels
    GtkWidget* _panels = gtk_paned_new( GTK_ORIENTATION_HORIZONTAL );
    gtk_paned_add1( GTK_PANED(pParent_), _panels );

    // two leftmost panels contain tree views
    GtkWidget* _treeViewPanels = gtk_paned_new( GTK_ORIENTATION_HORIZONTAL );
    gtk_paned_add1( GTK_PANED(_treeViewPanels), subscribe_panel_new() );
    gtk_paned_add2( GTK_PANED(_treeViewPanels), publish_panel_new() );
    gtk_paned_set_position(GTK_PANED(_treeViewPanels), 400); 

    // rightmost panel contains controls
    gtk_paned_add1( GTK_PANED(_panels), _treeViewPanels);
    gtk_paned_add2( GTK_PANED(_panels), control_panel_new() );
    gtk_paned_set_position(GTK_PANED(_panels), 820); 

    // bottom panel will be console window
    gtk_paned_add2( GTK_PANED(pParent_),  console_window_new());
    gtk_paned_set_position(GTK_PANED(pParent_), 650); 

    // populate template combo box
    g_signal_emit_by_name( G_OBJECT(pPublishTopicChooser_->get_combo_box()), "changed" );
}

GtkWidget* MessageWindow::console_window_new()
{
    GtkWidget* _pScrolledConsoleWindow = gtk_scrolled_window_new( NULL, NULL );
    GtkWidget* _pConsoleWindow = gtk_text_view_new();
    gtk_scrolled_window_set_policy ( GTK_SCROLLED_WINDOW (_pScrolledConsoleWindow), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC );
    gtk_container_add( GTK_CONTAINER(_pScrolledConsoleWindow), _pConsoleWindow );
    pConsoleBuffer_ = gtk_text_view_get_buffer( GTK_TEXT_VIEW(_pConsoleWindow) );
    pLogger_->set_text_buffer( pConsoleBuffer_ );

    return _pScrolledConsoleWindow;
}

GtkWidget* MessageWindow::subscribe_panel_new()
{
    GtkWidget* _pSubscribePanel = gtk_box_new( GTK_ORIENTATION_VERTICAL, 10 ); 
    gtk_container_set_border_width( GTK_CONTAINER(_pSubscribePanel), 10 );

    // add topic controls
    GtkWidget* _pSubscribeButton = gtk_toggle_button_new_with_label( "Subscribe" );
    g_signal_connect( G_OBJECT(_pSubscribeButton), "toggled", G_CALLBACK(OnSubscribeButtonToggled), gpointer(this));
    gtk_box_pack_start( GTK_BOX(_pSubscribePanel), _pSubscribeButton, FALSE, TRUE, 0 );

    gtk_box_pack_start( GTK_BOX(_pSubscribePanel), pSubscribeTopicChooser_->get_parent(), FALSE, TRUE, 0 );
    gtk_box_pack_start( GTK_BOX(_pSubscribePanel), pSubscribeTreeView_->get_parent(), TRUE, TRUE, 0 );

    g_signal_connect( G_OBJECT(pSubscribeTopicChooser_->get_combo_box()), "changed", G_CALLBACK(OnSubscribeTopicSelectionChanged), gpointer(this) );
    return _pSubscribePanel;
}

GtkWidget* MessageWindow::publish_panel_new()
{
    GtkWidget* _pPublishPanel = gtk_box_new( GTK_ORIENTATION_VERTICAL, 10 ); 
    gtk_container_set_border_width( GTK_CONTAINER(_pPublishPanel), 10 );

    // add topic controls
    GtkWidget* _pPublishButton = gtk_button_new_with_label( "Publish" );
    gtk_box_pack_start( GTK_BOX(_pPublishPanel), _pPublishButton, FALSE, TRUE, 0 );

    gtk_box_pack_start( GTK_BOX(_pPublishPanel), pPublishTopicChooser_->get_parent(), FALSE, TRUE, 0 );
    gtk_box_pack_start( GTK_BOX(_pPublishPanel), pPublishTreeView_->get_parent(), TRUE, TRUE, 0 );

    g_signal_connect( G_OBJECT(pPublishTopicChooser_->get_combo_box()), "changed", G_CALLBACK(OnPublishTopicSelectionChanged), gpointer(this) );
    return _pPublishPanel;
}

GtkWidget* MessageWindow::control_panel_new()
{
    GtkWidget* _pControlPanel = gtk_box_new( GTK_ORIENTATION_VERTICAL, 10 ); 
    gtk_container_set_border_width( GTK_CONTAINER(_pControlPanel), 10 );

    // add TourneyId chooser
    GtkWidget* _pTourneyIdChooser = gtk_box_new( GTK_ORIENTATION_HORIZONTAL, 10 );
    gtk_box_pack_start( GTK_BOX(_pTourneyIdChooser), gtk_label_new( "Tourney Id: " ), FALSE, TRUE, 0 );
    gtk_box_pack_start( GTK_BOX(_pTourneyIdChooser), pTourneyIdEntry_, TRUE, TRUE, 0);
    GtkWidget* _pUpdate = gtk_button_new_with_label("Update");
    gtk_box_pack_start( GTK_BOX(_pTourneyIdChooser), _pUpdate, FALSE, TRUE, 0);

    gtk_entry_set_text( GTK_ENTRY(pTourneyIdEntry_), pParms_->get_tourney_id().c_str() );
    g_signal_connect( G_OBJECT(_pUpdate), "clicked", G_CALLBACK(OnUpdateTourneyIdButtonClicked), gpointer(this) );
    gtk_box_pack_start( GTK_BOX(_pControlPanel), _pTourneyIdChooser, FALSE, TRUE, 0 );
 
    // add TemplateChooser with label
    GtkWidget* _pBoxTemplateChooser = gtk_box_new( GTK_ORIENTATION_HORIZONTAL, 10 );
    gtk_box_pack_start( GTK_BOX(_pBoxTemplateChooser), gtk_label_new( "Template: " ), FALSE, FALSE, 0 );
    pTemplateChooser_ = std::make_shared<TemplateChooser>(pLogger_);
    g_signal_connect( G_OBJECT(pTemplateChooser_->get_combo_box()), "changed", G_CALLBACK(OnMessageSampleSelectionChanged), gpointer(this) );  
    gtk_box_pack_start( GTK_BOX(_pBoxTemplateChooser), pTemplateChooser_->get_combo_box(), TRUE, TRUE, 0 );
    gtk_box_pack_start( GTK_BOX(_pControlPanel), _pBoxTemplateChooser, FALSE, TRUE, 0 );

    // add buttons
    GtkWidget* _pBoxButtons = gtk_box_new( GTK_ORIENTATION_HORIZONTAL, 10 );
    GtkWidget* _pButtonSave = gtk_button_new_with_label("Save");
    gtk_box_pack_start( GTK_BOX(_pBoxButtons), _pButtonSave, TRUE, TRUE, 0 );  
    gtk_box_pack_start( GTK_BOX(_pControlPanel), _pBoxButtons, FALSE, TRUE, 0 );  

    // add help window
    GtkWidget* _pScrolledHelpWindow = gtk_scrolled_window_new( NULL, NULL );
    GtkWidget* _pHelpWindow = gtk_text_view_new();
    gtk_scrolled_window_set_policy ( GTK_SCROLLED_WINDOW (_pScrolledHelpWindow), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC );
    gtk_container_add( GTK_CONTAINER(_pScrolledHelpWindow), _pHelpWindow );
    pHelpBuffer_ = gtk_text_view_get_buffer( GTK_TEXT_VIEW(_pHelpWindow) );
    gtk_box_pack_start( GTK_BOX(_pControlPanel), gtk_label_new( "Help" ), FALSE, TRUE, 0 );
    gtk_box_pack_start( GTK_BOX(_pControlPanel), _pScrolledHelpWindow, TRUE, TRUE, 0 ); 

    return _pControlPanel;
}

void MessageWindow::OnMessageSampleSelectionChanged(GtkComboBox* pComboBox, MessageWindow* pMessageWindow)
{
    auto _templateMap = pMessageWindow->get_current_template_map();
    if ( _templateMap )
    { 
        gchar* _id = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(pComboBox)); 
        gchar* _msg;    
        pMessageWindow->pLogger_->Debug( _msg = g_strdup_printf( "Template changed to %s", _id ) );
        pMessageWindow->set_help_message( _templateMap->get_help ( _id ).c_str() );
        try
        {
            pMessageWindow->set_publish_message( std::make_shared<nlohmann::json>(nlohmann::json::parse( _templateMap->get_template ( _id ) )) );
        }
        catch(const std::exception& e)
        {
           pMessageWindow->pLogger_->Debug( _msg = g_strdup_printf( "Unable to parse json string: %s", _templateMap->get_template ( _id ).c_str() ) );
        }
        
        

        g_free( _id );
        g_free( _msg );
    }
}

void MessageWindow::OnUpdateTourneyIdButtonClicked(GtkButton* pButton, MessageWindow* pMessageWindow)
{
    pMessageWindow->pParms_->set_tourney_id( gtk_entry_get_text( GTK_ENTRY(pMessageWindow->pTourneyIdEntry_) ) );
    gchar* _msg = g_strdup_printf( "Tourney id changed to %s", gtk_entry_get_text( GTK_ENTRY(pMessageWindow->pTourneyIdEntry_) ) );
    pMessageWindow->pLogger_->Info( _msg );
    g_free( _msg );
}

void MessageWindow::OnSubscribeButtonToggled(GtkToggleButton* pButton, MessageWindow* pMessageWindow)
{
    if ( gtk_toggle_button_get_active( pButton))
    {
        gtk_button_set_label( GTK_BUTTON(pButton), "Unsubscribe" );
        if (!pMessageWindow->ignoreSubscribeButtonClick_) // if TRUE, we set the state -- user did not click
        {
            // TODO subscribe
            std::string _topic = pMessageWindow->pSubscribeTopicChooser_->get_topic();
            pMessageWindow->subscribedTopics_.insert( _topic );
            pMessageWindow->pLogger_->Info( "Subscribed to " + _topic );
        }
    }
    else
    {
        gtk_button_set_label( GTK_BUTTON(pButton), "Subscribe" );
        if (!pMessageWindow->ignoreSubscribeButtonClick_) // if TRUE, we set the state -- user did not click
        {
           // TODO subscribe
            std::string _topic = pMessageWindow->pSubscribeTopicChooser_->get_topic();
            pMessageWindow->subscribedTopics_.erase( _topic );
            pMessageWindow->pLogger_->Info( "Unsubscribed from " + _topic );
        }
   }
}

std::shared_ptr<TemplateMap> MessageWindow::get_template_map(gchar* const topic)
{
    std::shared_ptr<TemplateMap> _map = NULL;

    auto _it = templateMaps_.find( topic );
    if ( _it == templateMaps_.end() )
    {
        // try to create map from templates file
        std::string _fileName(topic);
        _fileName += ".templates.txt";

        _map = std::make_shared<TemplateMap>(pLogger_);
        if ( !_map->import_file(_fileName) )
            return NULL; // error will be logged by import_file

        templateMaps_[topic] = _map;
    }
    else
    {
        _map = _it->second;
    }

    return _map;
}

std::shared_ptr<TemplateMap> MessageWindow::get_current_template_map() const
{
    std::shared_ptr<TemplateMap> _map = NULL;
    gchar* _topic = gtk_combo_box_text_get_active_text( GTK_COMBO_BOX_TEXT(pPublishTopicChooser_->get_combo_box()) );
    auto _it = templateMaps_.find( _topic );
    if ( _it != templateMaps_.end() )
    {
        _map = _it->second;
    }
    g_free(_topic);   
    return _map;
}

void MessageWindow::OnPublishTopicSelectionChanged(GtkComboBox* pComboBox, MessageWindow* pMessageWindow)
{
    gchar* _topic = gtk_combo_box_text_get_active_text( GTK_COMBO_BOX_TEXT(pComboBox) );
    auto _map = pMessageWindow->get_template_map( _topic );
    g_free(_topic);

    if (_map)
    {
        pMessageWindow->pTemplateChooser_->add_messages(_map->get_keys());
        pMessageWindow->pLogger_->Info( "Now publishing to topic " + pMessageWindow->pPublishTopicChooser_->get_topic() );
    }
    else
    {
        pMessageWindow->pTemplateChooser_->clear_messages();
        pMessageWindow->pLogger_->Info( "Unable to publish to topic " + pMessageWindow->pPublishTopicChooser_->get_topic() + " - no template file" );
    }
}

void MessageWindow::OnSubscribeTopicSelectionChanged(GtkComboBox* pComboBox, MessageWindow* pMessageWindow)
{

}
