#include "JsonTreeViewModel.h"

static std::string _json_string = R"({"table":{"auction":[],"board":{"cards":[0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0],"dealer":0,"id":"","number":0,"vul":0},"cansee":[[true,false,false,false],[false,true,false,false],[false,false,true,false],[false,false,false,true]],"claim":{"by":0,"comment":"","decl":0,"moretime":false,"number":0,"result":0,"status":[0,0,0,0],"timeleft":0,"tricks":0},"dealid":"NoActiveDeal","elapsed":0,"flags":{"allowundocall":false,"allowundocard":false,"claimip":false,"dircall":false,"dircallip":false,"freeze":false,"phaseip":false,"undoip":false},"legend":"Test Game","move":[{"playerid":"","seat":0,"tableid":""},{"playerid":"","seat":0,"tableid":""},{"playerid":"","seat":0,"tableid":""},{"playerid":"","seat":0,"tableid":""}],"params":{"bpr":0,"round":0,"rounds":0,"tables":0},"play":{"contract":"","declarer":0,"hand":0,"index":0,"layout":[0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0],"suit":0,"tricks":[[0,0,0,0,0],[0,0,0,0,0],[0,0,0,0,0],[0,0,0,0,0],[0,0,0,0,0],[0,0,0,0,0],[0,0,0,0,0],[0,0,0,0,0],[0,0,0,0,0],[0,0,0,0,0],[0,0,0,0,0],[0,0,0,0,0],[0,0,0,0,0]],"trumps":0,"won_ew":0,"won_ns":0},"players":[{"id":""},{"id":""},{"id":""},{"id":""}],"queries":[],"rdtime":0,"result":{"award":0.0,"id":"","raw":0,"score":"","summary":""},"sitout":false,"tableid":"lh-test-1","timeleft":-1655913193686,"trigger":{"action":0,"type":"command"},"tstamp":1655913193685,"turn":{"canundo":[false,false,false,false],"dbl":0,"message":"","phase":0,"status":0,"who":0},"undo":{"by":0,"phase":0,"result":0,"status":[0,0,0,0],"timeleft":0}}})";

int main(int argc, char** argv) 
{
    // build Parameters instance from command-line arguments
    auto _pParms = std::make_shared<Parameters>();
    if (0 != _pParms->ParseArguments(argc, argv))
    {
        exit(0);
    }

    // start Logger
    auto _pLogger = std::make_shared<Logger>(_pParms);
    _pLogger->Info("Backend Tester started");

    // create json object from test string
    auto _pJson = std::make_shared<nlohmann::json>(nlohmann::json::parse(_json_string));

    // set up GUI
    gtk_init(&argc, &argv);
    GtkWidget* _window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    GtkWidget*  _scrolledWindow = gtk_scrolled_window_new(NULL, NULL);
    JsonTreeViewModel _model(_pLogger);
    _model.set_pJson(_pJson);  

    gtk_window_set_title(GTK_WINDOW(_window), "Backend Tester");
    gtk_container_set_border_width(GTK_CONTAINER(_window), 10);
    gtk_widget_set_size_request(_window, 1800, 800);  
    g_signal_connect(G_OBJECT(_window), "destroy", G_CALLBACK(gtk_main_quit), NULL); 
 
    GtkTreePath* _firstRow = gtk_tree_path_new_first();
    gtk_tree_view_expand_row(GTK_TREE_VIEW (_model.get_TreeView()), _firstRow, FALSE);
    gtk_tree_path_free(_firstRow);

    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (_scrolledWindow), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_container_add(GTK_CONTAINER(_scrolledWindow), _model.get_TreeView());
    gtk_container_add(GTK_CONTAINER(_window), _scrolledWindow);
    gtk_widget_show_all(_window);
    
    gtk_main();

    _pLogger->Info("Backend Tester exited");
    return 0;
}
