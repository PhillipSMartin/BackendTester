#pragma once
#include <gtk/gtk.h>

#include "Logger.h"

// TemplateChooser
//
//      a combo box for selecting pre-defined templates
//
//  Public methods:
//
//  Signals:
//      changed
//          inherited from combo box


G_BEGIN_DECLS

#define TEMPLATE_CHOOSER_TYPE                ( template_chooser_get_type() )
#define TEMPLATE_CHOOSER( obj )              ( G_TYPE_CHECK_INSTANCE_CAST( ( obj ), TEMPLATE_CHOOSER_TYPE, TemplateChooser ) )
#define TEMPLATE_CHOOSER_CLASS( klass )      ( G_TYPE_CHECK_CLASS_CAST( (klass), TEMPLATE_CHOOSER_TYPE, TemplateChooserClass ) )
#define IS_TEMPLATE_CHOOSER( obj )           ( G_TYPE_CHECK_INSTANCE_TYPE( (obj), TEMPLATE_CHOOSER_TYPE ) )
#define IS_TEMPLATE_CHOOSER_CLASS( klass )   ( G_TYPE_CHECK_CLASS_TYPE( (klass), TEMPLATE_CHOOSER_TYPE ) )

#define TEMPLATE_FILE_NAME( topic )          ( g_strdup_printf( "%s.templates.txt", (topic) ) )

typedef struct _TemplateChooser               TemplateChooser;
typedef struct _TemplateChooserClass          TemplateChooserClass;

struct _TemplateChooser
{
    GtkComboBox comboBox_;
};

struct _TemplateChooserClass
{
    GtkComboBoxClass parentClass_;

    void (* json_edited) ( TemplateChooser* templateChooser );
    void (* json_replaced) ( TemplateChooser* templateChooser );
};

GType template_chooser_get_type( void ) G_GNUC_CONST;
GtkWidget* template_chooser_new( Logger* logger, std::string workingDirectory );

// retrieves fields from active selection
// caller assumes ownership of the cstrings and must free them
gchar* template_chooser_get_template_name( TemplateChooser* templatechooser );
gchar* template_chooser_get_template( TemplateChooser* templatechooser, int* id );
gchar* template_chooser_get_help_text( TemplateChooser* templatechooser, int* id );

G_END_DECLS