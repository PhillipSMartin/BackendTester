#include "JsonTreeStore.h"
#include "nlohmann/json.hpp"

#define JSON_TREE_STORE_GET_PRIVATE( obj ) ( G_TYPE_INSTANCE_GET_PRIVATE( (obj), JSON_TREE_STORE_TYPE, JsonTreeStorePrivate ) )

typedef struct _JsonTreeStorePrivate JsonTreeStorePrivate;

struct _JsonTreeStorePrivate
{
    nlohmann::json* pJson_;
    Logger* pLogger_;
};

// signals emitted
enum
{
    EDITED_SIGNAL,
    REPLACED_SIGNAL,
    LAST_SIGNAL
};

static guint json_tree_store_signals[LAST_SIGNAL] = { 0 };

// Private function prototypes
static void json_tree_store_class_init( JsonTreeStoreClass* klass );
static void json_tree_store_init( JsonTreeStore* jsonTreeStore );
static void json_tree_store_fill_data(JsonTreeStore* jsonTreeStore, nlohmann::json* pBranch, GtkTreeIter* parent = NULL, const gchar* keyPath = NULL );
static nlohmann::json* json_tree_store_parse_json_string( const gchar* text , Logger* pLogger );
static nlohmann::json* const json_tree_store_get_branch( nlohmann::json* pJson, const gchar* keyPath );

// Type registration
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

        _json_tree_store_type = g_type_register_static( GTK_TYPE_TREE_STORE, "JsonTreeStore", &_json_tree_store_info, (GTypeFlags)0 );
    }

    return _json_tree_store_type;
}

// Initialization functions
static void json_tree_store_class_init( JsonTreeStoreClass* klass )
{
    GObjectClass* gobject_class = G_OBJECT_CLASS( klass );

    // add private data
    g_type_class_add_private( klass, sizeof( JsonTreeStorePrivate ) );

    // register the signals
    json_tree_store_signals[EDITED_SIGNAL] =
            g_signal_new( "json-edited", G_TYPE_FROM_CLASS( klass ),
            (GSignalFlags)(G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION),
            G_STRUCT_OFFSET( JsonTreeStoreClass, json_edited ),
            NULL,
            NULL,
            g_cclosure_marshal_VOID__VOID,
            G_TYPE_NONE,
            0 );
    json_tree_store_signals[REPLACED_SIGNAL] = 
        g_signal_new( "json-replaced", G_TYPE_FROM_CLASS( klass ),
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

// Private functions
static void json_tree_store_fill_data(JsonTreeStore* jsonTreeStore, nlohmann::json* pBranch, GtkTreeIter* parent, const gchar* keyPath )
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

static nlohmann::json* json_tree_store_parse_json_string( const gchar* text, Logger* pLogger )
{
    try
    {
        return new nlohmann::json(nlohmann::json::parse( text ));
    }
    catch(const std::exception& e)
    {
        gchar* msg = g_strdup_printf( "Error parsing string %s", text );
        pLogger->Info(" msg ");
        pLogger->Error( e.what() );
        g_free( msg );
        return nullptr;
     }   
}

static nlohmann::json* const json_tree_store_get_branch( nlohmann::json* pJson, const gchar* keyPath )
{
    nlohmann::json* _pJson = pJson;
    gchar** _keys = g_strsplit( keyPath, ":", -1 );
    for ( int i=0; _keys[i] != NULL; i++ )
    {
        if ( _pJson->type() == nlohmann::json::value_t::array )
        {
            _pJson = &_pJson->at( atoi( _keys[i] ) );
        }
        else
        {
            _pJson = &_pJson->at( _keys[i] );
        }
    }

    g_strfreev( _keys );
    return _pJson;
}

// Public functions
JsonTreeStore* json_tree_store_new(Logger* pLogger)
{
    JsonTreeStore* _pTreeStore = JSON_TREE_STORE( g_object_new( json_tree_store_get_type(), NULL ) );
    JsonTreeStorePrivate* _priv = JSON_TREE_STORE_GET_PRIVATE( _pTreeStore );
    _priv->pLogger_ = pLogger;
    return _pTreeStore;
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

void json_tree_store_set_json_string( JsonTreeStore* jsonTreeStore, const gchar* text) 
{ 
    JsonTreeStorePrivate* _priv = JSON_TREE_STORE_GET_PRIVATE( jsonTreeStore );
    if ( _priv->pJson_ != nullptr) 
    {
        free( _priv->pJson_ );
        gtk_tree_store_clear( GTK_TREE_STORE( jsonTreeStore ) );
    }
  
    _priv->pJson_ = json_tree_store_parse_json_string( text, _priv->pLogger_ );
    if ( _priv->pJson_ != nullptr )
    {
        json_tree_store_fill_data( jsonTreeStore, _priv->pJson_ ); 
    }

    g_signal_emit_by_name( (gpointer)jsonTreeStore, "json-replaced" );
}

void json_tree_store_edit_json_string( JsonTreeStore* jsonTreeStore, const gchar* path, const gchar* newText )
{
    GtkTreeIter _iter;
    JsonTreeStorePrivate* _priv = JSON_TREE_STORE_GET_PRIVATE( jsonTreeStore );

    if ( gtk_tree_model_get_iter_from_string( GTK_TREE_MODEL( jsonTreeStore ), &_iter, path ) )
    {
        gchar* _typeName;
        gchar* _keyPath;
        gchar* _errorMsg;
        gtk_tree_model_get( GTK_TREE_MODEL( jsonTreeStore ), &_iter, KEY_PATH, &_keyPath, -1 );
        
        nlohmann::json* _pJson = json_tree_store_get_branch( _priv->pJson_, _keyPath );
        switch (_pJson->type())
        {
            case nlohmann::json::value_t::number_integer:
            case nlohmann::json::value_t::number_unsigned:
                *_pJson = atoi( newText );
                break;
            case nlohmann::json::value_t::number_float:
                *_pJson = atof( newText );
                break;
            case nlohmann::json::value_t::string:
                {
                    std::string s(newText);
                    s.erase( remove( s.begin(), s.end(), '"' ), s.end() );
                    *_pJson =  s.c_str();
                }
                break;
            case nlohmann::json::value_t::boolean:
                *_pJson = g_str_has_prefix( newText, "t" ) || g_str_has_prefix( newText, "T" );
                break;
            default: 
                _errorMsg = g_strdup_printf( "Unhandled type \"%s\" for json item \"%s\"", _pJson->type_name(), _keyPath );           
                _priv->pLogger_->Error( _errorMsg );
                g_free( _errorMsg );
                return;
        }

        gtk_tree_store_set( GTK_TREE_STORE( jsonTreeStore ), &_iter, VALUE, nlohmann::to_string( *_pJson).c_str(), -1 );
        _priv->pLogger_->Debug( "Publish message changed to: " + nlohmann::to_string( *_priv->pJson_ ) );  

        g_signal_emit_by_name( (gpointer)jsonTreeStore, "json-edited" );   
    }
}
