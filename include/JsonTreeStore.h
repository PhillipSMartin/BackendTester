#pragma once

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define JSON_TREE_STORE_TYPE                ( json_tree_store_get_type() )
#define JSON_TREE_STORE( obj )              ( G_TYPE_CHECK_INSTANCE_CAST( ( obj ), JSON_TREE_STORE_TYPE, JsonTreeStore ) )
#define JSON_TREE_STORE_CLASS( klass )      ( G_TYPE_CHECK_CLASS_CAST( (klass), JSON_TREE_STORE_TYPE, JsonTreeStoreClass ) )
#define IS_JSON_TREE_STORE( obj )           ( G_TYPE_CHECK_INSTANCE_TYPE( (obj), JSON_TREE_STORE_TYPE ) )
#define IS_JSON_TREE_STORE_CLASS( klass )   ( G_TYPE_CHECK_CLASS_TYPE( (klass), JSON_TREE_STORE_TYPE ) )

typedef struct _JsonTreeStore               JsonTreeStore;
typedef struct _JsonTreeStoreClass          JsonTreeStoreClass;

struct _JsonTreeStore
{
    GtkTreeStore treeStore_;
};

struct _JsonTreeStoreClass
{
    GtkTreeStoreClass parentClass_;

    void (* json_edited) ( JsonTreeStore* jsonTreeStore );
    void (* json_replaced) ( JsonTreeStore* jsonTreeStore );
};

GType json_tree_store_get_type( void ) G_GNUC_CONST;
GtkWidget*  json_tree_store_new( void );

gchar* json_tree_store_get_json_string( JsonTreeStore* jsonTreeStore );
void json_tree_store_set_json_string( JsonTreeStore* jsonTreeStore , const gchar* text );

G_END_DECLS