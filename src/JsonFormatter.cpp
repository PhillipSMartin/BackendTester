#include "JsonFormatter.h"
#include <iostream>
#include <iomanip>

namespace JsonFormatter
{
    std::string format( std::string const& json_text, Logger* pLogger )
    {
        try
        {
            return format( nlohmann::json::parse( json_text ), pLogger );
        }
        catch(const std::exception& e)
        {
            pLogger->Error( "Unable to parse json string " + json_text );
            return "";
        }       
     }
    
    std::string format( nlohmann::json const& json, Logger* pLogger )
    {
        try
        {
            std::ostringstream os;
            os << std::setw(4) << json << std::endl;
            return os.str();
        }
        catch(const std::exception& e)
        {
            gchar* _msg = g_strdup_printf( "Unable to print json object: %s", e.what() );
            pLogger->Error( _msg );
            g_free( _msg );
            return "";
        }       
     }
}
