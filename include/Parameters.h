#pragma once

#include <string>

class Parameters
{
    public:
        enum LogLevel { DEBUG, INFO, WARN, ERROR, FATAL, NONE, UNKNOWN };

    private:   
        LogLevel fileLogLevel_ = LogLevel::INFO;
        LogLevel consoleLogLevel_ = LogLevel::NONE;

    public:
        LogLevel get_fileLogLevel() const { return fileLogLevel_; }
        void set_fileLogLevel(LogLevel const logLevel) { fileLogLevel_ = logLevel; }

        LogLevel get_consoleLogLevel() const { return consoleLogLevel_; }
        void set_consoleLogLevel(LogLevel const logLevel) { consoleLogLevel_ = logLevel; }
        
        // returns -1 if unable to parse command-line arguments
        // valid arguments are
        //      -l<LogLevel>    log level for logfile
        //      -c<LogLevel>    log level for console
        // valid LogLevels are
        //      DEBUG, INFO, WARN, ERROR, FATAL
        int ParseArguments(int const argc, 
            char** const& argv);

        // converts string to LogLevel enum and back
        static LogLevel StrToLogLevel(std::string const& logLevelStr);
        static std::string LogLevelToStr(LogLevel const logLevel);
};