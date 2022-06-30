#include "TopicChooser.h"

TopicChooser::TopicChooser(std::shared_ptr<Parameters> pParms, std::shared_ptr<Logger> pLogger, TOPICS defaultTopic) :
    pParms_(pParms),
    pLogger_(pLogger),
    pParent_(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10)),
    pTopicPrefix_(gtk_combo_box_text_new()),
    pTableNumber_(gtk_spin_button_new_with_range(1, 99, 1))
{
    gtk_box_pack_start( GTK_BOX(pParent_), gtk_label_new("Topic: "), FALSE, TRUE, 0 );
    gtk_box_pack_start( GTK_BOX(pParent_), pTopicPrefix_, FALSE, TRUE, 0 );
    gtk_box_pack_start( GTK_BOX(pParent_), gtk_label_new("Table no: "), FALSE, TRUE, 0 );
    gtk_box_pack_start( GTK_BOX(pParent_), pTableNumber_, FALSE, TRUE, 0 );
    for ( auto _topic : topics_ )
    {
        gtk_combo_box_text_append_text( GTK_COMBO_BOX_TEXT(pTopicPrefix_), _topic );
    }

    gtk_combo_box_set_active( GTK_COMBO_BOX(pTopicPrefix_), defaultTopic );
  }

std::string TopicChooser::get_topic() const
{
    gchar* _topic = g_strdup_printf( "%s=%s-%d", 
        topics_[gtk_combo_box_get_active( GTK_COMBO_BOX(pTopicPrefix_) )], 
        pParms_->get_tourney_id().c_str(),
        g_str_has_suffix( topics_[gtk_combo_box_get_active( GTK_COMBO_BOX(pTopicPrefix_) )], "tb" ) ?
            gtk_spin_button_get_value_as_int( GTK_SPIN_BUTTON(pTableNumber_) ) : 0 ); 
    std::string _s(_topic);
    g_free(_topic);
    return _s;
}