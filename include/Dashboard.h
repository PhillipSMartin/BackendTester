#pragma once

#include <gtk/gtk.h>
#include <set>

#include "Logger.h"
#include "TemplateChooser.h"
#include "Parameters.h"
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

        std::set<std::string> subscribedTopics_;        // a list of topics we've subscribed to
        gboolean ignoreSubscribeButtonClick_ = FALSE;   // set when we are changing the active state of the subscribe button programattically

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

        GtkWidget* subscribe_panel_new(); 
        GtkWidget* publish_panel_new(); 
        GtkWidget* control_panel_new(); 

        static void OnSubscribeTopicChanged( TopicChooser* pTopicChooser, Dashboard* pDashboard );
        static void OnPublishTopicChanged( TopicChooser* pTopicChooser, Dashboard* pDashboard );
        static void OnMessageTemplateChanged( TemplateChooser* pTemplateChooser, Dashboard* pDashboard ); 
        static void OnTourneyIdChanged( TourneyIdChooser* pTourneyIdChooser, Dashboard* pDashboard );
 
        static void OnSubscribeButtonClicked( GtkToggleButton* pButton, Dashboard* pDashboard );
        static void OnPublishButtonClicked( GtkButton* pButton, Dashboard* pDashboard );
        static void OnSaveButtonClicked( GtkButton* pButton, Dashboard* pDashboard );

        static void OnMainWindowDestroy( GtkWidget* pWindow, Logger* pLogger );
};