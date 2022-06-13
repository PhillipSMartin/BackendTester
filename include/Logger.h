#pragma once

#include "Parameters.h"

#include <memory>

class Logger
{
    private:
        const char* logFileBase_ = "BackendTester";
        std::shared_ptr<Parameters> pParms_;

    public:
        bool started_ = false;

        Logger(std::shared_ptr<Parameters> const& pParms);

        void Debug(std::string const& message) const;
        void Info(std::string const& message) const;
        void Warn(std::string const& message) const;
        void Error(std::string const& message) const;
        void Fatal(std::string const& message) const;

    private:
        void PrintConsoleMessage(std::string const& message, Parameters::LogLevel const logLevel) const;
};