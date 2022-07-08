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
};

GType template_chooser_get_type( void ) G_GNUC_CONST;
GtkWidget* template_chooser_new( Logger* logger, const char* workingDirectory );

// retrieves fields from active selection
// caller assumes ownership of the string and must free it
gchar* template_chooser_get_template_name( TemplateChooser* templateChooser );
gchar* template_chooser_get_template( TemplateChooser* templateChooser, int* id );
gchar* template_chooser_get_help_text( TemplateChooser* templateChooser, int* id );

void template_chooser_save_changes( TemplateChooser* templateChooser );

// caller retains ownership of the string
void template_chooser_update_template( TemplateChooser* templateChooser, const gchar* text, int id );
void template_chooser_update_help_text( TemplateChooser* templateChooser, const gchar* text, int id );

G_END_DECLS