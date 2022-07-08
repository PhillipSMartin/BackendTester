#pragma once

#include <gtk/gtk.h>
#include <set>

#include "JsonViewer.h"
#include "Logger.h"
#include "Parameters.h"
#include "TemplateChooser.h"
#include "TopicChooser.h"
#include "TourneyIdChooser.h"

typedef struct 
{
    public:
        GtkWidget* mainWindow_;

        // panes
        GtkWidget* verticalPane_;
        GtkWidget* horizontalPane_;
        GtkWidget* jsonViewerPane_;
     
        // viewers
        GtkWidget* subscribeJsonViewer_;
        GtkWidget* publishJsonViewer_;
        GtkWidget* consoleViewer_;
        GtkWidget* helpViewer_;

        // choosers
        GtkWidget* subscribeTopicChooser_;
        GtkWidget* publishTopicChooser_;
        GtkWidget* templateChooser_;
        GtkWidget* tourneyIdChooser_;

        // buttons
        GtkWidget* subscribeButton_;
        GtkWidget* publishButton_;
        GtkWidget* saveTemplateButton_;

        // panels
        GtkWidget* subscribePanel_;
        GtkWidget* publishPanel_;
        GtkWidget* controlPanel_;
        
} WidgetVault;


class Dashboard
{
    private:
        Parameters* pParms_;
        Logger* pLogger_;
        WidgetVault* pWidgets_; 

        std::set<std::string> subscribedTopics_;    // a list of topics we've subscribed to
        gulong subscribe_button_handler_id;         // handler id of OnSubscribeButtonClicked, which we use to block the signal

    public:
        Dashboard(Parameters* pParms, Logger* pLogger);
        ~Dashboard();

        void set_publish_message( const gchar* text, int id ) ;
        void set_subscribe_message( const gchar* text ); 
        void set_help_message( const gchar* text, int id ); 
        void run(); 
 
    private:
        void instantiate_widgets();
        void arrange_widgets();
        void wire_signals();

        void manage_subscribe_button_state( const gchar* topic );
        gchar* get_subscribed_topics_tooltip_text();

        GtkWidget* subscribe_panel_new(); 
        GtkWidget* publish_panel_new(); 
        GtkWidget* control_panel_new(); 

        static void OnSubscribeTopicChanged( TopicChooser* pTopicChooser, Dashboard* pDashboard );
        static void OnPublishTopicChanged( TopicChooser* pTopicChooser, Dashboard* pDashboard );
        static void OnTemplateChanged( TemplateChooser* pTemplateChooser, Dashboard* pDashboard ); 
        static void OnTourneyIdChanged( TourneyIdChooser* pTourneyIdChooser, Dashboard* pDashboard );
 
        static void OnSubscribeButtonClicked( GtkToggleButton* pButton, Dashboard* pDashboard );
        static bool OnSubscribeButtonToolTip( GtkWidget* pButton, gint x, gint y, gboolean keyboard_mode, GtkTooltip* pTooltip, Dashboard* pDashboard );
        static void OnPublishButtonClicked( GtkButton* pButton, Dashboard* pDashboard );
        static void OnSaveButtonClicked( GtkButton* pButton, Dashboard* pDashboard );

        static void OnJsonEdited( JsonViewer* pJsonViewer, Dashboard* pDashboard );
        static void OnJsonReplaced( JsonViewer* pJsonViewer, Dashboard* pDashboard );

        static void OnMainWindowDestroy( GtkWidget* pWindow, Logger* pLogger );
};