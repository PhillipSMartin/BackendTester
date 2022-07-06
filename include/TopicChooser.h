#pragma once

#include <gtk/gtk.h>

#include "Logger.h"
#include "Parameters.h"

// TopicChooser
//
//      a collection of widgets for determining a topic
//
//  Public methods:
//      gchar* topic_chooser_get_topic()
//          caller takes ownership of the string
//      gchar* topic_chooser_get_topic_prefix()
//          caller takes ownership of the string
//
//  Signals:
//      changed
//          topic has been changed


G_BEGIN_DECLS

#define TOPIC_CHOOSER_TYPE                ( topic_chooser_get_type() )
#define TOPIC_CHOOSER( obj )              ( G_TYPE_CHECK_INSTANCE_CAST( ( obj ), TOPIC_CHOOSER_TYPE, TopicChooser ) )
#define TOPIC_CHOOSER_CLASS( klass )      ( G_TYPE_CHECK_CLASS_CAST( (klass), TOPIC_CHOOSER_TYPE, TopicChooserClass ) )
#define IS_TOPIC_CHOOSER( obj )           ( G_TYPE_CHECK_INSTANCE_TYPE( (obj), TOPIC_CHOOSER_TYPE ) )
#define IS_TOPIC_CHOOSER_CLASS( klass )   ( G_TYPE_CHECK_CLASS_TYPE( (klass), TOPIC_CHOOSER_TYPE ) )

typedef struct _TopicChooser               TopicChooser;
typedef struct _TopicChooserClass          TopicChooserClass;

struct _TopicChooser
{
    GtkBox box_;
};

struct _TopicChooserClass
{
    GtkBoxClass parentClass_;

    void (* changed) ( TopicChooser* topicChooser );
};

enum TOPICS 
{
    EVENT_TB = 0,
    EVENT_TM,
    STATE_TB,
    STATE_TM
};

GType topic_chooser_get_type( void ) G_GNUC_CONST;
GtkWidget* topic_chooser_new( Parameters* pParms, Logger* logger, TOPICS defaultTopic );

// caller assumes ownership of the cstring and must free it
gchar* topic_chooser_get_topic( TopicChooser* topicchooser );
gchar* topic_chooser_get_topic_prefix( TopicChooser* topicchooser );

G_END_DECLS
