#include "JsonTreeViewModel.h"

#include <algorithm>
#include <gtk/gtk.h>
#include <iomanip>
#include <iostream>
#include <memory>
#include <stdlib.h>
#include <string>


using json = nlohmann::json;

static std::string _json_string = R"({"table":{"auction":[],"board":{"cards":[0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0],"dealer":0,"id":"","number":0,"vul":0},"cansee":[[true,false,false,false],[false,true,false,false],[false,false,true,false],[false,false,false,true]],"claim":{"by":0,"comment":"","decl":0,"moretime":false,"number":0,"result":0,"status":[0,0,0,0],"timeleft":0,"tricks":0},"dealid":"NoActiveDeal","elapsed":0,"flags":{"allowundocall":false,"allowundocard":false,"claimip":false,"dircall":false,"dircallip":false,"freeze":false,"phaseip":false,"undoip":false},"legend":"Test Game","move":[{"playerid":"","seat":0,"tableid":""},{"playerid":"","seat":0,"tableid":""},{"playerid":"","seat":0,"tableid":""},{"playerid":"","seat":0,"tableid":""}],"params":{"bpr":0,"round":0,"rounds":0,"tables":0},"play":{"contract":"","declarer":0,"hand":0,"index":0,"layout":[0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0],"suit":0,"tricks":[[0,0,0,0,0],[0,0,0,0,0],[0,0,0,0,0],[0,0,0,0,0],[0,0,0,0,0],[0,0,0,0,0],[0,0,0,0,0],[0,0,0,0,0],[0,0,0,0,0],[0,0,0,0,0],[0,0,0,0,0],[0,0,0,0,0],[0,0,0,0,0]],"trumps":0,"won_ew":0,"won_ns":0},"players":[{"id":""},{"id":""},{"id":""},{"id":""}],"queries":[],"rdtime":0,"result":{"award":0.0,"id":"","raw":0,"score":"","summary":""},"sitout":false,"tableid":"lh-test-1","timeleft":-1655913193686,"trigger":{"action":0,"type":"command"},"tstamp":1655913193685,"turn":{"canundo":[false,false,false,false],"dbl":0,"message":"","phase":0,"status":0,"who":0},"undo":{"by":0,"phase":0,"result":0,"status":[0,0,0,0],"timeleft":0}}})";

enum
{
    KEY = 0,
    VALUE,
    KEY_PATH,
    POINTER,
    TYPE,
    NUM_COLUMNS
};

#if 0
static void step_through(json const& j, int indent)
{
    std::string _pad;
    _pad.append(indent, ' ');
    for (auto& it : j.items())
    {
        if (it.value().is_object())
        {
            if (it.value().size() == 0)
            {
                std::cout << _pad  << it.key() << ": {}" << std::endl;
            }
            else
            {
                std::cout << _pad  << it.key() << ":" << std::endl;
                json child = it.value();
                step_through(child, indent+4);
            }
        }
        else if (it.value().is_array())
        {
            if (it.value().size() == 0)
            {
                std::cout << _pad  << it.key() << ": []" << std::endl;
            }
            else if (it.value().at(0).is_structured())
            {
                std::cout << _pad  << it.key() << ":" << std::endl;
                json child = it.value();
                step_through(child, indent+4);
            }
            else
            {
                std::cout << _pad << it.key() << ": " << it.value() << std::endl;            
            }
        }
        else
        {
            std::cout << _pad << it.key() << ": " << it.value() << std::endl;
        }
    }
}
#endif

static void setup_model(GtkTreeStore* const, json, GtkTreeIter* const, gchar* const);
static void OnCellEdited(GtkCellRendererText*, gchar*, gchar*, JsonTreeViewModel*);
static json* get_json_object(json* const, gchar* const);

int main(int argc, char** argv) 
{
    gtk_init(&argc, &argv);

    json* _pJson = new json(json::parse(_json_string));
    GtkWidget* _window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    GtkWidget*  _scrolledWindow = gtk_scrolled_window_new(NULL, NULL);
    JsonTreeViewModel _model(_pJson);
    GtkCellRenderer* _renderer;

    _renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_append_column(GTK_TREE_VIEW(_model.get_TreeView()), 
    gtk_tree_view_column_new_with_attributes("Key", _renderer, "text", 0, NULL));

    _renderer = gtk_cell_renderer_text_new();
    g_object_set(_renderer, "editable", TRUE, "editable-set", TRUE, NULL);
    g_signal_connect(G_OBJECT (_renderer), "edited", G_CALLBACK (OnCellEdited), (gpointer) &_model);
    gtk_tree_view_append_column(GTK_TREE_VIEW(_model.get_TreeView()),     
        gtk_tree_view_column_new_with_attributes("Value", _renderer, "text", 1, NULL));

    gtk_window_set_title(GTK_WINDOW(_window), "Backend Tester");
    gtk_container_set_border_width(GTK_CONTAINER(_window), 10);
    gtk_widget_set_size_request(_window, 1800, 800);  
    g_signal_connect(G_OBJECT(_window), "destroy", G_CALLBACK(gtk_main_quit), NULL); 

    setup_model(_model.get_TreeStore(), *_model.get_Json(), NULL, NULL);

    g_object_unref(_model.get_TreeStore());
    GtkTreePath* _firstRow = gtk_tree_path_new_first();
    gtk_tree_view_expand_row(GTK_TREE_VIEW (_model.get_TreeView()), _firstRow, FALSE);
    gtk_tree_path_free(_firstRow);

    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (_scrolledWindow), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_container_add(GTK_CONTAINER(_scrolledWindow), _model.get_TreeView());
    gtk_container_add(GTK_CONTAINER(_window), _scrolledWindow);
    gtk_widget_show_all(_window);
    
    gtk_main();
    return 0;
}

static void OnCellEdited(GtkCellRendererText* renderer, gchar* path, gchar* newText, JsonTreeViewModel* model)
{
    GtkTreeIter _iter;
    GtkTreeModel* _model = gtk_tree_view_get_model(GTK_TREE_VIEW(model->get_TreeView()));
  
    if (gtk_tree_model_get_iter_from_string(_model, &_iter, path))
    {
        gchar* _typeName;
        gchar* _keyPath;

        gtk_tree_model_get(_model, &_iter, TYPE, &_typeName, KEY_PATH, &_keyPath, -1);
     
        json* _pJson = get_json_object(model->get_Json(), _keyPath);

        if (g_strcmp0(_typeName, "number") == 0)
        {   
            *_pJson = atoi(newText);
        }
        else if (g_strcmp0(_typeName, "string") == 0)
        { 
            std::string s(newText);
            s.erase(remove(s.begin(), s.end(), '"'), s.end());
            *_pJson = s.c_str();
        }
        else if (g_strcmp0(_typeName, "boolean") == 0)
        {
            if (g_str_has_prefix(newText, "t") || g_str_has_prefix(newText, "T"))
            {
                *_pJson = true;
            }
            else
            {
                *_pJson = false;
            }
        }

        std::ostringstream _os;
        _os << *_pJson;
        gtk_tree_store_set(GTK_TREE_STORE(_model), &_iter, VALUE, _os.str().c_str(), -1);
        std::cout << nlohmann::to_string(*model->get_Json()) << std::endl;     
    }
}

static json* get_json_object(json* j, gchar* const keyPath)
{
    json* _pJson = j;
    gchar** _keys = g_strsplit(keyPath, ":", -1);
    for(int i=0; _keys[i] != NULL; i++)
    {
        if (_pJson->type() == json::value_t::array)
        {
            _pJson = &(_pJson->at(atoi(_keys[i])));
        }
        else
        {
            _pJson = &(_pJson->at(_keys[i]));
        }
    }

    g_strfreev(_keys);
    return _pJson;
}

static void setup_model(GtkTreeStore* const model, json j, GtkTreeIter* const parent, gchar* const keyPath)
{
    GtkTreeIter _iter; 
    for (auto& _it : j.items())
    {
        gchar* _newKeyPath = (parent == NULL) ?
                g_strdup(_it.key().c_str()) :
                g_strjoin(":", keyPath, _it.key().c_str(), NULL);
         
        gtk_tree_store_append(model, &_iter, parent);
        if (_it.value().is_structured())
        {
            gtk_tree_store_set(model, &_iter, KEY, _it.key().c_str(), VALUE, "", KEY_PATH, _newKeyPath, TYPE, _it.value().type_name(), -1);
            if (_it.value().size() > 0)
            {
                setup_model(model, _it.value(), &_iter, _newKeyPath);
            }
        }
        else
        {
            std::ostringstream _os;
            _os << _it.value();
            gtk_tree_store_set(model, &_iter, KEY, _it.key().c_str(), VALUE, _os.str().c_str(), KEY_PATH, _newKeyPath, POINTER, gpointer(&_it.value()), TYPE, _it.value().type_name(), -1);
        }
//        g_free(_newKeyPath);
   }
}