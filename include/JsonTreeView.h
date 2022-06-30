#pragma once

#include "Logger.h"
#include "nlohmann/json.hpp"

#include <gtk/gtk.h>
#include <memory>

class JsonTreeView
{
    private:
        std::shared_ptr<Logger> pLogger_;

        GtkWidget* pParent_;                    // a scrolled window to contain the tree view
        GtkWidget* pTreeView_;                  // the tree view
        GtkTreeStore* pTreeStore_;              // the model associated with the tree view
        std::shared_ptr<nlohmann::json> pJson_; // the json message to be displayed in the tree view

        enum
        {
            KEY = 0,
            VALUE,
            KEY_PATH,
            EDITABLE,
            NUM_COLUMNS
        };

    public:
        JsonTreeView(std::shared_ptr<Logger> pLogger);

        GtkWidget* const get_parent() const { return pParent_; }

        void set_message( std::shared_ptr<nlohmann::json> pJson, gboolean expandFirstRow = FALSE );
        nlohmann::json* const get_pJson() const { return pJson_.get(); }

    private:
        static void OnCellEdited( GtkCellRendererText* renderer, gchar* path, gchar* newText, JsonTreeView* model );
        static nlohmann::json* const get_json_branch( nlohmann::json* const pJson, const gchar* keyPath );

        void fill_data( nlohmann::json* const pBranch, GtkTreeIter* const parent = NULL, const gchar* keyPath = NULL );
};