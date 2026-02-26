#define _USE_MATH_DEFINES
#include <cmath>
#include <fstream>
#include <iostream>
#include <vector>

#include "../../shared/include/utils.hpp"


std::vector<Point> coneTriangles(const float radius, const float height,
    const size_t slices, const size_t stacks) {
    std::vector<Point> vertex;

    const float alfa = static_cast<float>(2 * M_PI / slices);
    const float stackHeight = height / stacks;

    const auto base_middle = Point{ 0, 0, 0 };

    for (size_t slice = 0; slice < slices; ++slice) {
        for (size_t stack = 0; stack < stacks; ++stack) {
            const float cRadius = radius - stack * radius / stacks;
            const float nRadius = radius - (stack + 1) * radius / stacks;

            const Point bottom_left =
                Point(cRadius * sinf(slice * alfa), stack * stackHeight,
                    cRadius * cosf(slice * alfa));
            const Point bottom_right =
                Point(cRadius * sinf((slice + 1) * alfa), stack * stackHeight,
                    cRadius * cosf((slice + 1) * alfa));
            const Point top_left =
                Point(nRadius * sinf(slice * alfa), (stack + 1) * stackHeight,
                    nRadius * cosf(slice * alfa));
            const Point top_right =
                Point(nRadius * sinf((slice + 1) * alfa), (stack + 1) * stackHeight,
                    nRadius * cosf((slice + 1) * alfa));

            vertex.push_back(top_left);
            vertex.push_back(bottom_left);
            vertex.push_back(bottom_right);

            vertex.push_back(top_left);
            vertex.push_back(bottom_right);
            vertex.push_back(top_right);
        }

        const Point base_bottom_left =
            Point(radius * sinf(static_cast<float>(slice) * alfa), 0,
                radius * cosf(static_cast<float>(slice) * alfa));
        const Point base_bottom_right =
            Point(radius * sinf(static_cast<float>(slice + 1) * alfa), 0,
                radius * cosf(static_cast<float>(slice + 1) * alfa));

        vertex.push_back(base_middle);
        vertex.push_back(base_bottom_right);
        vertex.push_back(base_bottom_left);
    }

    return vertex;
}

bool generateCone(float radius, float height, int slices, int stacks,
    const char* filepath) {
    std::vector<Point> triangles = coneTriangles(radius, height, slices, stacks);

    if (triangles.empty()) {
        std::cerr << "Error: Empty vector of triangles.\n";
        return false;
    }
    saveToFile(triangles, filepath);

    return true;
}