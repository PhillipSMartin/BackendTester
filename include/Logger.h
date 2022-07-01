#pragma once

#include "Parameters.h"

class Logger
{
    private:
        const char* logFileBase_ = "BackendTester";
        Parameters* pParms_;
        GtkTextBuffer* pConsoleTextBuffer_ = NULL;

    public:
        bool started_ = false;

        Logger(Parameters* pParms);

        void Debug(std::string const& message) const;
        void Info(std::string const& message) const;
        void Warn(std::string const& message) const;
        void Error(std::string const& message) const;
        void Fatal(std::string const& message) const;

        void set_text_buffer(GtkTextBuffer* const pConsoleTextBuffer) { pConsoleTextBuffer_ = pConsoleTextBuffer; }

    private:
        void PrintConsoleMessage(std::string const& message, Parameters::LogLevel const logLevel) const;
};