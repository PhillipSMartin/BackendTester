#include "Parameters.h"

#include <algorithm>
#include <cctype>
#include <iostream>
#include <unistd.h>

Parameters::LogLevel Parameters::StrToLogLevel(std::string const& logLevelStr)
{
    std::string _s = logLevelStr;
    _s.erase(std::remove_if(_s.begin(), _s.end(), ::isspace), _s.end());
    if (_s == "DEBUG") { return LogLevel::DEBUG; }
    if (_s == "INFO") { return LogLevel::INFO; }
    if (_s == "WARN") { return LogLevel::WARN; }
    if (_s == "ERROR") { return LogLevel::ERROR; }
    if (_s == "FATAL") { return LogLevel::FATAL; }
    if (_s == "NONE") { return LogLevel::NONE; }

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
//      -l<LogLevel>    log level for logfile
//      -c<LogLevel>    log level for console
// valid LogLevels are
//      DEBUG, INFO, WARN, ERROR, FATAL
int Parameters::ParseArguments(int const argc,
    char** const& argv)
{
    int _opt;
    int _rc = 0;
    while ( -1 != (_opt = getopt( argc, argv, ":l:c:" )) )
    {
        switch ( _opt )
        {
            case 'l': 
                fileLogLevel_ = StrToLogLevel(optarg);
                if (fileLogLevel_ == LogLevel::UNKNOWN)
                {
                    _rc = -1;
                }
                break;
            case 'c': 
                consoleLogLevel_ = StrToLogLevel(optarg);
                if (consoleLogLevel_ == LogLevel::UNKNOWN)
                {
                   _rc = -1;
                }
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
        std::cout << "   -c<log level> (default is NONE)" << std::endl;
        std::cout << "       Log levels for logfile and console respectively" << std::endl;
        std::cout << "       Valid values are NONE, DEBUG, INFO, WARN, ERROR, and FATAL" << std::endl;
    }

    return _rc;
}

