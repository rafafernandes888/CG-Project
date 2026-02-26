#ifndef UTILS_HPP
#define UTILS_HPP

#include <vector>
#include <string>
#include "point.hpp"

void saveToFile(const std::vector<Point>& points, const std::string& filepath);

std::vector<Point> parse3Dfile(const std::string& filename);

#endif