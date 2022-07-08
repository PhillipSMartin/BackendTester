#include "TourneyIdChooser.h"

#define TOURNEY_ID_CHOOSER_GET_PRIVATE( obj ) ( G_TYPE_INSTANCE_GET_PRIVATE( (obj), TOURNEY_ID_CHOOSER_TYPE, TourneyIdChooserPrivate ) )

typedef struct _TourneyIdChooserPrivate TourneyIdChooserPrivate;

struct _TourneyIdChooserPrivate
{
    GtkWidget* pTourneyIdEntry_;
    GtkWidget* pUpdateButton_;   
};

// signals emitted
enum
{
    CHANGED_SIGNAL,
    LAST_SIGNAL
};

static guint tourney_id_chooser_signals[LAST_SIGNAL] = { 0 };

// Private function prototypes
static void tourney_id_chooser_class_init( TourneyIdChooserClass* klass );
static void tourney_id_chooser_init( TourneyIdChooser* tourneyIdChooser );
static void tourney_id_chooser_on_entry_activate( GtkEntry* entry, TourneyIdChooser* tourneyIdChooser  );
static void tourney_id_chooser_on_update_button_clicked( GtkButton* button, TourneyIdChooser* tourneyIdChooser );

// Type registration
GType tourney_id_chooser_get_type( void )
{
    static GType _tourney_id_chooser_type = 0;

    if ( !_tourney_id_chooser_type )
    {
        static const GTypeInfo _tourney_id_chooser_info =
        {
            sizeof( TourneyIdChooserClass ),
            NULL,
            NULL,
            (GClassInitFunc) tourney_id_chooser_class_init,
            NULL,
            NULL,
            sizeof( TourneyIdChooser ),
            0,
            (GInstanceInitFunc) tourney_id_chooser_init,
        };

        _tourney_id_chooser_type = g_type_register_static( GTK_TYPE_BOX, "TourneyIdChooser", &_tourney_id_chooser_info, (GTypeFlags)0 );
    }

    return _tourney_id_chooser_type;
}

// Initialization functions
static void tourney_id_chooser_class_init( TourneyIdChooserClass* klass )
{
    GObjectClass* gobject_class = G_OBJECT_CLASS( klass );  

    // add private data
    g_type_class_add_private( klass, sizeof( TourneyIdChooserPrivate ) );


    // register the signals
    tourney_id_chooser_signals[CHANGED_SIGNAL] =
            g_signal_new( "changed", G_TYPE_FROM_CLASS( klass ),
            (GSignalFlags)(G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION),
            G_STRUCT_OFFSET( TourneyIdChooserClass, changed ),
            NULL,
            NULL,
            g_cclosure_marshal_VOID__VOID,
            G_TYPE_NONE,
            0 );
}

static void tourney_id_chooser_init( TourneyIdChooser* tourneyIdChooser )
{
    TourneyIdChooserPrivate* _priv = TOURNEY_ID_CHOOSER_GET_PRIVATE( tourneyIdChooser );

    gtk_orientable_set_orientation( GTK_ORIENTABLE( tourneyIdChooser ), GTK_ORIENTATION_HORIZONTAL );
    gtk_box_set_spacing( GTK_BOX( tourneyIdChooser ), 10 );
    
    _priv->pTourneyIdEntry_ = gtk_entry_new();
    _priv->pUpdateButton_ = gtk_button_new_with_label( "Update" );
    gtk_widget_set_sensitive( _priv->pUpdateButton_, FALSE );

    gtk_box_pack_start( GTK_BOX( tourneyIdChooser ), gtk_label_new( "Tourney Id: " ), FALSE, TRUE, 0 );
    gtk_box_pack_start( GTK_BOX( tourneyIdChooser ), _priv->pTourneyIdEntry_, TRUE, TRUE, 0);
    gtk_box_pack_start( GTK_BOX( tourneyIdChooser ), _priv->pUpdateButton_, FALSE, TRUE, 0 );

    g_signal_connect( G_OBJECT( _priv->pTourneyIdEntry_ ), "activate", G_CALLBACK( tourney_id_chooser_on_entry_activate ), gpointer( tourneyIdChooser ) );
    g_signal_connect( G_OBJECT( _priv->pUpdateButton_ ), "clicked", G_CALLBACK( tourney_id_chooser_on_update_button_clicked ), gpointer( tourneyIdChooser ) );
}

static void tourney_id_chooser_on_entry_activate( GtkEntry* entry, TourneyIdChooser* tourneyIdChooser )
{
    TourneyIdChooserPrivate* _priv = TOURNEY_ID_CHOOSER_GET_PRIVATE( tourneyIdChooser );  
    gtk_widget_set_sensitive( _priv->pUpdateButton_, TRUE );
}

static void tourney_id_chooser_on_update_button_clicked( GtkButton* button, TourneyIdChooser* tourneyIdChooser )
{
    TourneyIdChooserPrivate* _priv = TOURNEY_ID_CHOOSER_GET_PRIVATE( tourneyIdChooser );  
    gtk_widget_set_sensitive( _priv->pUpdateButton_, FALSE );
    g_signal_emit_by_name( G_OBJECT( tourneyIdChooser ), "changed" );
}

// Public functions
GtkWidget* tourney_id_chooser_new(  const gchar* tourneyId )
{
    GtkWidget* _pBox = GTK_WIDGET( g_object_new( tourney_id_chooser_get_type(), NULL ) );
    TourneyIdChooserPrivate* _priv = TOURNEY_ID_CHOOSER_GET_PRIVATE( _pBox );  

    gtk_entry_set_text( GTK_ENTRY( _priv->pTourneyIdEntry_ ), tourneyId );

    return _pBox;
} 

const gchar* tourney_id_chooser_get_tourney_id( TourneyIdChooser* tourneyIdchooser )
{
    TourneyIdChooserPrivate* _priv = TOURNEY_ID_CHOOSER_GET_PRIVATE( tourneyIdchooser );
    return gtk_entry_get_text( GTK_ENTRY( _priv->pTourneyIdEntry_ ) );
}