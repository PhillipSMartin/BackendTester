#include "TopicChooser.h"

#define TOPIC_CHOOSER_GET_PRIVATE( obj ) ( G_TYPE_INSTANCE_GET_PRIVATE( (obj), TOPIC_CHOOSER_TYPE, TopicChooserPrivate ) )

typedef struct _TopicChooserPrivate TopicChooserPrivate;

struct _TopicChooserPrivate
{
    Parameters* pParms_;
    Logger* pLogger_;
    GtkWidget* pTopicPrefixComboBox_;
    GtkWidget* pTableNumberSpinButton_;   
};

// signals emitted
enum
{
    CHANGED_SIGNAL,
    LAST_SIGNAL
};

static guint topic_chooser_signals[LAST_SIGNAL] = { 0 };
static const gchar* topic_chooser_topics[4] = { "Event.tb", "Event.tm", "State.tb", "State.tm" };

// Private function prototypes
static void topic_chooser_class_init( TopicChooserClass* klass );
static void topic_chooser_init( TopicChooser* topicChooser );
static void topic_chooser_on_combo_box_changed( GtkComboBox* comboBox, TopicChooser* topicChooser );
static void topic_chooser_on_spin_button_value_changed( GtkSpinButton* spinButton, TopicChooser* topicChooser );

// Type registration
GType topic_chooser_get_type( void )
{
    static GType _topic_chooser_type = 0;

    if ( !_topic_chooser_type )
    {
        static const GTypeInfo _topic_chooser_info =
        {
            sizeof( TopicChooserClass ),
            NULL,
            NULL,
            (GClassInitFunc) topic_chooser_class_init,
            NULL,
            NULL,
            sizeof( TopicChooser ),
            0,
            (GInstanceInitFunc) topic_chooser_init,
        };

        _topic_chooser_type = g_type_register_static( GTK_TYPE_BOX, "TopicChooser", &_topic_chooser_info, (GTypeFlags)0 );
    }

    return _topic_chooser_type;
}

// Initialization functions
static void topic_chooser_class_init( TopicChooserClass* klass )
{
    GObjectClass* gobject_class = G_OBJECT_CLASS( klass );  

    // add private data
    g_type_class_add_private( klass, sizeof( TopicChooserPrivate ) );

    // register the signals
    topic_chooser_signals[CHANGED_SIGNAL] =
            g_signal_new( "changed", G_TYPE_FROM_CLASS( klass ),
            (GSignalFlags)(G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION),
            G_STRUCT_OFFSET( TopicChooserClass, changed ),
            NULL,
            NULL,
            g_cclosure_marshal_VOID__VOID,
            G_TYPE_NONE,
            0 );
}

static void topic_chooser_init( TopicChooser* topicChooser )
{
    TopicChooserPrivate* _priv = TOPIC_CHOOSER_GET_PRIVATE( topicChooser );

    gtk_orientable_set_orientation( GTK_ORIENTABLE( topicChooser ), GTK_ORIENTATION_HORIZONTAL );
    gtk_box_set_spacing( GTK_BOX( topicChooser ), 10 );
    
    _priv->pTopicPrefixComboBox_ = gtk_combo_box_text_new();
    _priv->pTableNumberSpinButton_ = gtk_spin_button_new_with_range( 1, 100000, 1 );

    gtk_box_pack_start( GTK_BOX( topicChooser ), gtk_label_new( "Topic: " ), FALSE, TRUE, 0 );
    gtk_box_pack_start( GTK_BOX( topicChooser ), _priv->pTopicPrefixComboBox_, FALSE, TRUE, 0 );
    gtk_box_pack_start( GTK_BOX( topicChooser ), gtk_label_new( "Table no: " ), FALSE, TRUE, 0 );
    gtk_box_pack_start( GTK_BOX( topicChooser ), _priv->pTableNumberSpinButton_, FALSE, TRUE, 0 );

    for ( const gchar* _topic : topic_chooser_topics )
    {
        gtk_combo_box_text_append_text( GTK_COMBO_BOX_TEXT( _priv->pTopicPrefixComboBox_ ), _topic );
    }

    g_signal_connect( G_OBJECT( _priv->pTopicPrefixComboBox_ ), "changed", G_CALLBACK( topic_chooser_on_combo_box_changed ), gpointer( topicChooser ) );
    g_signal_connect( G_OBJECT( _priv->pTableNumberSpinButton_ ), "value-changed", G_CALLBACK( topic_chooser_on_spin_button_value_changed ), gpointer( topicChooser ) );
}

static void topic_chooser_on_combo_box_changed( GtkComboBox* comboBox, TopicChooser* topicChooser )
{
    g_signal_emit_by_name( G_OBJECT( topicChooser ), "changed" );
}

static void topic_chooser_on_spin_button_value_changed( GtkSpinButton* spinButton, TopicChooser* topicChooser )
{
    g_signal_emit_by_name( G_OBJECT( topicChooser ), "changed" );
}

// Public functions
GtkWidget* topic_chooser_new( Parameters* pParms, Logger* pLogger, TOPICS defaultTopic )
{
    GtkWidget* _pBox = GTK_WIDGET( g_object_new( topic_chooser_get_type(), NULL ) );
    TopicChooserPrivate* _priv = TOPIC_CHOOSER_GET_PRIVATE( _pBox );
    _priv->pParms_ = pParms;
    _priv->pLogger_ = pLogger;
    gtk_combo_box_set_active( GTK_COMBO_BOX( _priv->pTopicPrefixComboBox_ ), defaultTopic );

    return _pBox;
} 

gchar* topic_chooser_get_topic( TopicChooser* topicchooser )
{
    TopicChooserPrivate* _priv = TOPIC_CHOOSER_GET_PRIVATE( topicchooser );

    int _topicIndex = gtk_combo_box_get_active( GTK_COMBO_BOX( _priv->pTopicPrefixComboBox_ ) );
    if ( _topicIndex < 0 )
    {
        return NULL;
    }

    gchar* _topic = g_strdup_printf( "%s=%s-%d", 
        topic_chooser_topics[ _topicIndex ], 
        _priv->pParms_->get_tourney_id(),
        g_str_has_suffix( topic_chooser_topics[ _topicIndex ], "tb" ) ?
            gtk_spin_button_get_value_as_int( GTK_SPIN_BUTTON( _priv->pTableNumberSpinButton_ ) ) : 0 ); 
    return _topic;
}

gchar* topic_chooser_get_topic_prefix( TopicChooser* topicchooser )
{
    TopicChooserPrivate* _priv = TOPIC_CHOOSER_GET_PRIVATE( topicchooser );

    int _topicIndex = gtk_combo_box_get_active( GTK_COMBO_BOX( _priv->pTopicPrefixComboBox_ ) );
    if ( _topicIndex < 0 )
    {
        return NULL;
    }

    return  g_strdup( topic_chooser_topics[ _topicIndex ] ); 
}
