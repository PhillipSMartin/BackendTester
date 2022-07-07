#include "ConsoleViewer.h"


#define CONSOLE_VIEWER_GET_PRIVATE( obj ) ( G_TYPE_INSTANCE_GET_PRIVATE( (obj), CONSOLE_VIEWER_TYPE, ConsoleViewerPrivate ) )

typedef struct _ConsoleViewerPrivate ConsoleViewerPrivate;

struct _ConsoleViewerPrivate
{
    GtkWidget* pTextView_;
};

// Private function prototypes
static void console_viewer_class_init( ConsoleViewerClass* klass );
static void console_viewer_init( ConsoleViewer* consoleViewer );

// Type registration
GType console_viewer_get_type( void )
{
    static GType _console_viewer_type = 0;

    if ( !_console_viewer_type )
    {
        static const GTypeInfo _console_viewer_info =
        {
            sizeof( ConsoleViewerClass ),
            NULL,
            NULL,
            (GClassInitFunc) console_viewer_class_init,
            NULL,
            NULL,
            sizeof( ConsoleViewer ),
            0,
            (GInstanceInitFunc) console_viewer_init,
        };

        _console_viewer_type = g_type_register_static( GTK_TYPE_SCROLLED_WINDOW, "ConsoleViewer", &_console_viewer_info, (GTypeFlags)0 );
    }

    return _console_viewer_type;
}

// Initialization functions
static void console_viewer_class_init( ConsoleViewerClass* klass )
{
    GObjectClass* gobject_class = G_OBJECT_CLASS( klass );

    // add private data
    g_type_class_add_private( klass, sizeof( ConsoleViewerPrivate ) );
}

static void console_viewer_init( ConsoleViewer* consoleViewer )
{
    ConsoleViewerPrivate* _priv = CONSOLE_VIEWER_GET_PRIVATE( consoleViewer );

    _priv->pTextView_ = gtk_text_view_new();
    gtk_scrolled_window_set_policy ( GTK_SCROLLED_WINDOW( consoleViewer ), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC );
    gtk_container_add( GTK_CONTAINER( consoleViewer ), _priv->pTextView_ );
} 

// Public functions
GtkWidget* console_viewer_new( Logger* pLogger )
{
    GtkWidget* _pViewer = GTK_WIDGET( g_object_new( console_viewer_get_type(), NULL ) );
    ConsoleViewerPrivate* _priv = CONSOLE_VIEWER_GET_PRIVATE( _pViewer );

    pLogger->set_text_buffer( gtk_text_view_get_buffer( GTK_TEXT_VIEW( _priv->pTextView_ ) ) );

    return _pViewer;
} 