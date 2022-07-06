#include "TemplateChooser.h"
#include "TemplateListStore.h"

#define TEMPLATE_CHOOSER_GET_PRIVATE( obj ) ( G_TYPE_INSTANCE_GET_PRIVATE( (obj), TEMPLATE_CHOOSER_TYPE, TemplateChooserPrivate ) )

typedef struct _TemplateChooserPrivate TemplateChooserPrivate;

struct _TemplateChooserPrivate
{
    Logger* pLogger_;
    TemplateListStore* pListStore_;
    gchar* topic_ = NULL;
    gchar* workingDirectory_;
};

// signals emitted

// properties
enum
{
    PROP_0,
    PROP_TOPIC
};

// Private function prototypes
static void template_chooser_class_init( TemplateChooserClass* klass );
static void template_chooser_init( TemplateChooser* templateChooser );
static void template_chooser_get_property( GObject*, guint, GValue*, GParamSpec* );
static void template_chooser_set_property( GObject*, guint, const GValue*, GParamSpec* );
static void template_chooser_set_topic( TemplateChooser* templateChooser, const gchar* topic );

// Type registration
GType template_chooser_get_type( void )
{
    static GType _template_chooser_type = 0;

    if ( !_template_chooser_type )
    {
        static const GTypeInfo _template_chooser_info =
        {
            sizeof( TemplateChooserClass ),
            NULL,
            NULL,
            (GClassInitFunc) template_chooser_class_init,
            NULL,
            NULL,
            sizeof( TemplateChooser ),
            0,
            (GInstanceInitFunc) template_chooser_init,
        };

        _template_chooser_type = g_type_register_static( GTK_TYPE_COMBO_BOX, "TemplateChooser", &_template_chooser_info, (GTypeFlags)0 );
    }

    return _template_chooser_type;
}

// Initialization functions
static void template_chooser_class_init( TemplateChooserClass* klass )
{
    GObjectClass* gobject_class = G_OBJECT_CLASS( klass );

    // override property functions
    gobject_class->set_property = template_chooser_set_property;
    gobject_class->get_property = template_chooser_get_property;   

    // add private data
    g_type_class_add_private( klass, sizeof( TemplateChooserPrivate ) );

    // register the properties
    g_object_class_install_property( gobject_class, PROP_TOPIC,
        g_param_spec_string("topic",
            "AMQ Topic",
            "AMQ Topic name to publish to",
            "",
            G_PARAM_READWRITE)
    );
}

static void template_chooser_init( TemplateChooser* templateChooser )
{
    TemplateChooserPrivate* _priv = TEMPLATE_CHOOSER_GET_PRIVATE( templateChooser );

    GtkCellRenderer* _renderer = gtk_cell_renderer_text_new();
    
    gtk_cell_layout_pack_start( GTK_CELL_LAYOUT( templateChooser ), 
        _renderer, TRUE);
    gtk_cell_layout_set_attributes( GTK_CELL_LAYOUT( templateChooser ), 
        _renderer, "text", KEY, NULL );
}

static void template_chooser_set_property( GObject* object, guint prop_id, const GValue* value, GParamSpec* pspec )
{
    TemplateChooser* _templateChooser = TEMPLATE_CHOOSER( object );

    switch (prop_id)
    {
        case PROP_TOPIC:
            template_chooser_set_topic( _templateChooser, g_value_get_string( value ) );
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID( object, prop_id, pspec );
            break;
    }
}

static void template_chooser_get_property( GObject* object, guint prop_id, GValue* value, GParamSpec* pspec )
{
    TemplateChooser* _templateChooser = TEMPLATE_CHOOSER( object );
    TemplateChooserPrivate* _priv = TEMPLATE_CHOOSER_GET_PRIVATE( _templateChooser );

    switch (prop_id)
    {
        case PROP_TOPIC:
            g_value_set_string( value, _priv->topic_ );
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID( object, prop_id, pspec );
            break;
    }
}

// returns number of templates imported or -1 on an error
static void template_chooser_set_topic( TemplateChooser* templateChooser, const gchar* topic )
{
    TemplateChooserPrivate* _priv = TEMPLATE_CHOOSER_GET_PRIVATE( templateChooser );

    g_free( _priv->topic_ );
    _priv->topic_ = g_strdup( topic );
 
    gchar* _templateBaseFileName = TEMPLATE_FILE_NAME( topic );
    gchar* _templateFileName = g_build_filename( _priv->workingDirectory_, _templateBaseFileName, NULL );
    gint _templatesImported = template_list_store_import_file(_priv->pListStore_, _templateFileName );

    if ( _templatesImported >= 0 )
    {
        _priv->pLogger_->Info( g_strdup_printf( "Publish topic changed to %s", topic ) );
    }
    else
    {
        _priv->pLogger_->Info( g_strdup_printf( "No template file found for topic %s", topic ) );
    } 

    g_free( _templateBaseFileName );
    g_free( _templateFileName );
}

// Public functions
GtkWidget* template_chooser_new( Logger* pLogger, const gchar* workingDirectory )
{
    GtkWidget* _pComboBox = GTK_WIDGET( g_object_new( template_chooser_get_type(), NULL ) );
    TemplateChooserPrivate* _priv = TEMPLATE_CHOOSER_GET_PRIVATE( _pComboBox );
    _priv->pLogger_ = pLogger;
    _priv->workingDirectory_ = g_strdup( workingDirectory );

    // add model- must be done here because we did not have pLogger during template_chooser_init

    _priv->pListStore_ = template_list_store_new( _priv->pLogger_ );
    gtk_combo_box_set_model( GTK_COMBO_BOX( _pComboBox ), GTK_TREE_MODEL( _priv->pListStore_ ) );
    g_object_unref(  _priv->pListStore_ );

    return _pComboBox;
} 

gchar* template_chooser_get_template_name( TemplateChooser* templatechooser )
{
    TemplateChooserPrivate* _priv = TEMPLATE_CHOOSER_GET_PRIVATE( templatechooser );

    GtkTreeIter _iter;
    gchar* _templateName = NULL;
    if ( gtk_combo_box_get_active_iter( GTK_COMBO_BOX( templatechooser ), &_iter ) )
    {
        gtk_tree_model_get( GTK_TREE_MODEL( _priv->pListStore_ ), &_iter, KEY, &_templateName, -1 );
    }
 
    return _templateName;
}

gchar* template_chooser_get_template( TemplateChooser* templatechooser, int* id )
{
    TemplateChooserPrivate* _priv = TEMPLATE_CHOOSER_GET_PRIVATE( templatechooser );

    GtkTreeIter _iter;
    gchar* _template = NULL;
    int _id = -1;
    if ( gtk_combo_box_get_active_iter( GTK_COMBO_BOX( templatechooser ), &_iter ) )
    {
        gtk_tree_model_get( GTK_TREE_MODEL( _priv->pListStore_ ), &_iter, TEMPLATE, &_template, ID, &_id, -1 );
    }
 
    if (id != NULL)
    {
        *id = _id;
    }
    return _template;
}

gchar* template_chooser_get_help_text( TemplateChooser* templatechooser , int* id )
{
    TemplateChooserPrivate* _priv = TEMPLATE_CHOOSER_GET_PRIVATE( templatechooser );

    GtkTreeIter _iter;
    gchar* _helpText = NULL;
    int _id = -1;
    if ( gtk_combo_box_get_active_iter( GTK_COMBO_BOX( templatechooser ), &_iter ) )
    {
        gtk_tree_model_get( GTK_TREE_MODEL( _priv->pListStore_ ), &_iter, HELP, &_helpText, ID, &_id, -1 );
    }
 
    if (id != NULL)
    {
        *id = _id;
    }
    return _helpText;
}