#pragma once

#include <string>

class Parameters
{
    public:
        enum LogLevel { DEBUG, INFO, WARN, ERROR, FATAL, NONE, UNKNOWN };

    private:   
        LogLevel fileLogLevel_ = LogLevel::INFO;
        LogLevel consoleLogLevel_ = LogLevel::INFO;
        std::string tourneyId_;

    public:
        LogLevel get_file_log_level() const { return fileLogLevel_; }
        void set_file_log_level(LogLevel const logLevel) { fileLogLevel_ = logLevel; }

        LogLevel get_console_log_level() const { return consoleLogLevel_; }
        void set_console_log_level(LogLevel const logLevel) { consoleLogLevel_ = logLevel; }

        std::string get_tourney_id() const { return tourneyId_; }
        void set_tourney_id(std::string const tourneyId) { tourneyId_ = tourneyId; }
        
        // returns -1 if unable to parse command-line arguments
        // valid arguments are
        //      -t<id>          tourney id
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