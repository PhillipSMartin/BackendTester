#include <algorithm>
#include <sstream>

#include "TemplateListStore.h"

#define TEMPLATE_LIST_STORE_GET_PRIVATE( obj ) ( G_TYPE_INSTANCE_GET_PRIVATE( (obj), TEMPLATE_LIST_STORE_TYPE, TemplateListStorePrivate ) )

typedef struct _TemplateListStorePrivate TemplateListStorePrivate;

struct _TemplateListStorePrivate
{
    Logger* pLogger_;
    gchar* readFileName_ = NULL;
    gchar* writeFileName_ = NULL;
};

// Private function prototypes
static void template_list_store_class_init( TemplateListStoreClass* klass );
static void template_list_store_init( TemplateListStore* templateListStore );

static int template_list_store_generate_id();
static int template_list_store_handle_error( Logger* pLogger, const gchar* prefix, GError* error ); 
static int template_list_store_read_file( TemplateListStore* templateListStore, GIOChannel* channel );
static int template_list_store_write_file( TemplateListStore* templateListStore, GIOChannel* channel );
static int template_list_store_add_item( TemplateListStore* templateListStore, const gchar* line );
static int template_list_store_get_item( TemplateListStore* templateListStore, GtkTreeIter* iter, gchar** line );
static int template_list_store_generate_id();

// Type registration
GType template_list_store_get_type( void )
{
    static GType _template_list_store_type = 0;

    if ( !_template_list_store_type )
    {
        static const GTypeInfo _template_list_store_info =
        {
            sizeof( TemplateListStoreClass ),
            NULL,
            NULL,
            (GClassInitFunc) template_list_store_class_init,
            NULL,
            NULL,
            sizeof( TemplateListStore ),
            0,
            (GInstanceInitFunc) template_list_store_init,
        };

        _template_list_store_type = g_type_register_static( GTK_TYPE_LIST_STORE, "TemplateListStore", &_template_list_store_info, (GTypeFlags)0 );
    }

    return _template_list_store_type;
}

// Initialization functions
static void template_list_store_class_init( TemplateListStoreClass* klass )
{
    GObjectClass* gobject_class = G_OBJECT_CLASS( klass );

    // add private data
    g_type_class_add_private( klass, sizeof( TemplateListStorePrivate ) );

    // register the signals
 }

static void template_list_store_init( TemplateListStore* templateListStore )
{
    GType _gtypes[NUM_COLUMNS] = { G_TYPE_STRING, G_TYPE_INT, G_TYPE_STRING, G_TYPE_STRING };
    gtk_list_store_set_column_types( GTK_LIST_STORE( templateListStore ), NUM_COLUMNS,  _gtypes );
}

// Private functions
static int template_list_store_generate_id()
{
    static int _id = 0;
    return _id++;
}

// returns -1 if there was an error, 0 if not
static int template_list_store_handle_error( Logger* pLogger, const gchar* prefix, GError* error ) 
{
    if ( error != NULL )
    {
        pLogger->Error( g_strdup_printf( "%s: %s", prefix, error->message ) );
        g_clear_error( &error );
        return -1;
    }

    return 0;
}

// returns number of lines read or -1 on an error
static int template_list_store_read_file( TemplateListStore* templateListStore, GIOChannel* channel )
{
    gchar* _line = NULL;
    GError* _error = NULL;
    int _lineNo = 0;
    TemplateListStorePrivate* _priv = TEMPLATE_LIST_STORE_GET_PRIVATE( templateListStore );

    while ( G_IO_STATUS_EOF != g_io_channel_read_line( channel, &_line, NULL, NULL, &_error ) )
    {
        if ( _error )
        {
            gchar* _errorMsg = g_strdup_printf( "Unable to read file %s",  _priv->readFileName_ );
            template_list_store_handle_error( _priv->pLogger_, _errorMsg, _error );
            g_free( _errorMsg );
            _lineNo = -1;
            break;
        }
 
        if ( 0 > template_list_store_add_item( templateListStore, _line ) )
        {
            _priv->pLogger_->Error( g_strdup_printf("Format error in %s, line %d",  _priv->readFileName_, _lineNo) );
            _lineNo = -1;
            break;
        }

        g_free( _line );
        _line = NULL;
        _lineNo++;
    } 

    g_free( _line );
    return _lineNo;
}

// returns number of lines written or -1 on an error
static int template_list_store_write_file( TemplateListStore* templateListStore, GIOChannel* channel )
{
    gchar* _line = NULL;
    GError* _error = NULL;
    int _lineNo = 0;
    GtkTreeIter _iter;
    TemplateListStorePrivate* _priv = TEMPLATE_LIST_STORE_GET_PRIVATE( templateListStore );

    gboolean _rc = gtk_tree_model_get_iter_first( GTK_TREE_MODEL( templateListStore ) , &_iter );
    while ( _rc )
    {
        _lineNo++;
        if ( 0 > template_list_store_get_item( templateListStore, &_iter, &_line ) )
        {
            _priv->pLogger_->Error( g_strdup_printf( "Unable to construct line %d for %s", _lineNo, _priv->writeFileName_ ));
            _lineNo = -1;
            break;
        }

        g_io_channel_write_chars( channel, _line, -1, NULL, &_error ); 
        if ( _error )
        {
            gchar* _errorMsg = g_strdup_printf( "Unable to write file %s",  _priv->writeFileName_ );
            template_list_store_handle_error( _priv->pLogger_, _errorMsg, _error );
            g_free( _errorMsg );
            _lineNo = -1;
            break;
        }

        _rc = gtk_tree_model_iter_next( GTK_TREE_MODEL( templateListStore ) , &_iter );
    }

    g_free( _line );
    return _lineNo;
}

static int template_list_store_add_item( TemplateListStore* templateListStore, const gchar* line )
{
    TemplateListStorePrivate* _priv = TEMPLATE_LIST_STORE_GET_PRIVATE( templateListStore );
    GtkTreeIter _iter;
    std::string _key, _template, _help;
    std::stringstream _sstream(line);

    if ( !std::getline( _sstream, _key, '|' ) )
    {
        return -1;
    }

    if ( !std::getline( _sstream, _template, '|' ) )
    {
        return -1;
    }

    std::getline( _sstream, _help, '|' );
    std::replace( _help.begin(), _help.end(), ';', '\n' );

    gtk_list_store_append( GTK_LIST_STORE( templateListStore ), &_iter );
    gtk_list_store_set( GTK_LIST_STORE( templateListStore ), &_iter,
        KEY, _key.c_str(),
        ID, template_list_store_generate_id(),
        TEMPLATE, _template.c_str(),
        HELP, _help.c_str(), -1 );

    return 0;
}

static int template_list_store_get_item( TemplateListStore* templateListStore, GtkTreeIter* iter, gchar** line )
{
    gchar *_key, *_template, *_help;
    int _rc = 0;

    try
    {  
        gtk_tree_model_get( GTK_TREE_MODEL( templateListStore ), iter, KEY, &_key, TEMPLATE, &_template, HELP, &_help, -1 );
        std::string _helpStr{ _help };
        std::replace( _helpStr.begin(), _helpStr.end(), '\n', ';' );
        _helpStr += "\n";

        *line = g_strjoin( "|", _key, _template, _helpStr.c_str(), NULL );
    }
    catch(const std::exception& e)
    {
        _rc = -1;
    }
 
    g_free( _key );
    g_free( _template );
    g_free( _help );
    return _rc;
}

// Public functions
TemplateListStore* template_list_store_new(Logger* pLogger)
{
    TemplateListStore* _pListStore = TEMPLATE_LIST_STORE( g_object_new( template_list_store_get_type(), NULL ) );
    TemplateListStorePrivate* _priv = TEMPLATE_LIST_STORE_GET_PRIVATE( _pListStore );
    _priv->pLogger_ = pLogger;
    return _pListStore;
} 

// returns number of templates imported or -1 on an error
gint template_list_store_import_file( TemplateListStore* templateListStore, const gchar* fileName )
{
    GError* _error = NULL;
    TemplateListStorePrivate* _priv = TEMPLATE_LIST_STORE_GET_PRIVATE( templateListStore );

    g_free( _priv->readFileName_ );
    _priv->readFileName_ = g_strdup( fileName );

    gtk_list_store_clear( GTK_LIST_STORE( templateListStore ) );
    GIOChannel* _channel = g_io_channel_new_file( _priv->readFileName_, "r", &_error );
    if ( _error )
    {
        gchar* _errorMsg = g_strdup_printf( "Unable to open file %s", _priv->readFileName_ );
        template_list_store_handle_error( _priv->pLogger_, _errorMsg, _error );
        g_free( _errorMsg );
        return -1;
    }

    int _linesRead = template_list_store_read_file( templateListStore, _channel );

    g_io_channel_shutdown( _channel, FALSE, &_error );
    if ( _error )
    {
        gchar* _errorMsg = g_strdup_printf( "Unable to close file %s", _priv->readFileName_ );
        template_list_store_handle_error(  _priv->pLogger_, _errorMsg, _error );
        g_free( _errorMsg );
        return -1;
    }

    return _linesRead;   
}

gboolean template_list_store_export_file( TemplateListStore* templateListStore )
{
    GError* _error = NULL;
    TemplateListStorePrivate* _priv = TEMPLATE_LIST_STORE_GET_PRIVATE( templateListStore );  

    g_free( _priv->writeFileName_ );
    _priv->writeFileName_ = g_strdup_printf( "%s.tmp", _priv->readFileName_);

    GIOChannel* _channel = g_io_channel_new_file( _priv->writeFileName_, "w", &_error );
    if ( _error )
    {
        gchar* _errorMsg = g_strdup_printf( "Unable to open file %s", _priv->writeFileName_ );
        template_list_store_handle_error( _priv->pLogger_, _errorMsg, _error );
        g_free( _errorMsg );
        return FALSE;
    }

    template_list_store_write_file( templateListStore, _channel );

    g_io_channel_shutdown( _channel, TRUE, &_error );
    if ( _error )
    {
        gchar* _errorMsg = g_strdup_printf( "Unable to close file %s", _priv->writeFileName_ );
        template_list_store_handle_error(  _priv->pLogger_, _errorMsg, _error );
        g_free( _errorMsg );
        return FALSE;
    }

    // TODO - copy temp file

    return TRUE;
}


