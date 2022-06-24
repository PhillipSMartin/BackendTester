#pragma once

#include "Logger.h"
#include "nlohmann/json.hpp"

#include <gtk/gtk.h>
#include <memory>

class JsonTreeViewModel
{
    private:
        GtkWidget* pTreeView_;
        GtkTreeStore* pTreeStore_;
        std::shared_ptr<nlohmann::json> pJson_;
        std::shared_ptr<Logger> pLogger_;

        enum
        {
            KEY = 0,
            VALUE,
            KEY_PATH,
            EDITABLE,
            NUM_COLUMNS
        };

    public:
        JsonTreeViewModel(std::shared_ptr<Logger> pLogger);

        GtkWidget* const get_TreeView() const { return pTreeView_; }

        void set_pJson(std::shared_ptr<nlohmann::json> pJson);
        nlohmann::json* const get_pJson() const { return pJson_.get(); }

    private:
        static void OnCellEdited(GtkCellRendererText* renderer, gchar* path, gchar* newText, JsonTreeViewModel* model);
        static nlohmann::json* get_json_branch(nlohmann::json* const pJson, gchar* const keyPath);

        void fill_data(nlohmann::json* const pBranch, GtkTreeIter* const parent=NULL, gchar* const keyPath=NULL);
};