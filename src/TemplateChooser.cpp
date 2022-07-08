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
// returns number of templates imported or -1 on an error
static void template_chooser_set_topic( TemplateChooser* templateChooser, const gchar* topic );
// returns false if no current selection
// pointer (except iter) can be NULL if we are not interested in the value
static gboolean template_chooser_get_current_values( TemplateChooser* templateChooser, GtkTreeIter *iter, int* id, gchar** templateName, gchar** templateText, gchar** helpText );

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

// returns false if no current selection
// pointer (except iter) can be NULL if we are not interested in the value
static gboolean template_chooser_get_current_values( TemplateChooser* templateChooser, GtkTreeIter *iter, int *id, gchar** templateName, gchar** templateText, gchar** helpText)
{
    TemplateChooserPrivate* _priv = TEMPLATE_CHOOSER_GET_PRIVATE( templateChooser );

    if ( !gtk_combo_box_get_active_iter( GTK_COMBO_BOX( templateChooser ), iter ) )
    {
        return FALSE;
    }

    if (id != NULL)
    {
        gtk_tree_model_get( GTK_TREE_MODEL( _priv->pListStore_ ), iter, ID, id, -1 );
    }
    if (templateName != NULL)
    {
        gtk_tree_model_get( GTK_TREE_MODEL( _priv->pListStore_ ), iter, KEY, templateName, -1 );
    }
    if (templateText != NULL)
    {
        gtk_tree_model_get( GTK_TREE_MODEL( _priv->pListStore_ ), iter, TEMPLATE, templateText, -1 );
    }
   if (helpText != NULL)
    {
        gtk_tree_model_get( GTK_TREE_MODEL( _priv->pListStore_ ), iter, HELP, helpText, -1 );
    }
   return TRUE;
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

gchar* template_chooser_get_template_name( TemplateChooser* templateChooser )
{
    GtkTreeIter _iter;
    gchar* _templateName = NULL;
    if ( !template_chooser_get_current_values( templateChooser, &_iter, NULL, &_templateName, NULL, NULL ) )
    {
        g_free( _templateName );
        return NULL;
    }
    else
    {
        return _templateName;
    }
}

gchar* template_chooser_get_template( TemplateChooser* templateChooser, int* id )
{
    GtkTreeIter _iter;
    gchar* _templateText = NULL;
    if ( !template_chooser_get_current_values( templateChooser, &_iter, id, NULL, &_templateText, NULL ) )
    {
        g_free( _templateText );
        return NULL;
    }
    else
    {
        return _templateText;
    }
}

gchar* template_chooser_get_help_text( TemplateChooser* templateChooser , int* id )
{
    GtkTreeIter _iter;
    gchar* _helpText = NULL;
    if ( !template_chooser_get_current_values( templateChooser, &_iter, id, NULL, NULL, &_helpText ) )
    {
        g_free( _helpText );
        return NULL;
    }
    else
    {
        return _helpText;
    }
}

void template_chooser_save_changes( TemplateChooser* templateChooser )
{
    TemplateChooserPrivate* _priv = TEMPLATE_CHOOSER_GET_PRIVATE( templateChooser );  
    template_list_store_export_file( _priv->pListStore_ ); 
}

void template_chooser_update_template( TemplateChooser* templateChooser, const gchar* text, int id )
{
    TemplateChooserPrivate* _priv = TEMPLATE_CHOOSER_GET_PRIVATE( templateChooser );
    GtkTreeIter _iter;
    int _id = -1;
    gchar* _templateName;

    template_chooser_get_current_values( templateChooser, &_iter, &_id, &_templateName, NULL, NULL );
    if (_id == id)
    {
        gtk_list_store_set( GTK_LIST_STORE( _priv->pListStore_ ), &_iter, TEMPLATE, text, -1 );
        _priv->pLogger_->Debug( g_strdup_printf( "Template for '%s' has been updated", _templateName ) );
    }
    else
    {
        _priv->pLogger_->Debug( g_strdup_printf( "Template id %d not updated - selection has changed", id ) );
    }

    g_free( _templateName );
}

void template_chooser_update_help_text( TemplateChooser* templateChooser, const gchar* text, int id )
{
    TemplateChooserPrivate* _priv = TEMPLATE_CHOOSER_GET_PRIVATE( templateChooser );
}