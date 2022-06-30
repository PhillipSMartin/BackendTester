#pragma once

#include <gtk/gtk.h>
#include <memory>

#include "Parameters.h"

class Logger
{
    private:
        const char* logFileBase_ = "BackendTester";
        std::shared_ptr<Parameters> pParms_;
        GtkTextBuffer* pTextBuffer_ = NULL;

    public:
        bool started_ = false;

        Logger(std::shared_ptr<Parameters> pParms);

        void Debug(std::string const& message) const;
        void Info(std::string const& message) const;
        void Warn(std::string const& message) const;
        void Error(std::string const& message) const;
        void Fatal(std::string const& message) const;

        void set_text_buffer(GtkTextBuffer* textBuffer) { pTextBuffer_ = textBuffer; }

    private:
        void PrintConsoleMessage(std::string const& message, Parameters::LogLevel const logLevel) const;
};