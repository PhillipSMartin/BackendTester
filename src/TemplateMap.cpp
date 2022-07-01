#include <algorithm>
#include <sstream>

#include "TemplateMap.h"

TemplateMap::TemplateMap(Logger* pLogger) : pLogger_(pLogger) 
{
}

// returns TRUE on successful import
gboolean TemplateMap::import_file( std::string const& fileName )
{
    GError* error = NULL;
    GIOChannel* _channel = g_io_channel_new_file( fileName.c_str(), "r", &error );
    if ( handle_error( "Unable to open file " + fileName, error ) )
        return FALSE;

    gchar* _line;
    GIOStatus _status = G_IO_STATUS_NORMAL;
    while ( G_IO_STATUS_NORMAL ==  ( _status = g_io_channel_read_line( _channel, &_line, NULL, NULL, &error ) ) )
    {
        if ( !add_template_to_map( _line ) )
        {
            handle_error(  "Format error in " + fileName, _line );
            break;
        }
    } 
    handle_error(  "Unable to read file " + fileName, error );
     
    g_io_channel_shutdown( _channel, TRUE, &error );
    handle_error(  "Unable to close file " + fileName, error );
    g_free( _line );

    return _status == G_IO_STATUS_EOF;
}

std::vector<std::string> TemplateMap::get_keys() const
{
    std::vector<std::string> _keys;
    for ( auto _item : map_ )
    {
        _keys.push_back( _item.first );
    }
    return _keys;
}

std::string TemplateMap::get_template( std::string const& key ) const
{
    auto it = map_.find( key );
    if ( map_.end() != it )
    {
        return it->second.first;
    }
    else
    {
        return "";
    }
}

std::string TemplateMap::get_help( std::string const& key ) const
{
    auto it = map_.find( key );
    if ( map_.end() != it )
    {
        return it->second.second;
    }
    else
    {
        return "";
    }
}

// returns FALSE if there is a syntax error in 'line'
gboolean TemplateMap::add_template_to_map( gchar* const line )
{
    std::string _key, _template, _help;
    std::stringstream _sstream(line);

    if ( !std::getline( _sstream, _key, '|' ) )
        return FALSE;

    if ( !std::getline( _sstream, _template, '|' ) ) 
        return FALSE;

    std::getline( _sstream, _help, '|' );
    std::replace( _help.begin(), _help.end(), ';', '\n' );

    map_[_key] = std::pair<std::string, std::string>( _template, _help );   
    return TRUE;
}

// returns TRUE if there was an error
gboolean TemplateMap::handle_error( std::string const& prefix, std::string const& error ) const
{
    pLogger_->Error( prefix + ": " + error );
    return TRUE;
}

gboolean TemplateMap::handle_error( std::string const& prefix, GError* error ) const
{
    if ( error != NULL )
    {
        handle_error( prefix, error->message );
        g_clear_error( &error );
        return TRUE;
    }

    return FALSE;
}