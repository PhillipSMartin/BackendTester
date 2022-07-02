#pragma once

#include <gtk/gtk.h>

#include "Logger.h"
#include "nlohmann/json.hpp"

class JsonTreeView
{
    private:
        Logger* pLogger_;

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
        JsonTreeView(Logger* pLogger);

        GtkWidget* const get_parent() const { return pParent_; }

        void set_contents( std::shared_ptr<nlohmann::json> pJson, gboolean expandFirstRow = FALSE );
        std::string get_json_string() const { return nlohmann::to_string( *pJson_ ); }
 
    private:
        static void OnCellEdited( GtkCellRendererText* renderer, gchar* path, gchar* newText, JsonTreeView* treeView );
        static nlohmann::json* const get_json_branch( nlohmann::json* const pJson, const gchar* keyPath );

        void fill_data( nlohmann::json* const pBranch, GtkTreeIter* const parent = NULL, const gchar* keyPath = NULL );
};