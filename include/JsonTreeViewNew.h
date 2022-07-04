#pragma once

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define JSON_TREE_VIEW_TYPE                 ( json_tree_view_get_type() )
#define JSON_TREE_VIEW( obj )               ( G_TYPE_CHECK_INSTANCE_CAST( (obj), JSON_TREE_VIEW_TYPE, JsonTreeView ) )
#define JSON_TREE_VIEW_CLASS( klass )       ( G_TYPE_CHECK_CLASS_CAST( (klass), JSON_TREE_VIEW_TYPE, JsonTreeViewClass ) )
#define IS_JSON_TREE_VIEW( obj )            ( G_TYPE_CHECK_INSTANCE_TYPE( (obj), JSON_TREE_VIEW_TYPE ) )
#define IS_JSON_TREE_VIEW_CLASS( klass )    ( G_TYPE_CHECK_CLASS_TYPE( (klass), JSON_TREE_VIEW_TYPE ) )


typedef struct _JsonTreeView                JsonTreeView;
typedef struct _JsonTreeViewClass           JsonTreeViewClass;

struct _JsonTreeView
{
    GtkTreeView treeView_;
};

struct _JsonTreeViewClass
{
    GtkTreeViewClass parentClass_;

    void (* json_edited) ( JsonTreeView *jsonTreeView );
    void (* json_replaced) ( JsonTreeView *jsonTreeView );
};

GType json_tree_view_get_type( void ) G_GNUC_CONST;
GtkWidget*  json_tree_view_new( void );

gchar* json_tree_view_get_text( JsonTreeView* jsonTreeView );
void json_tree_view_set_text( sonTreeView* jsonTreeView , const gchar* text );

G_END_DECLS