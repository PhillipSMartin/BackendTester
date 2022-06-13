#include "Logger.h"
#include "utility/Logging.h"

#include <iostream>

Logger::Logger(std::shared_ptr<Parameters> const& pParms) : pParms_(pParms)
{
    if (pParms->get_fileLogLevel() < Parameters::LogLevel::NONE)
    {
        if (started_ = Utility::Log::set_unique_log( logFileBase_ ))
        {
            started_ = Utility::Log::set_log_level( Parameters::LogLevelToStr(pParms->get_fileLogLevel()) ); 
        }
        if (!started_)
        {
            pParms_->set_fileLogLevel(Parameters::LogLevel::NONE );
            std::cout << "WARN: Unable to open logfile" << std:: endl;
        }
    }

    Info("Logfile log level set to " + Parameters::LogLevelToStr(pParms->get_fileLogLevel()));
    Info("Console log level set to " + Parameters::LogLevelToStr(pParms->get_consoleLogLevel()));
}

void Logger::PrintConsoleMessage(std::string const& message, Parameters::LogLevel const logLevel) const
{
   if (pParms_->get_consoleLogLevel() <= logLevel)
    {
        std::cout << Parameters::LogLevelToStr (logLevel) << ": " << message << std::endl;
    }
}

void Logger::Debug(std::string const& message) const
{
    if (pParms_->get_fileLogLevel() <= Parameters::LogLevel::DEBUG)
    {
        LOG_STRM_DEBUG(nullptr, message);
    }
    PrintConsoleMessage(message, Parameters::LogLevel::DEBUG);
}

void Logger::Info(std::string const& message) const
{
    if (pParms_->get_fileLogLevel() <= Parameters::LogLevel::INFO)
    {
        LOG_STRM_INFO(nullptr, message);
    }
    PrintConsoleMessage(message, Parameters::LogLevel::INFO);
}

void Logger::Warn(std::string const& message) const
{
    if (pParms_->get_fileLogLevel() <= Parameters::LogLevel::WARN)
    {
        LOG_STRM_WARN(nullptr, message);
    }
    PrintConsoleMessage(message, Parameters::LogLevel::WARN);
}

void Logger::Error(std::string const& message) const
{
    if (pParms_->get_fileLogLevel() <= Parameters::LogLevel::ERROR)
    {
        LOG_STRM_ERROR(nullptr, message);
    }
    PrintConsoleMessage(message, Parameters::LogLevel::ERROR);
 }
void Logger::Fatal(std::string const& message) const
{
    if (pParms_->get_fileLogLevel() <= Parameters::LogLevel::FATAL)
    {
        LOG_STRM_FATAL(nullptr, message);
    }
    PrintConsoleMessage(message, Parameters::LogLevel::FATAL);
}