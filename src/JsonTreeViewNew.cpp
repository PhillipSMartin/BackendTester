#include "JsonTreeView.h"
#include "nlohmann/json.hpp"

#define JSON_TREE_VIEW_GET_PRIVATE( obj ) ( G_TYPE_INSTANCE_GET_PRIVATE( (obj), JSON_TREE_VIEW_TYPE, JsonTreeViewPrivate ) )

typedef struct _JsonTreeViewPrivate JsonTreeViewPrivate;

struct _JsonTreeViewPrivate
{
    nlohmann::json json_;
};

enum
{
    EDITED_SIGNAL,
    REPLACED_SIGNAL,
    LAST_SIGNAL
};

enum
{
    KEY = 0,
    VALUE,
    KEY_PATH,
    EDITABLE,
    NUM_COLUMNS
};

static guint json_tree_view_signals[LAST_SIGNAL] = { 0 };

GType json_tree_view_get_type( void )
{
    static GType _json_tree_view_type = 0;

    if ( !_json_tree_view_type )
    {
        static const GTypeInfo _json_tree_view_info = 
        {
            sizeof( JsonTreeViewClass ),
            NULL,
            NULL,
            (GClassInitFunc) json_tree_view_class_init,
            NULL,
            NULL,
            sizeof( JsonTreeView ),
            0,
            (GInstanceInitFunc) json_tree_view_init,
        };

        _json_tree_view_type = g_type_register_static( GTK_TYPE_TREE_VIEW, "JsonTreeView", &_json_tree_view_info, (GTypeFlags)0 );
    }

    return _json_tree_view_type;
}

static void json_tree_view_class_init( JsonTreeViewClass* klass )
{
    GObjectClass* gobject_class = G_OBJECT_CLASS( klass );

    // add private data
    g_type_class_add_private( klass, sizeof( JsonTreeViewPrivate ) );

    // register the signals
    json_tree_view_signals[EDITED_SIGNAL] = 
        g_signal_new( "json_edited", G_TYPE_FROM_CLASS( klass ),
            (GSignalFlags)(G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION),
            G_STRUCT_OFFSET( JsonTreeViewClass, json_edited ),
            NULL,
            NULL,
            g_cclosure_marshal_VOID__VOID,
            G_TYPE_NONE,
            0 );
    json_tree_view_signals[REPLACED_SIGNAL] = 
        g_signal_new( "json_replaced", G_TYPE_FROM_CLASS( klass ),
            (GSignalFlags)(G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION),
            G_STRUCT_OFFSET( JsonTreeViewClass, json_replaced ),
            NULL,
            NULL,
            g_cclosure_marshal_VOID__VOID,
            G_TYPE_NONE,
            0 );
}

static void json_tree_view_init( JsonTreeView* jsonTreeView )
{
    JsonTreeViewPrivate* priv = JSON_TREE_VIEW_GET_PRIVATE( jsonTreeView );
    priv->json_ = nlohmann::json();

    // add columns to tree view
    GtkCellRenderer* _renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_append_column( GTK_TREE_VIEW( jsonTreeView ), 
        gtk_tree_view_column_new_with_attributes( "Key", _renderer, "text", KEY, NULL ) );

    _renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_append_column( GTK_TREE_VIEW( jsonTreeView ),     
        gtk_tree_view_column_new_with_attributes( "Value", _renderer, "text", VALUE, "editable", EDITABLE, NULL ) );

    // wire renderer's "edited" signal
    g_signal_connect( G_OBJECT( _renderer ), "edited", G_CALLBACK( OnCellEdited ), gpointer(jsonTreeView) );
}

static void OnCellEdited( GtkCellRendererText* renderer, gchar* path, gchar* newText, JsonTreeView* treeView )
{

}
