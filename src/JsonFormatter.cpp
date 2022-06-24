#include "JsonFormatter.h"
#include <iostream>
#include <iomanip>

namespace JsonFormatter
{
    using json = nlohmann::json;

    std::string format(std::string const& json_text)
    {
        json j = json::parse(json_text);
        std::ostringstream os;
        os << std::setw(4) << j << std::endl;
        return os.str();
    }

#if 0
    {
        #define BUFFERLEN 512
        #define INDENT 2

        auto _outputVector = std::make_shared<std::vector<std::string>>();
        char _buffer[BUFFERLEN];
        int _bufferIndex = 0;
        int _stringIndex = 0;
        int _indent = 0;
        bool _newLine = false;
        bool _inBrackets = false;
     
        for (const char& c : json)
        {
            _stringIndex++;
            switch (c)
            {
                case '{':
                    if (_outputVector->size() > 0)
                    {
                        _newLine = true;
                    }
                    _inBrackets = false;
                    break;
                case '}':
                    _newLine = true;
                    _indent -= INDENT;
                    break;
                case '"':
                    continue;
                case '[':
                    _inBrackets = true;
                    if (json[_stringIndex] == '[' || json[_stringIndex] == '{')
                    {
                        _newLine = true;
                    }
                    break;
                case ']':
                    _inBrackets = false;
                    if (json[_stringIndex-2] == ']' || json[_stringIndex-2] == '}')
                    {
                        _newLine = true;
                        _indent -= INDENT;                      
                    }
                    break;
                case ',':
                    if (!_inBrackets)
                    {
                        _newLine = true;
                        continue;
                    }
                    break;
                default:
                    break;
            }

            if (_newLine)
            {
                _buffer[_bufferIndex] = '\0';
                _outputVector->emplace_back(_buffer);

                _bufferIndex = 0;
                for (int n = 0; n < _indent; n++)
                {
                    _buffer[_bufferIndex++] = ' ';
                }

                _newLine = false;
             }

            _buffer[_bufferIndex++] = c;

            switch (c)
            {
                case '{':
                    _indent += INDENT;
                    _newLine = true;
                    break;
                case '[':
                    if (json[_stringIndex] == '[' || json[_stringIndex] == '{')
                    {
                        _indent += INDENT;
                       _newLine = true;
                    }
                    break;
               case ':':
                    _buffer[_bufferIndex++] = ' ';
                    break; 
                default:
                    break;
            }
        }

        if (_bufferIndex > 0)
        {
            _buffer[_bufferIndex] = '\0';
            _outputVector->emplace_back(_buffer);
        }
        return _outputVector;
    }
#endif
}
