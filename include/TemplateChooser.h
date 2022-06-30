#pragma once
#include <gtk/gtk.h>
#include <memory>
#include <string>
#include <vector>

#include "JsonTreeView.h"
#include "Logger.h"

class TemplateChooser
{
    private:
        GtkWidget* pComboBox_;
        std::shared_ptr<Logger> pLogger_;

    public:
        TemplateChooser(std::shared_ptr<Logger> pLogger);
        void add_messages(std::vector<std::string> const& items);
        void clear_messages();

        GtkWidget* const get_combo_box() const { return pComboBox_; }
};