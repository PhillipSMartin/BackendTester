#pragma once

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "Logger.h"
#include "nlohmann/json.hpp"

class TemplateMap
{
    using TemplateKey = std::string;
    using TemplateValue = std::pair<std::shared_ptr<nlohmann::json>, std::string>;
    using TemplateItem = std::pair<TemplateKey, TemplateValue>;
 
    private:
        Logger* pLogger_;
        std::string fileName_;
        std::map<TemplateKey, TemplateValue>  map_;

    public:
        TemplateMap(Logger* pLogger);
        gboolean import_file( std::string const& fileName );
        gboolean export_file() const;
        std::vector<TemplateKey> get_keys() const;
        std::shared_ptr<nlohmann::json> get_template( TemplateKey const& key ) const;
        std::string get_help( TemplateKey const& key ) const;

    private:  
        std::string get_line_from_map_item( TemplateItem const& mapItem ) const;       

        // functions below return negative int on an error, 0 on success    
        int read_file( GIOChannel* channel );        
        int write_file( GIOChannel* channel ) const;
        int add_item_to_map( gchar* const line );       
        int handle_error( std::string const& prefix, GError* error ) const;
};