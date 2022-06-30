#pragma once

#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "Logger.h"

class TemplateMap
{
    private:
        std::shared_ptr<Logger> pLogger_;
        std::map<std::string, std::pair<std::string, std::string>>  map_;

    public:
        TemplateMap(std::shared_ptr<Logger> pLogger);
        gboolean import_file( std::string const& fileName );
        std::vector<std::string> get_keys() const;
        std::string get_template( std::string const& key ) const;
        std::string get_help( std::string const& key ) const;

    private:       
        gboolean handle_error( std::string const& prefix, GError* error ) const;
        gboolean handle_error( std::string const& prefix, std::string const& error ) const;
        gboolean add_template_to_map( gchar* const line );
};