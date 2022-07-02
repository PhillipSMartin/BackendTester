#include "Dashboard.h"
#include "TemplateMap.h"

static std::string _json_string = R"({"table":{"auction":[],"board":{"cards":[0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0],"dealer":0,"id":"","number":0,"vul":0},"cansee":[[true,false,false,false],[false,true,false,false],[false,false,true,false],[false,false,false,true]],"claim":{"by":0,"comment":"","decl":0,"moretime":false,"number":0,"result":0,"status":[0,0,0,0],"timeleft":0,"tricks":0},"dealid":"NoActiveDeal","elapsed":0,"flags":{"allowundocall":false,"allowundocard":false,"claimip":false,"dircall":false,"dircallip":false,"freeze":false,"phaseip":false,"undoip":false},"legend":"Test Game","move":[{"playerid":"","seat":0,"tableid":""},{"playerid":"","seat":0,"tableid":""},{"playerid":"","seat":0,"tableid":""},{"playerid":"","seat":0,"tableid":""}],"params":{"bpr":0,"round":0,"rounds":0,"tables":0},"play":{"contract":"","declarer":0,"hand":0,"index":0,"layout":[0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0],"suit":0,"tricks":[[0,0,0,0,0],[0,0,0,0,0],[0,0,0,0,0],[0,0,0,0,0],[0,0,0,0,0],[0,0,0,0,0],[0,0,0,0,0],[0,0,0,0,0],[0,0,0,0,0],[0,0,0,0,0],[0,0,0,0,0],[0,0,0,0,0],[0,0,0,0,0]],"trumps":0,"won_ew":0,"won_ns":0},"players":[{"id":""},{"id":""},{"id":""},{"id":""}],"queries":[],"rdtime":0,"result":{"award":0.0,"id":"","raw":0,"score":"","summary":""},"sitout":false,"tableid":"lh-test-1","timeleft":-1655913193686,"trigger":{"action":0,"type":"command"},"tstamp":1655913193685,"turn":{"canundo":[false,false,false,false],"dbl":0,"message":"","phase":0,"status":0,"who":0},"undo":{"by":0,"phase":0,"result":0,"status":[0,0,0,0],"timeleft":0}}})";
//static std::string _json_string = R"({"type":"player","action":1,"seat":1,"id":"1"})";

static void OnMainWindowDestroy( GtkWidget*, Logger* );

int main( int argc, char** argv ) 
{
    // build Parameters instance from command-line arguments
    Parameters _parms{};
    if (!_parms.ParseArguments( argc, argv ) )
    {
        exit( 0 );
    }

    // start Logger
    Logger _logger(&_parms);
     _logger.Info( "Backend Tester started" );

    // create json object from test string
    auto _json = std::make_shared<nlohmann::json>( nlohmann::json::parse( _json_string ) );

    // set up GUI
    gtk_init( &argc, &argv );
    GtkWidget* _pWindow = gtk_window_new( GTK_WINDOW_TOPLEVEL );
 
    gtk_window_set_title( GTK_WINDOW( _pWindow ), "Backend Tester" );
    gtk_container_set_border_width( GTK_CONTAINER( _pWindow ), 10 ); 
    g_signal_connect( G_OBJECT(_pWindow), "destroy", G_CALLBACK( OnMainWindowDestroy ), gpointer( &_logger ) ); 

    Dashboard _Dashboard(&_parms, &_logger);
    gtk_container_add( GTK_CONTAINER( _pWindow ), _Dashboard.get_parent() );   
    _Dashboard.set_subscribe_message( _json );

    gtk_widget_show_all( _pWindow );  
    gtk_main();

    _logger.Info( "Backend Tester exited" );
    return 0;
}

static void OnMainWindowDestroy( GtkWidget* window, Logger* pLogger )
{
    pLogger->set_text_buffer( NULL );
    gtk_main_quit();
}