#ifndef UTILS_HPP
#define UTILS_HPP

#include <vector>
#include <string>
#include "point.hpp"

void saveToFile(const std::vector<Point>& points, const std::string& filepath);
std::vector<Point> parse3Dfile(const std::string& filename);

void saveToFile(const std::vector<Vertex>& vertices, const std::string& filepath);
std::vector<Vertex> parse3DfileV2(const std::string& filename);

#endif