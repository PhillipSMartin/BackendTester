#include <algorithm>
#include <cctype>
#include <iostream>
#include <unistd.h>

#include "Parameters.h"

Parameters::~Parameters()
{
    g_free( tourneyId_ );
    g_free( workingDirectory_ );
}

Parameters::LogLevel Parameters::str_to_log_level( const gchar* logLevelStr )
{
    if ( 0 == g_strcmp0( logLevelStr, "DEBUG" ) ) { return LogLevel::DEBUG; }
    if ( 0 == g_strcmp0( logLevelStr, "INFO" ) ) { return LogLevel::INFO; }
    if ( 0 == g_strcmp0( logLevelStr, "WARN" ) ) { return LogLevel::WARN; }
    if ( 0 == g_strcmp0( logLevelStr, "ERROR" ) ) { return LogLevel::ERROR; }
    if ( 0 == g_strcmp0( logLevelStr, "FATAL" ) ) { return LogLevel::FATAL; }
    if ( 0 == g_strcmp0( logLevelStr, "NONE" ) ) { return LogLevel::NONE; }

    return LogLevel::UNKNOWN;
} 

const gchar* Parameters::log_level_to_str( Parameters::LogLevel const logLevel )
{
    switch (logLevel)
    {
        case LogLevel::DEBUG: return "DEBUG";
        case LogLevel::INFO: return "INFO";
        case LogLevel::WARN: return "WARN";
        case LogLevel::ERROR: return "ERROR";
        case LogLevel::FATAL: return "FATAL";
        case LogLevel::NONE: return "NONE";
        default: return "UNKNOWN";
   }
}

gchar* Parameters::get_default_working_directory( const char* programName ) const
{
    gchar* _executablePath = g_find_program_in_path( programName );
    gchar* _workingDirectory = g_path_get_dirname( _executablePath );
    g_free( _executablePath );
    return _workingDirectory;
}

// returns FALSE if unable to parse command-line arguments
// valid arguments are
//      -t:<id>          tourney id
//      -d:<path>        working directory (default is location of executable)
//      -l:<LogLevel>    log level for logfile
//      -c<:LogLevel>    log level for console
// valid LogLevels are
//      DEBUG, INFO, WARN, ERROR, FATAL
gboolean Parameters::parse_arguments( int const argc, char** const& argv )
{
    set_working_directory( get_default_working_directory( argv[0] ) );
 
    int _opt;
    gboolean _rc = TRUE;
    while ( -1 != (_opt = getopt( argc, argv, ":l:c:t:" )) )
    {
        std::string _optargstring = optarg;
        _optargstring.erase( std::remove_if( _optargstring.begin(), _optargstring.end(), ::isspace ), _optargstring.end() );
       switch (_opt)
        {
            case 'l': 
                fileLogLevel_ = str_to_log_level( _optargstring.c_str() );
                if ( fileLogLevel_ == LogLevel::UNKNOWN )
                {
                    _rc = FALSE;
                }
                break;
            case 'c': 
                consoleLogLevel_ = str_to_log_level( _optargstring.c_str() );
                if ( consoleLogLevel_ == LogLevel::UNKNOWN )
                {
                   _rc = FALSE;
                }
                break;
            case 't':
                set_tourney_id( _optargstring.c_str() );
                break;
            case 'd':
                set_working_directory( _optargstring.c_str() );
                break;
            default: 
               _rc = FALSE;
                break;
         }
    }

    if ( optind < argc )
    {
            _rc = FALSE;
    }

    if ( !_rc )
    {
        print_help();
    }

    return _rc;
}

void Parameters::print_help() const
{
        std::cout << "Usage:" << std::endl;
        std::cout << "   -t:<tourney id>" << std::endl;          
        std::cout << "   -d:<working directory> (default is location of executable)" << std::endl;
        std::cout << "   -l:<log level> (default is INFO)" << std::endl;
        std::cout << "   -c:<log level> (default is INFO)" << std::endl;
        std::cout << "       Log levels for logfile and console respectively" << std::endl;
        std::cout << "       Valid values are NONE, DEBUG, INFO, WARN, ERROR, and FATAL" << std::endl;
}
