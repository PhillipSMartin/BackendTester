#include "TemplateChooser.h"

TemplateChooser::TemplateChooser(std::shared_ptr<Logger> pLogger) : 
    pComboBox_(gtk_combo_box_text_new()),
    pLogger_(pLogger)
{
}

void TemplateChooser::add_messages(std::vector<std::string> const& items) 
{
    clear_messages();
    for ( auto _item : items )
    {
        gtk_combo_box_text_append_text( GTK_COMBO_BOX_TEXT(pComboBox_), _item.c_str() );
    }
}

void TemplateChooser::clear_messages()
{
        gtk_combo_box_text_remove_all( GTK_COMBO_BOX_TEXT(pComboBox_) );
}
