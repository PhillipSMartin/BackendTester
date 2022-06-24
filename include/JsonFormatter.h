#pragma once

#include "nlohmann/json.hpp"
#include <memory>
#include <string>
#include <vector>

namespace JsonFormatter
{
 //   std::shared_ptr<std::vector<std::string>> format(std::string const& json);
   std::string format(std::string const& json);
}