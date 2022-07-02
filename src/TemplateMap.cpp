#include <algorithm>
#include <sstream>

#include "TemplateMap.h"

TemplateMap::TemplateMap(Logger* pLogger) : pLogger_(pLogger) 
{
}

// returns TRUE on successful import
int TemplateMap::import_file( std::string const& fileName )
{
    fileName_ = fileName;
    GError* _error = NULL;

    GIOChannel* _channel = g_io_channel_new_file( fileName_.c_str(), "r", &_error );
    if ( 0 > handle_error( "Unable to open file " + fileName_, _error ) )
    {
        return -1;
    }

   int _rc = read_file( _channel );

    g_io_channel_shutdown( _channel, TRUE, &_error );
    if ( 0 > handle_error(  "Unable to close file " + fileName_, _error ) )
    {
        return -1;
    }

    return _rc;
}

int TemplateMap::read_file( GIOChannel* channel )
{
    gchar* _line = NULL;
    GError* _error = NULL;
    int _rc = 0;

    while ( G_IO_STATUS_EOF != g_io_channel_read_line( channel, &_line, NULL, NULL, &_error ) )
    {
        if ( 0 > handle_error(  "Unable to read file " + fileName_, _error ))
        {
            _rc = -1;
            break;
        }
 
        if ( 0 > add_template_to_map( _line ) )
        {
            handle_error( "Format error in " + fileName_, _line );
            _rc = -1;
            break;
        }

        g_free( _line );
        _line = NULL;
    } 

    g_free( _line );
    return _rc;
}

int TemplateMap::export_file() const
{
    GError* _error = NULL;

    GIOChannel* _channel = g_io_channel_new_file( fileName_.c_str(), "w", &_error );
    if ( 0 > handle_error( "Unable to open file " + fileName_, _error ) )
    {
        return -1;
    }

    int _rc = write_file( _channel );

    g_io_channel_shutdown( _channel, TRUE, &_error );
    if ( 0 > handle_error(  "Unable to close file " + fileName_, _error ) )
    {
        return -1;
    } 

    return _rc;
}

int TemplateMap::write_file( GIOChannel* channel ) const
{
    GError* _error = NULL;

    for ( TemplateItem _item : map_ )
    {
        std::string _line = get_line_from_map_item( _item );
        g_io_channel_write_chars( channel, _line.c_str(), -1, NULL, &_error );
        if ( 0 > handle_error(  "Unable to write to file " + fileName_, _error ) )
        {
            return -1;
        }
    }

    return 0;
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

// returns -1 if there is a syntax error in 'line'
int TemplateMap::add_template_to_map( gchar* const line )
{
    TemplateKey _key;
    std::string _template;
    std::string _help;
    std::stringstream _sstream(line);

    if ( !std::getline( _sstream, _key, '|' ) )
        return -1;

    if ( !std::getline( _sstream, _template, '|' ) ) 
        return -1;

    std::getline( _sstream, _help, '|' );
    std::replace( _help.begin(), _help.end(), ';', '\n' );

    map_[_key] = TemplateValue( _template, _help );   
    return 0;
}

std::string TemplateMap::get_line_from_map_item( TemplateItem const& mapItem ) const
{
    std::string _help { mapItem.second.second };
    std::replace( _help.begin(), _help.end(), '\n', ';' );

    gchar* _line = g_strjoin( "|", mapItem.first.c_str(), mapItem.second.first.c_str(), _help.c_str(), NULL );
    std::string _lineStr{_line};
    g_free( _line );
    return _lineStr;
}

// returns -1 to indicate an error
int TemplateMap::handle_error( std::string const& prefix, std::string const& errorMsg ) const
{
    pLogger_->Error( prefix + ": " + errorMsg );
    return -1;
}

// returns -1 if there was an error, 0 if not
int TemplateMap::handle_error( std::string const& prefix, GError* error ) const
{
    if ( error != NULL )
    {
        handle_error( prefix, error->message );
        g_clear_error( &error );
        return -1;
    }

    return 0;
}