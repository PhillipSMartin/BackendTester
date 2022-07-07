#pragma once

#include "gtk/gtk.h"
#include "Logger.h"

// ConsoleViewer
//
//      a scrollwindow containing a TextView object for viewing console messages
//
//  Public methods:
//      GtkWidget* console_viewer_new( Logger* pLogger )
//          creates a new ConsoleViewer
//
//  Signals:


G_BEGIN_DECLS

#define CONSOLE_VIEWER_TYPE                ( console_viewer_get_type() )
#define CONSOLE_VIEWER( obj )              ( G_TYPE_CHECK_INSTANCE_CAST( ( obj ), CONSOLE_VIEWER_TYPE, ConsoleViewer ) )
#define CONSOLE_VIEWER_CLASS( klass )      ( G_TYPE_CHECK_CLASS_CAST( (klass), CONSOLE_VIEWER_TYPE, ConsoleViewerClass ) )
#define IS_CONSOLE_VIEWER( obj )           ( G_TYPE_CHECK_INSTANCE_TYPE( (obj), CONSOLE_VIEWER_TYPE ) )
#define IS_CONSOLE_VIEWER_CLASS( klass )   ( G_TYPE_CHECK_CLASS_TYPE( (klass), CONSOLE_VIEWER_TYPE ) )

typedef struct _ConsoleViewer               ConsoleViewer;
typedef struct _ConsoleViewerClass          ConsoleViewerClass;

struct _ConsoleViewer
{
    GtkScrolledWindow scrolledWindow_;
};

struct _ConsoleViewerClass
{
    GtkScrolledWindowClass parentClass_;

    void (* console_edited) ( ConsoleViewer* consoleViewer );
    void (* console_replaced) ( ConsoleViewer* consoleViewer );
};

GType console_viewer_get_type( void ) G_GNUC_CONST;
GtkWidget* console_viewer_new( Logger* logger );

G_END_DECLS