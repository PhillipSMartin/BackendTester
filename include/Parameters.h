#pragma once

#include <gtk/gtk.h>
#include <string>

class Parameters
{
    public:
        enum LogLevel { DEBUG, INFO, WARN, ERROR, FATAL, NONE, UNKNOWN };

    private:   
        LogLevel fileLogLevel_ = LogLevel::INFO;
        LogLevel consoleLogLevel_ = LogLevel::INFO;
        gchar* tourneyId_;
        gchar* workingDirectory_;

    public:
        ~Parameters();
        
        LogLevel get_file_log_level() const { return fileLogLevel_; }
        void set_file_log_level( LogLevel const logLevel ) { fileLogLevel_ = logLevel; }

        LogLevel get_console_log_level() const { return consoleLogLevel_; }
        void set_console_log_level( LogLevel const logLevel ) { consoleLogLevel_ = logLevel; }

        const gchar* get_tourney_id() const { return tourneyId_; }
        void set_tourney_id( const gchar* tourneyId ) { g_free( tourneyId_ ); tourneyId_ = g_strdup( tourneyId ); }

        const gchar* get_working_directory() const { return workingDirectory_; }
        void set_working_directory( const gchar* workingDirectory ) { g_free( workingDirectory_ ); workingDirectory_ = g_strdup( workingDirectory_ ); }
   
        // returns FALSE if unable to parse command-line arguments
        // valid arguments are
        //      -t:<id>          tourney id
        //      -d:<path>        working directory (default is location of executable)
        //      -l:<LogLevel>    log level for logfile
        //      -c<:LogLevel>    log level for console
        // valid LogLevels are
        //      DEBUG, INFO, WARN, ERROR, FATAL
        gboolean parse_arguments( int const argc, char** const& argv );


        // converts string to LogLevel enum and back
        static LogLevel str_to_log_level( std::string const& logLevelStr );
        static std::string log_level_to_str( LogLevel const logLevel );

    private:
        void print_help() const;
        gchar* get_default_working_directory( const char* programName ) const; // caller owns string
};