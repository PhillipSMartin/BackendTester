#pragma once

#include <gtk/gtk.h>
#include <map>
#include <set>

#include "JsonViewer.h"
#include "Logger.h"
#include "TemplateChooser.h"
#include "Parameters.h"
#include "TemplateMap.h"
#include "TopicChooser.h"

class Dashboard
{
    private:
        Parameters* pParms_;
        Logger* pLogger_;

        GtkWidget* pParent_; // a horizontal window with three panes

        // leftmost panel for subscriptions
        TopicChooser subscribeTopicChooser_; 
        GtkWidget* subscribeViewer_; 

        // middle panel for publishing
        TopicChooser publishTopicChooser_; 
        GtkWidget* publishViewer_; 

        // rightmost panel for controls
        GtkWidget* pTourneyIdEntry_; // an entry box for setting tourney id
        TemplateChooser templateChooser_; // a combo box for choosing a message template
        GtkTextBuffer* pHelpBuffer_; // a text buffer for displaying help about the current message
        GtkTextBuffer* pConsoleBuffer_; // a text buffer for displaying log messages

        std::set<std::string> subscribedTopics_;  // a list of topics we've subscribed to
        gboolean ignoreSubscribeButtonClick_ = FALSE; // set when we are changing the active state of the subscribe button programattically

        std::map<std::string, TemplateMap*> templateMaps_; // keyed by topic prefix

    public:
        Dashboard(Parameters* pParms, Logger* pLogger);
        ~Dashboard();
        
        GtkWidget* const get_parent() const { return pParent_; }

        void set_publish_message( std::shared_ptr<nlohmann::json> pJson, gboolean expandFirstRow = FALSE ) 
            { json_viewer_set_json_string( JSON_VIEWER( publishViewer_ ), -1, nlohmann::to_string( *pJson ).c_str() ); }
        void set_subscribe_message( std::shared_ptr<nlohmann::json> pJson, gboolean expandFirstRow = TRUE ) 
            { json_viewer_set_json_string( JSON_VIEWER( subscribeViewer_ ), -1, nlohmann::to_string( *pJson ).c_str() ); }
        void set_help_message( const gchar* helpText ) const { gtk_text_buffer_set_text( pHelpBuffer_, helpText, -1 ); }

    private:
        GtkWidget* subscribe_panel_new(); // build left panel of pParent
        GtkWidget* publish_panel_new(); // build middle panel of pParent
        GtkWidget* control_panel_new(); // build right panel of pParent
        GtkWidget* console_window_new(); 

        TemplateMap* get_template_map(gchar* const topic);
        TemplateMap* get_current_template_map() const;

        static void OnMessageSampleSelectionChanged( GtkComboBox* pComboBox, Dashboard* pDashboard ); 
        static void OnSubscribeButtonToggled( GtkToggleButton* pButton, Dashboard* pDashboard ) ;
        static void OnPublishButtonClicked();
        static void OnSaveButtonClicked( GtkButton* pButton, Dashboard* pDashboard );
        static void OnUpdateTourneyIdButtonClicked( GtkButton* pButton, Dashboard* pDashboard ) ;
        static void OnPublishTopicSelectionChanged( GtkComboBox* pComboBox, Dashboard* pDashboard );
        static void OnSubscribeTopicSelectionChanged( GtkComboBox* pComboBox, Dashboard* pDashboard );
};