#pragma once

#include "nlohmann/json.hpp"

#include <gtk/gtk.h>

class JsonTreeViewModel
{
    private:
        GtkWidget* pTreeView_;
        GtkTreeStore* pTreeStore_;
        nlohmann::json* pJson_;
        enum
        {
            KEY = 0,
            VALUE,
            KEY_PATH,
            POINTER,
            TYPE,
            NUM_COLUMNS
        };

    public:
        JsonTreeViewModel(nlohmann::json* pJson) : 
            pTreeView_(gtk_tree_view_new()),
            pTreeStore_(gtk_tree_store_new( NUM_COLUMNS, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_POINTER, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_INT, G_TYPE_BOOLEAN)),
            pJson_(pJson) 
        {
            gtk_tree_view_set_model(GTK_TREE_VIEW (pTreeView_), GTK_TREE_MODEL (pTreeStore_));
        }

        GtkWidget* const get_TreeView() const { return pTreeView_; }
        GtkTreeStore* const get_TreeStore() const { return pTreeStore_; }
        nlohmann::json* const get_Json() const { return pJson_; }
};