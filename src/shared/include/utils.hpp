#ifndef UTILS_HPP
#define UTILS_HPP

#include <vector>
#include <string>
#include "point.hpp" // Para que as funções reconheçam a struct Point

// Grava os pontos gerados num ficheiro .3d (usado pelo Generator)
void saveToFile(const std::vector<Point>& points, std::string filepath);

// Lê os pontos de um ficheiro .3d para a memória (usado pelo Engine)
std::vector<Point> parse3Dfile(std::string filename);

#endif