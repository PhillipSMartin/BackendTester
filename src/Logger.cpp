#include <iostream>

#include "Logger.h"
#include "utility/Logging.h"

Logger::Logger(Parameters* pParms) : pParms_(pParms)
{
    if ( pParms_->get_file_log_level() < Parameters::LogLevel::NONE )
    {
        if ( started_ = Utility::Log::set_unique_log( logFileBase_ ) )
        {
            started_ = Utility::Log::set_log_level( Parameters::log_level_to_str( pParms_->get_file_log_level() ) ); 
        }
        if ( !started_ )
        {
            pParms_->set_file_log_level( Parameters::LogLevel::NONE );
            std::cout << "WARN: Unable to open logfile" << std:: endl;
        }
    }

    Info( g_strdup_printf( "Logfile log level set to %s", Parameters::log_level_to_str( pParms_->get_file_log_level() ) ) );
    Info( g_strdup_printf( "Console log level set to %s", Parameters::log_level_to_str( pParms_->get_console_log_level() ) ) );
    Info( g_strdup_printf( "Tourney id set to %s", pParms_->get_tourney_id() ) );
}

void Logger::PrintConsoleMessage( const gchar* message, Parameters::LogLevel const logLevel ) const
{
   if ( pParms_->get_console_log_level() <= logLevel )
    {
        gchar* _msg = g_strdup_printf( "%s: %s\n",  Parameters::log_level_to_str( logLevel ), message );

        if ( pConsoleTextBuffer_ != NULL )
        {
            GtkTextIter _iter;
            gtk_text_buffer_get_end_iter( pConsoleTextBuffer_, &_iter );
            gtk_text_buffer_insert( pConsoleTextBuffer_, &_iter, _msg, -1 );
         }
        else
        {
            std::cout << _msg;
        }

        g_free( _msg );
    }
}

void Logger::Debug( gchar* message ) const
{
    DebugStr( message );
    g_free( message );
}

void Logger::Info( gchar* message ) const
{
    InfoStr( message );
    g_free( message );
}

void Logger::Warn( gchar* message ) const
{
    WarnStr( message );
    g_free( message );
}

void Logger::Error( gchar* message ) const
{
    ErrorStr( message );
    g_free( message );
}

void Logger::Fatal( gchar* message ) const
{
    FatalStr( message );
    g_free( message );
}

void Logger::DebugStr(std::string const& message) const
{
    if ( pParms_->get_file_log_level() <= Parameters::LogLevel::DEBUG )
    {
        LOG_STRM_DEBUG( nullptr, message );
    }
    PrintConsoleMessage( message.c_str(), Parameters::LogLevel::DEBUG );
}

void Logger::InfoStr(std::string const& message) const
{
    if ( pParms_->get_file_log_level() <= Parameters::LogLevel::INFO )
    {
        LOG_STRM_INFO( nullptr, message );
    }
    PrintConsoleMessage( message.c_str(), Parameters::LogLevel::INFO );
}

void Logger::WarnStr(std::string const& message) const
{
    if ( pParms_->get_file_log_level() <= Parameters::LogLevel::WARN )
    {
        LOG_STRM_WARN( nullptr, message );
    }
    PrintConsoleMessage( message.c_str(), Parameters::LogLevel::WARN );
}

void Logger::ErrorStr(std::string const& message) const
{
    if ( pParms_->get_file_log_level() <= Parameters::LogLevel::ERROR )
    {
        LOG_STRM_ERROR( nullptr, message );
    }
    PrintConsoleMessage( message.c_str(), Parameters::LogLevel::ERROR );
}

void Logger::FatalStr(std::string const& message) const
{
     if ( pParms_->get_file_log_level() <= Parameters::LogLevel::FATAL )
    {
        LOG_STRM_FATAL( nullptr, message );
    }
    PrintConsoleMessage( message.c_str(), Parameters::LogLevel::FATAL ); 
}
