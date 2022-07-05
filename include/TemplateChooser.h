#pragma once
#include <gtk/gtk.h>
#include <string>
#include <vector>

#include "Logger.h"

class TemplateChooser
{
    private:
        GtkWidget* pComboBox_;
        Logger* pLogger_;

    public:
        TemplateChooser(Logger* pLogger);
        void add_messages( std::vector<std::string> const& items );
        void clear_messages();

        GtkWidget* const get_combo_box() const { return pComboBox_; }
};