#include "Dashboard.h"
#include "nlohmann/json.hpp"

static std::string _json_string = R"({"table":{"auction":[],"board":{"cards":[0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0],"dealer":0,"id":"","number":0,"vul":0},"cansee":[[true,false,false,false],[false,true,false,false],[false,false,true,false],[false,false,false,true]],"claim":{"by":0,"comment":"","decl":0,"moretime":false,"number":0,"result":0,"status":[0,0,0,0],"timeleft":0,"tricks":0},"dealid":"NoActiveDeal","elapsed":0,"flags":{"allowundocall":false,"allowundocard":false,"claimip":false,"dircall":false,"dircallip":false,"freeze":false,"phaseip":false,"undoip":false},"legend":"Test Game","move":[{"playerid":"","seat":0,"tableid":""},{"playerid":"","seat":0,"tableid":""},{"playerid":"","seat":0,"tableid":""},{"playerid":"","seat":0,"tableid":""}],"params":{"bpr":0,"round":0,"rounds":0,"tables":0},"play":{"contract":"","declarer":0,"hand":0,"index":0,"layout":[0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0],"suit":0,"tricks":[[0,0,0,0,0],[0,0,0,0,0],[0,0,0,0,0],[0,0,0,0,0],[0,0,0,0,0],[0,0,0,0,0],[0,0,0,0,0],[0,0,0,0,0],[0,0,0,0,0],[0,0,0,0,0],[0,0,0,0,0],[0,0,0,0,0],[0,0,0,0,0]],"trumps":0,"won_ew":0,"won_ns":0},"players":[{"id":""},{"id":""},{"id":""},{"id":""}],"queries":[],"rdtime":0,"result":{"award":0.0,"id":"","raw":0,"score":"","summary":""},"sitout":false,"tableid":"lh-test-1","timeleft":-1655913193686,"trigger":{"action":0,"type":"command"},"tstamp":1655913193685,"turn":{"canundo":[false,false,false,false],"dbl":0,"message":"","phase":0,"status":0,"who":0},"undo":{"by":0,"phase":0,"result":0,"status":[0,0,0,0],"timeleft":0}}})";
//static std::string _json_string = R"({"type":"player","action":1,"seat":1,"id":"1"})";

int main( int argc, char** argv ) 
{
    gtk_init( &argc, &argv ); // needed for Parameters::ParseArguments

    // build Parameters instance from command-line arguments
    Parameters _parms{};
    if (!_parms.parse_arguments( argc, argv ) )
    {
        exit( 0 );
    }

    // start Logger
    Logger _logger(&_parms);
     _logger.InfoStr( "Backend Tester started" );

    Dashboard _Dashboard(&_parms, &_logger); 
    _Dashboard.set_subscribe_message( _json_string.c_str() );
    _Dashboard.run();

    _logger.InfoStr( "Backend Tester exited" );
    return 0;
}
