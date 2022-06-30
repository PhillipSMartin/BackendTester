#include "JsonFormatter.h"
#include <iostream>
#include <iomanip>

namespace JsonFormatter
{
    using json = nlohmann::json;

    std::string format(std::string const& json_text)
    {
        return format(json::parse(json_text));
    }
    
    std::string format(json const& j)
    {
        std::ostringstream os;
        os << std::setw(4) << j << std::endl;
        return os.str();
    }
}
