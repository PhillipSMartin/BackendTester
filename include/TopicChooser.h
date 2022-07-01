#pragma once

#include <gtk/gtk.h>

#include "Logger.h"
#include "Parameters.h"

class TopicChooser
{
    private:
        Parameters* pParms_;
        Logger* pLogger_;
        GtkWidget* pParent_;
        GtkWidget* pTopicPrefix_;
        GtkWidget* pTableNumber_;

        const char* topics_[4] = { "Event.tb", "Event.tm", "State.tb", "State.tm" };

    public:
        enum TOPICS 
        {
            EVENT_TB = 0,
            EVENT_TM,
            STATE_TB,
            STATE_TM
        };
       
        TopicChooser(Parameters* pParms, Logger* pLogger, TOPICS defaultTopic);
        GtkWidget* get_parent() const { return pParent_; }
        GtkWidget* get_combo_box() const { return pTopicPrefix_; }
        std::string get_topic() const;
};