#pragma once

#include <gtk/gtk.h>
#include <map>
#include <set>

#include "JsonTreeView.h"
#include "Logger.h"
#include "TemplateChooser.h"
#include "Parameters.h"
#include "TemplateMap.h"
#include "TopicChooser.h"

class MessageWindow
{
    private:
        std::shared_ptr<Parameters> pParms_;
        std::shared_ptr<Logger> pLogger_;

        GtkWidget* pParent_; // a horizontal window with three panes

        // leftmost panel for subscriptions
        std::shared_ptr<TopicChooser> pSubscribeTopicChooser_; 
        std::shared_ptr<JsonTreeView> pSubscribeTreeView_; 

        // middle panel for publishing
        std::shared_ptr<TopicChooser> pPublishTopicChooser_; 
        std::shared_ptr<JsonTreeView> pPublishTreeView_; 

        // rightmost panel for controls
        GtkWidget* pTourneyIdEntry_; // an entry box for setting tourney id
        std::shared_ptr<TemplateChooser> pTemplateChooser_; // a combo box for choosing a message template
        GtkTextBuffer* pHelpBuffer_; // a text buffer for displaying help about the current message
        GtkTextBuffer* pConsoleBuffer_; // a text buffer for displaying log messages

        std::set<std::string> subscribedTopics_;  // a list of topics we've subscribed to
        gboolean ignoreSubscribeButtonClick_ = FALSE; // set when we are changing the active state of the subscribe button programattically

        std::map<std::string, std::shared_ptr<TemplateMap>> templateMaps_; // keyed by topic prefix

    public:
        MessageWindow(std::shared_ptr<Parameters> pParms, std::shared_ptr<Logger> pLogger);
        GtkWidget* const get_parent() const { return pParent_; }

        void set_publish_message( std::shared_ptr<nlohmann::json> pJson, gboolean expandFirstRow = FALSE ) const { pPublishTreeView_->set_message( pJson, expandFirstRow ); }
        void set_subscribe_message( std::shared_ptr<nlohmann::json> pJson, gboolean expandFirstRow = TRUE ) const { pSubscribeTreeView_->set_message( pJson, expandFirstRow ); }
        void set_help_message( const gchar* helpText ) const { gtk_text_buffer_set_text( pHelpBuffer_, helpText, -1 ); }

    private:
        GtkWidget* subscribe_panel_new(); // build left panel of pParent
        GtkWidget* publish_panel_new(); // build middle panel of pParent
        GtkWidget* control_panel_new(); // build right panel of pParent
        GtkWidget* console_window_new(); 

        std::shared_ptr<TemplateMap> get_template_map(gchar* const topic);
        std::shared_ptr<TemplateMap> get_current_template_map() const;

        static void OnMessageSampleSelectionChanged(GtkComboBox* pComboBox, MessageWindow* pMessageWindow); 
        static void OnSubscribeButtonToggled(GtkToggleButton* pButton, MessageWindow* pMessageWindow);
        static void OnPublishButtonClicked();
        static void OnSaveButtonClicked();
        static void OnUpdateTourneyIdButtonClicked(GtkButton* pButton, MessageWindow* pMessageWindow);
        static void OnPublishTopicSelectionChanged(GtkComboBox* pComboBox, MessageWindow* pMessageWindow);
        static void OnSubscribeTopicSelectionChanged(GtkComboBox* pComboBox, MessageWindow* pMessageWindow);
};