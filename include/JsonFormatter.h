#pragma once

#include <string>

#include "Logger.h"
#include "nlohmann/json.hpp"

namespace JsonFormatter
{
   std::string format( std::string const&, std::shared_ptr<Logger> );
   std::string format( nlohmann::json const&, std::shared_ptr<Logger> );
}