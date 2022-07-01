#pragma once

#include <unordered_map>
#include <string>
#include <utility>
#include <vector>

#include "Logger.h"

class TemplateMap
{
    private:
        Logger* pLogger_;
        std::unordered_map<std::string, std::pair<std::string, std::string>>  map_;

    public:
        TemplateMap(Logger* pLogger);
        gboolean import_file( std::string const& fileName );
        std::vector<std::string> get_keys() const;
        std::string get_template( std::string const& key ) const;
        std::string get_help( std::string const& key ) const;

    private:       
        gboolean handle_error( std::string const& prefix, GError* error ) const;
        gboolean handle_error( std::string const& prefix, std::string const& error ) const;
        gboolean add_template_to_map( gchar* const line );
};