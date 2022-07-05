#include "JsonTreeStore.h"
#include "JsonViewer.h"
#include "nlohmann/json.hpp"

#define JSON_VIEWER_GET_PRIVATE( obj ) ( G_TYPE_INSTANCE_GET_PRIVATE( (obj), JSON_VIEWER_TYPE, JsonViewerPrivate ) )

typedef struct _JsonViewerPrivate JsonViewerPrivate;

struct _JsonViewerPrivate
{
    Logger* pLogger_;
    GtkWidget* pTreeView_;
    JsonTreeStore* pTreeStore_;
    gboolean expandFirstRow_;
    int jsonId;     // identifies json string so caller can be sure this is his string
};

// signals emitted
enum
{
    EDITED_SIGNAL,
    REPLACED_SIGNAL,
    LAST_SIGNAL
};

static guint json_viewer_signals[LAST_SIGNAL] = { 0 };

// Private function prototypes
static void json_viewer_class_init( JsonViewerClass* klass );
static void json_viewer_init( JsonViewer* jsonViewer );
static void json_viewer_on_cell_edited( GtkCellRendererText* renderer, gchar* path, gchar* newText, JsonViewer* jsonViewer );
static void json_viewer_on_json_edited( JsonTreeStore* jsonTreeStore, JsonViewer* jsonViewer );
static void json_viewer_on_json_replaced( JsonTreeStore* jsonTreeStore, JsonViewer* jsonViewer );

// Type registration
GType json_viewer_get_type( void )
{
    static GType _json_viewer_type = 0;

    if ( !_json_viewer_type )
    {
        static const GTypeInfo _json_viewer_info =
        {
            sizeof( JsonViewerClass ),
            NULL,
            NULL,
            (GClassInitFunc) json_viewer_class_init,
            NULL,
            NULL,
            sizeof( JsonViewer ),
            0,
            (GInstanceInitFunc) json_viewer_init,
        };

        _json_viewer_type = g_type_register_static( GTK_TYPE_SCROLLED_WINDOW, "JsonViewer", &_json_viewer_info, (GTypeFlags)0 );
    }

    return _json_viewer_type;
}

static void json_viewer_on_cell_edited( GtkCellRendererText* renderer, gchar* path, gchar* newText, JsonViewer* jsonViewer )
{
    JsonViewerPrivate* _priv = JSON_VIEWER_GET_PRIVATE( jsonViewer );
    json_tree_store_edit_json_string( _priv->pTreeStore_, path, newText );
}

static void json_viewer_on_json_edited( JsonTreeStore* jsonTreeStore, JsonViewer* jsonViewer )
{
    g_signal_emit_by_name( (gpointer)jsonViewer, "json-edited" );
}

static void json_viewer_on_json_replaced( JsonTreeStore* jsonTreeStore, JsonViewer* jsonViewer )
{
    g_signal_emit_by_name( (gpointer)jsonViewer, "json-replaced" );
}

// Initialization functions
static void json_viewer_class_init( JsonViewerClass* klass )
{
    GObjectClass* gobject_class = G_OBJECT_CLASS( klass );

    // add private data
    g_type_class_add_private( klass, sizeof( JsonViewerPrivate ) );


    // register the signals
    json_viewer_signals[EDITED_SIGNAL] =
            g_signal_new( "json-edited", G_TYPE_FROM_CLASS( klass ),
            (GSignalFlags)(G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION),
            G_STRUCT_OFFSET( JsonViewerClass, json_edited ),
            NULL,
            NULL,
            g_cclosure_marshal_VOID__VOID,
            G_TYPE_NONE,
            0 );
    json_viewer_signals[REPLACED_SIGNAL] = 
        g_signal_new( "json-replaced", G_TYPE_FROM_CLASS( klass ),
            (GSignalFlags)(G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION),
            G_STRUCT_OFFSET( JsonViewerClass, json_replaced ),
            NULL,
            NULL,
            g_cclosure_marshal_VOID__VOID,
            G_TYPE_NONE,
            0 );
}

static void json_viewer_init( JsonViewer* jsonViewer )
{
    JsonViewerPrivate* _priv = JSON_VIEWER_GET_PRIVATE( jsonViewer );

    gtk_scrolled_window_set_policy( GTK_SCROLLED_WINDOW( jsonViewer ), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC );

    // add tree view
    _priv->pTreeView_ = gtk_tree_view_new();
    gtk_container_add( GTK_CONTAINER( jsonViewer ), _priv->pTreeView_ );

    // add columns to tree view
    GtkCellRenderer* _renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_append_column( GTK_TREE_VIEW( _priv->pTreeView_ ), 
        gtk_tree_view_column_new_with_attributes( "Key", _renderer, "text", KEY, NULL ) );

    _renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_append_column( GTK_TREE_VIEW( _priv->pTreeView_ ),     
        gtk_tree_view_column_new_with_attributes( "Value", _renderer, "text", VALUE, "editable", EDITABLE, NULL ) );

    // wire signals
    g_signal_connect( G_OBJECT( _renderer ), "edited", G_CALLBACK( json_viewer_on_cell_edited ), gpointer(jsonViewer) );
}

// Public functions
GtkWidget* json_viewer_new( Logger* pLogger, gboolean expandFirstRow )
{
    GtkWidget* _pViewer = GTK_WIDGET( g_object_new( json_viewer_get_type(), NULL ) );
    JsonViewerPrivate* _priv = JSON_VIEWER_GET_PRIVATE( _pViewer );
    _priv->pLogger_ = pLogger;
    _priv->expandFirstRow_ = expandFirstRow;

    // add model to tree view - must be done here because we did not have pLogger during json_viewer_init
    _priv->pTreeStore_ = json_tree_store_new( _priv->pLogger_ );
    gtk_tree_view_set_model( GTK_TREE_VIEW( _priv->pTreeView_ ), GTK_TREE_MODEL(  _priv->pTreeStore_ ) );
    g_object_unref(  _priv->pTreeStore_ );

    g_signal_connect( G_OBJECT(_priv->pTreeStore_ ), "json-edited", G_CALLBACK( json_viewer_on_json_edited ), gpointer(_pViewer) );
    g_signal_connect( G_OBJECT(_priv->pTreeStore_ ), "json-replaced", G_CALLBACK( json_viewer_on_json_replaced ), gpointer(_pViewer) );

    return _pViewer;
} 

gchar* json_viewer_get_json_string( JsonViewer* jsonViewer , int* id )
{
     JsonViewerPrivate* _priv = JSON_VIEWER_GET_PRIVATE( jsonViewer ); 
     if ( id != NULL )
     {
        *id = _priv-> jsonId;
     }  
     return json_tree_store_get_json_string( _priv->pTreeStore_ );
}

void json_viewer_set_json_string( JsonViewer* jsonViewer , int id, const gchar* text )
{
     JsonViewerPrivate* _priv = JSON_VIEWER_GET_PRIVATE( jsonViewer ); 
     _priv-> jsonId = id;
     json_tree_store_set_json_string( _priv->pTreeStore_, text );  

    if ( _priv->expandFirstRow_ )
    {
        GtkTreePath* _firstRow = gtk_tree_path_new_first();
        gtk_tree_view_expand_row( GTK_TREE_VIEW( _priv->pTreeView_ ) , _firstRow, FALSE );
        gtk_tree_path_free( _firstRow );
    } 
}

