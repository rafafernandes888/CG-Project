#ifndef DRAW_HPP
#define DRAW_HPP

#include <math.h>

#include <vector>

#include "group.hpp"
#include "../../shared/include/utils.hpp"


void drawTriangles(const std::vector<Point>& points);

void drawGroup(const Group& group);


#endif  // DRAW_HPP