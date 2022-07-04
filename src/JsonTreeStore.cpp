#include "JsonTreeStore.h"
#include "nlohmann/json.hpp"

#define JSON_TREE_STORE_GET_PRIVATE( obj ) ( G_TYPE_INSTANCE_GET_PRIVATE( (obj), JSON_TREE_STORE_TYPE, JsonTreeStorePrivate ) )

typedef struct _JsonTreeStorePrivate JsonTreeStorePrivate;

struct _JsonTreeStorePrivate
{
    nlohmann::json* pJson_;
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

static void json_tree_store_class_init( JsonTreeStoreClass* klass );
static void json_tree_store_init( JsonTreeStore* jsonTreeStore );
static void json_tree_store_fill_data(JsonTreeStore* jsonTreeStore, nlohmann::json* const pBranch, GtkTreeIter* const parent = NULL, const gchar* keyPath = NULL );

static guint json_tree_store_signals[LAST_SIGNAL] = { 0 };

GType json_tree_store_get_type( void )
{
    static GType _json_tree_store_type = 0;

    if ( !_json_tree_store_type )
    {
        static const GTypeInfo _json_tree_store_info =
       {
            sizeof( JsonTreeStoreClass ),
            NULL,
            NULL,
            (GClassInitFunc) json_tree_store_class_init,
            NULL,
            NULL,
            sizeof( JsonTreeStore ),
            0,
            (GInstanceInitFunc) json_tree_store_init,
        };

        _json_tree_store_type = g_type_register_static( GTK_TYPE_TREE_VIEW, "JsonTreeStore", &_json_tree_store_info, (GTypeFlags)0 );
    }

    return _json_tree_store_type;
}

static void json_tree_store_class_init( JsonTreeStoreClass* klass )
{
    GObjectClass* gobject_class = G_OBJECT_CLASS( klass );

    // add private data
    g_type_class_add_private( klass, sizeof( JsonTreeStorePrivate ) );

    // register the signals
    json_tree_store_signals[EDITED_SIGNAL] =
            g_signal_new( "json_edited", G_TYPE_FROM_CLASS( klass ),
            (GSignalFlags)(G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION),
            G_STRUCT_OFFSET( JsonTreeStoreClass, json_edited ),
            NULL,
            NULL,
            g_cclosure_marshal_VOID__VOID,
            G_TYPE_NONE,
            0 );
    json_tree_store_signals[REPLACED_SIGNAL] = 
        g_signal_new( "json_replaced", G_TYPE_FROM_CLASS( klass ),
            (GSignalFlags)(G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION),
            G_STRUCT_OFFSET( JsonTreeStoreClass, json_replaced ),
            NULL,
            NULL,
            g_cclosure_marshal_VOID__VOID,
            G_TYPE_NONE,
            0 );
}

static void json_tree_store_init( JsonTreeStore* jsonTreeStore )
{
    GType _gtypes[NUM_COLUMNS] = { G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_BOOLEAN };
    gtk_tree_store_set_column_types( GTK_TREE_STORE( jsonTreeStore ), NUM_COLUMNS,  _gtypes );

    JsonTreeStorePrivate* _priv = JSON_TREE_STORE_GET_PRIVATE( jsonTreeStore );
    _priv->pJson_ = nullptr;
}


static void json_tree_store_fill_data(JsonTreeStore* jsonTreeStore, nlohmann::json* const pBranch, GtkTreeIter* const parent, const gchar* keyPath )
{
    GtkTreeIter _iter; 
    for ( auto& _it : pBranch->items() )
    {
        gchar* _newKeyPath = ( parent == NULL ) ?
                g_strdup( _it.key().c_str() ) :
                g_strjoin( ":", keyPath, _it.key().c_str(), NULL );
         
        gtk_tree_store_append( GTK_TREE_STORE( jsonTreeStore ), &_iter, parent );
        if ( _it.value().is_structured() )
        {
            gtk_tree_store_set( GTK_TREE_STORE( jsonTreeStore ), &_iter, KEY, _it.key().c_str(), VALUE, "", KEY_PATH, _newKeyPath, EDITABLE, FALSE, -1 );
            if ( _it.value().size() > 0 )
            {
                json_tree_store_fill_data(jsonTreeStore, &_it.value(), &_iter, _newKeyPath );
            }
        }
        else
        {
            gtk_tree_store_set( GTK_TREE_STORE( jsonTreeStore ), &_iter, KEY, _it.key().c_str(), VALUE, nlohmann::to_string( _it.value()) .c_str(), KEY_PATH, _newKeyPath, EDITABLE, TRUE, -1 );
        }
    }
}

GtkWidget* json_tree_store_new()
{
    return GTK_WIDGET( g_object_new( json_tree_store_get_type(), NULL ) );
}

void json_tree_store_set_json_string( JsonTreeStore* jsonTreeStore, nlohmann::json* pJson, gboolean expandFirstRow ) 
{ 
    JsonTreeStorePrivate* _priv = JSON_TREE_STORE_GET_PRIVATE( jsonTreeStore );
    if ( _priv->pJson_ != nullptr) 
    {
        free( _priv->pJson_ );
    }
    _priv->pJson_ = pJson;
    gtk_tree_store_clear( GTK_TREE_STORE( jsonTreeStore ) );
    json_tree_store_fill_data( jsonTreeStore, _priv->pJson_ ); 
}

gchar* json_tree_store_get_json_string( JsonTreeStore* jsonTreeStore )
{
    JsonTreeStorePrivate* _priv = JSON_TREE_STORE_GET_PRIVATE( jsonTreeStore );
    if (_priv->pJson_ == nullptr)
    {
        return g_strdup( "" );
    }
    else
    {
        return g_strdup( nlohmann::to_string( *_priv->pJson_ ).c_str() );
    }
}