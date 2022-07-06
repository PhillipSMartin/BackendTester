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

        // for g_strdup_printf output
        // we take owndership of the string
        void Debug(gchar* message) const;
        void Info(gchar* message) const;
        void Warn(gchar* message) const;
        void Error(gchar* message) const;
        void Fatal(gchar* message) const;

        // alternative functions for literals
        void DebugStr(std::string const& message) const;
        void InfoStr(std::string const& message) const;
        void WarnStr(std::string const& message) const;
        void ErrorStr(std::string const& message) const;
        void FatalStr(std::string const& message) const;


        void set_text_buffer(GtkTextBuffer* const pConsoleTextBuffer) { pConsoleTextBuffer_ = pConsoleTextBuffer; }  // TODO - move buffer to ConsoleWindow

    private:
        void PrintConsoleMessage(const gchar* message, Parameters::LogLevel const logLevel) const;
};