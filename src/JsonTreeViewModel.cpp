#include "JsonTreeViewModel.h"

#include <gtk/gtk.h>
#include <sstream>

JsonTreeViewModel::JsonTreeViewModel(std::shared_ptr<Logger> pLogger) : 
    pTreeView_(gtk_tree_view_new()),
    pTreeStore_(gtk_tree_store_new(NUM_COLUMNS, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_BOOLEAN)),
    pLogger_(pLogger)
{
    GtkCellRenderer* _renderer;

    gtk_tree_view_set_model(GTK_TREE_VIEW(pTreeView_), GTK_TREE_MODEL(pTreeStore_));
    g_object_unref(pTreeStore_);

    _renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_append_column(GTK_TREE_VIEW(pTreeView_), 
        gtk_tree_view_column_new_with_attributes("Key", _renderer, "text", KEY, NULL));

    _renderer = gtk_cell_renderer_text_new();
 //   g_object_set(_renderer, "editable", TRUE, "editable-set", TRUE, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(pTreeView_),     
        gtk_tree_view_column_new_with_attributes("Value", _renderer, "text", VALUE, "editable", EDITABLE, NULL));

    g_signal_connect(G_OBJECT(_renderer), "edited", G_CALLBACK(OnCellEdited), gpointer(this));
}

void JsonTreeViewModel::set_pJson(std::shared_ptr<nlohmann::json> pJson) 
{ 
    pJson_  = pJson; 
    gtk_tree_store_clear(pTreeStore_);
    fill_data(pJson_.get()); 
}

void JsonTreeViewModel::fill_data(nlohmann::json* const pBranch, GtkTreeIter* const parent, gchar* const keyPath)
{
    GtkTreeIter _iter; 
    for (auto& _it : pBranch->items())
    {
        gchar* _newKeyPath = (parent == NULL) ?
                g_strdup(_it.key().c_str()) :
                g_strjoin(":", keyPath, _it.key().c_str(), NULL);
         
        gtk_tree_store_append(pTreeStore_, &_iter, parent);
        if (_it.value().is_structured())
        {
            gtk_tree_store_set(pTreeStore_, &_iter, KEY, _it.key().c_str(), VALUE, "", KEY_PATH, _newKeyPath, EDITABLE, FALSE, -1);
            if (_it.value().size() > 0)
            {
                fill_data(&_it.value(), &_iter, _newKeyPath);
            }
        }
        else
        {
            std::ostringstream _os;
            _os << _it.value();
            gtk_tree_store_set(pTreeStore_, &_iter, KEY, _it.key().c_str(), VALUE, _os.str().c_str(), KEY_PATH, _newKeyPath, EDITABLE, TRUE, -1);
        }
    }
}

void JsonTreeViewModel::OnCellEdited(GtkCellRendererText* renderer, gchar* path, gchar* newText, JsonTreeViewModel* model)
{
    GtkTreeIter _iter;
 
    if (gtk_tree_model_get_iter_from_string(GTK_TREE_MODEL(model->pTreeStore_), &_iter, path))
    {
        gchar* _typeName;
        gchar* _keyPath;
        gchar* _errorMsg;
        gtk_tree_model_get(GTK_TREE_MODEL(model->pTreeStore_), &_iter, KEY_PATH, &_keyPath, -1);
        
        nlohmann::json* _pJson = get_json_branch(model->get_pJson(), _keyPath);
        switch (_pJson->type())
        {
            case nlohmann::json::value_t::number_integer:
            case nlohmann::json::value_t::number_unsigned:
                *_pJson = atoi(newText);
                break;
            case nlohmann::json::value_t::number_float:
                *_pJson = atof(newText);
                break;
            case nlohmann::json::value_t::string:
                {
                    std::string s(newText);
                    s.erase(remove(s.begin(), s.end(), '"'), s.end());
                    *_pJson =  s.c_str();
                }
                break;
            case nlohmann::json::value_t::boolean:
                *_pJson = (g_str_has_prefix(newText, "t") || g_str_has_prefix(newText, "T"));
                break;
            default: 
                _errorMsg = g_strdup_printf("Unhandled type \"%s\" for json item \"%s\"", _pJson->type_name(), _keyPath);           
                model->pLogger_->Error(_errorMsg);
                g_free(_errorMsg);
                return;
        }

        std::ostringstream _os;
        _os << *_pJson;
        gtk_tree_store_set(model->pTreeStore_, &_iter, VALUE, _os.str().c_str(), -1);
        model->pLogger_->Debug(nlohmann::to_string(*model->get_pJson()));     
    }
}

nlohmann::json* JsonTreeViewModel::get_json_branch(nlohmann::json* const pJson, gchar* const keyPath)
{
    nlohmann::json* _pJson = pJson;
    gchar** _keys = g_strsplit(keyPath, ":", -1);
    for(int i=0; _keys[i] != NULL; i++)
    {
        if (_pJson->type() == nlohmann::json::value_t::array)
        {
            _pJson = &_pJson->at(atoi(_keys[i]));
        }
        else
        {
            _pJson = &_pJson->at(_keys[i]);
        }
    }

    g_strfreev(_keys);
    return _pJson;
}
