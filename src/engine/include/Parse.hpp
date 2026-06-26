#ifndef PARSE_HPP
#define PARSE_HPP

#include <string>
#include "../../../libs/rapidxml-1.13/rapidxml.hpp"
#include "Configuration.hpp"
#include "group.hpp"

Configuration parseConfig(const std::string& filename);
Group parseGroup(rapidxml::xml_node<>* groupNode, const std::string& modelsDir, const std::string& texturesDir);

#endif