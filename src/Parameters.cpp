#include "Parameters.h"

#include <algorithm>
#include <cctype>
#include <iostream>
#include <unistd.h>

Parameters::LogLevel Parameters::StrToLogLevel(std::string const& logLevelStr)
{
    if ( logLevelStr == "DEBUG" ) { return LogLevel::DEBUG; }
    if ( logLevelStr == "INFO" ) { return LogLevel::INFO; }
    if ( logLevelStr == "WARN" ) { return LogLevel::WARN; }
    if ( logLevelStr == "ERROR" ) { return LogLevel::ERROR; }
    if ( logLevelStr == "FATAL" ) { return LogLevel::FATAL; }
    if ( logLevelStr == "NONE" ) { return LogLevel::NONE; }

    return LogLevel::UNKNOWN;
} 

std::string Parameters::LogLevelToStr(Parameters::LogLevel const logLevel)
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

// returns -1 if unable to parse command-line arguments
// valid arguments are
//      -t<id>          tourney id
//      -l<LogLevel>    log level for logfile
//      -c<LogLevel>    log level for console
// valid LogLevels are
//      DEBUG, INFO, WARN, ERROR, FATAL
int Parameters::ParseArguments(int const argc,
    char** const& argv)
{
    int _opt;
    int _rc = 0;
    while ( -1 != (_opt = getopt( argc, argv, ":l:c:t:" )) )
    {
        std::string _optargstring = optarg;
        _optargstring.erase(std::remove_if(_optargstring.begin(), _optargstring.end(), ::isspace), _optargstring.end());
       switch ( _opt )
        {
            case 'l': 
                fileLogLevel_ = StrToLogLevel(_optargstring);
                if (fileLogLevel_ == LogLevel::UNKNOWN)
                {
                    _rc = -1;
                }
                break;
            case 'c': 
                consoleLogLevel_ = StrToLogLevel(_optargstring);
                if (consoleLogLevel_ == LogLevel::UNKNOWN)
                {
                   _rc = -1;
                }
                break;
            case 't':
                tourneyId_ = _optargstring;
                break;
            default: 
               _rc = -1;
                break;
         }
    }

    if (optind < argc)
    {
            _rc = -1;
    }

    if (_rc < 0)
    {
        std::cout << "Usage:" << std::endl;
        std::cout << "   -l<log level> (default is INFO)" << std::endl;
        std::cout << "   -c<log level> (default is INFO)" << std::endl;
        std::cout << "       Log levels for logfile and console respectively" << std::endl;
        std::cout << "       Valid values are NONE, DEBUG, INFO, WARN, ERROR, and FATAL" << std::endl;
    }

    return _rc;
}

