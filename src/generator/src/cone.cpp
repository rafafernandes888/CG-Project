#include <vector>
#include <cmath>
#include "../../shared/include/utils.hpp"
#include <iostream>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

std::vector<Point> coneTriangles(float radius, float height, int slices, int stacks) {
    std::vector<Point> points;
    float sliceStep = 2.0f * M_PI / slices;
    float stackStep = height / stacks;
    float radiusStep = radius / stacks;

    // Corpo lateral
    for (int i = 0; i < stacks; i++) {
        float y1 = i * stackStep;
        float y2 = (i + 1) * stackStep;
        float r1 = radius - i * radiusStep;
        float r2 = radius - (i + 1) * radiusStep;

        for (int j = 0; j < slices; j++) {
            float theta1 = j * sliceStep;
            float theta2 = (j + 1) * sliceStep;

            Point p1(r1 * sin(theta1), y1, r1 * cos(theta1));
            Point p2(r1 * sin(theta2), y1, r1 * cos(theta2));
            Point p3(r2 * sin(theta1), y2, r2 * cos(theta1));
            Point p4(r2 * sin(theta2), y2, r2 * cos(theta2));

            points.push_back(p1);
            points.push_back(p2);
            points.push_back(p3);

            points.push_back(p2);
            points.push_back(p4);
            points.push_back(p3);
        }
    }

    // Base (círculo no Y=0)
    for (int j = 0; j < slices; j++) {
        float theta1 = j * sliceStep;
        float theta2 = (j + 1) * sliceStep;

        Point center(0.0f, 0.0f, 0.0f);
        Point p1(radius * sin(theta1), 0.0f, radius * cos(theta1));
        Point p2(radius * sin(theta2), 0.0f, radius * cos(theta2));

        // sentido inverso para a normal apontar para baixo
        points.push_back(center);
        points.push_back(p2);
        points.push_back(p1);
    }

    return points;
}

bool generateCone(float radius, float height, int slices, int stacks, const char* filepath) {
    std::vector<Point> triangles = coneTriangles(radius, height, slices, stacks);

    if (triangles.empty()) {
        std::cerr << "Error: Empty vector of triangles.\n";
        return false;
    }

    saveToFile(triangles, filepath);
    return true;
}