#include "Logger.h"
#include "utility/Logging.h"

#include <iostream>

Logger::Logger(std::shared_ptr<Parameters> pParms) : pParms_(pParms)
{
    if (pParms->get_file_log_level() < Parameters::LogLevel::NONE)
    {
        if (started_ = Utility::Log::set_unique_log( logFileBase_ ))
        {
            started_ = Utility::Log::set_log_level( Parameters::LogLevelToStr(pParms->get_file_log_level()) ); 
        }
        if (!started_)
        {
            pParms_->set_file_log_level(Parameters::LogLevel::NONE );
            std::cout << "WARN: Unable to open logfile" << std:: endl;
        }
    }

    Info("Logfile log level set to " + Parameters::LogLevelToStr(pParms->get_file_log_level()));
    Info("Console log level set to " + Parameters::LogLevelToStr(pParms->get_console_log_level()));
    Info("Tourney id set to " + pParms->get_tourney_id());
}

void Logger::PrintConsoleMessage( std::string const& message, Parameters::LogLevel const logLevel ) const
{
   if ( pParms_->get_console_log_level() <= logLevel )
    {
        if ( pTextBuffer_ != NULL )
        {
            GtkTextIter _iter;
            gchar* _msg = g_strdup_printf( "%s: %s\n",  Parameters::LogLevelToStr( logLevel ).c_str(), message.c_str() );

            gtk_text_buffer_get_end_iter( pTextBuffer_, &_iter );
            gtk_text_buffer_insert( pTextBuffer_, &_iter, _msg, -1 );

            g_free( _msg );
         }
        else
        {
            std::cout << Parameters::LogLevelToStr( logLevel ) << ": " << message << std::endl;
        }
    }
}

void Logger::Debug(std::string const& message) const
{
    if (pParms_->get_file_log_level() <= Parameters::LogLevel::DEBUG)
    {
        LOG_STRM_DEBUG(nullptr, message);
    }
    PrintConsoleMessage(message, Parameters::LogLevel::DEBUG);
}

void Logger::Info(std::string const& message) const
{
    if (pParms_->get_file_log_level() <= Parameters::LogLevel::INFO)
    {
        LOG_STRM_INFO(nullptr, message);
    }
    PrintConsoleMessage(message, Parameters::LogLevel::INFO);
}

void Logger::Warn(std::string const& message) const
{
    if (pParms_->get_file_log_level() <= Parameters::LogLevel::WARN)
    {
        LOG_STRM_WARN(nullptr, message);
    }
    PrintConsoleMessage(message, Parameters::LogLevel::WARN);
}

void Logger::Error(std::string const& message) const
{
    if (pParms_->get_file_log_level() <= Parameters::LogLevel::ERROR)
    {
        LOG_STRM_ERROR(nullptr, message);
    }
    PrintConsoleMessage(message, Parameters::LogLevel::ERROR);
 }
void Logger::Fatal(std::string const& message) const
{
    if (pParms_->get_file_log_level() <= Parameters::LogLevel::FATAL)
    {
        LOG_STRM_FATAL(nullptr, message);
    }
    PrintConsoleMessage(message, Parameters::LogLevel::FATAL);
}