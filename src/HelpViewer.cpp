#include "HelpViewer.h"

#define HELP_VIEWER_GET_PRIVATE( obj ) ( G_TYPE_INSTANCE_GET_PRIVATE( (obj), HELP_VIEWER_TYPE, HelpViewerPrivate ) )

typedef struct _HelpViewerPrivate HelpViewerPrivate;

struct _HelpViewerPrivate
{
    GtkWidget* pTextView_;
    GtkTextBuffer* pTextBuffer_;
    int helpId;     // identifies help string so caller can be sure this is his string
};

// signals emitted
enum
{
    EDITED_SIGNAL,
    REPLACED_SIGNAL,
    LAST_SIGNAL
};

static guint help_viewer_signals[LAST_SIGNAL] = { 0 };

// Private function prototypes
static void help_viewer_class_init( HelpViewerClass* klass );
static void help_viewer_init( HelpViewer* helpViewer );
static void help_viewer_on_text_buffer_changed( GtkTextBuffer* pTextBuffer, HelpViewer* helpViewer );

// Type registration
GType help_viewer_get_type( void )
{
    static GType _help_viewer_type = 0;

    if ( !_help_viewer_type )
    {
        static const GTypeInfo _help_viewer_info =
        {
            sizeof( HelpViewerClass ),
            NULL,
            NULL,
            (GClassInitFunc) help_viewer_class_init,
            NULL,
            NULL,
            sizeof( HelpViewer ),
            0,
            (GInstanceInitFunc) help_viewer_init,
        };

        _help_viewer_type = g_type_register_static( GTK_TYPE_SCROLLED_WINDOW, "HelpViewer", &_help_viewer_info, (GTypeFlags)0 );
    }

    return _help_viewer_type;
}


// Initialization functions
static void help_viewer_class_init( HelpViewerClass* klass )
{
    GObjectClass* gobject_class = G_OBJECT_CLASS( klass );

    // add private data
    g_type_class_add_private( klass, sizeof( HelpViewerPrivate ) );


    // register the signals
    help_viewer_signals[EDITED_SIGNAL] =
            g_signal_new( "edited", G_TYPE_FROM_CLASS( klass ),
            (GSignalFlags)(G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION),
            G_STRUCT_OFFSET( HelpViewerClass, edited ),
            NULL,
            NULL,
            g_cclosure_marshal_VOID__VOID,
            G_TYPE_NONE,
            0 );
    help_viewer_signals[REPLACED_SIGNAL] = 
        g_signal_new( "replaced", G_TYPE_FROM_CLASS( klass ),
            (GSignalFlags)(G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION),
            G_STRUCT_OFFSET( HelpViewerClass, replaced ),
            NULL,
            NULL,
            g_cclosure_marshal_VOID__VOID,
            G_TYPE_NONE,
            0 );
}

static void help_viewer_init( HelpViewer* helpViewer )
{
    HelpViewerPrivate* _priv = HELP_VIEWER_GET_PRIVATE( helpViewer );

    gtk_scrolled_window_set_policy( GTK_SCROLLED_WINDOW( helpViewer ), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC );

    // add text view
    _priv->pTextView_ = gtk_text_view_new();
    _priv->pTextBuffer_ = gtk_text_view_get_buffer( GTK_TEXT_VIEW( _priv->pTextView_ ) );
    gtk_container_add( GTK_CONTAINER( helpViewer ), _priv->pTextView_ );

    // wire signals
    g_signal_connect( G_OBJECT( _priv->pTextBuffer_ ), "changed", G_CALLBACK( help_viewer_on_text_buffer_changed ), gpointer(helpViewer) );
}

static void  help_viewer_on_text_buffer_changed( GtkTextBuffer* pTextBuffer, HelpViewer* helpViewer )
{
    g_signal_emit_by_name( (gpointer)helpViewer, "edited" );
}

// Public functions
GtkWidget* help_viewer_new()
{
    return GTK_WIDGET( g_object_new( help_viewer_get_type(), NULL ) );
} 

gchar* help_viewer_get_text( HelpViewer* helpViewer , int* id )
{
     HelpViewerPrivate* _priv = HELP_VIEWER_GET_PRIVATE( helpViewer ); 
     if ( id != NULL )
     {
        *id = _priv-> helpId;
     }  

     GtkTextIter _start, _end;
     gtk_text_buffer_get_start_iter( _priv->pTextBuffer_, &_start );     
     gtk_text_buffer_get_end_iter( _priv->pTextBuffer_, &_end );
     return gtk_text_buffer_get_text( _priv->pTextBuffer_, &_start, &_end, TRUE );
}

void help_viewer_set_text( HelpViewer* helpViewer , int id, const gchar* text )
{
     HelpViewerPrivate* _priv = HELP_VIEWER_GET_PRIVATE( helpViewer ); 
     _priv-> helpId = id;
     gtk_text_buffer_set_text( _priv->pTextBuffer_, text ? text : "", -1 );

     g_signal_emit_by_name( (gpointer)helpViewer, "replaced" );
}