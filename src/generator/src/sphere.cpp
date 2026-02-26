#define _USE_MATH_DEFINES
#include <fstream>
#include <iostream>
#include <vector>
#include <cmath>

#include "../../shared/include/utils.hpp"


static float snapZero(float v) {
    return (std::fabs(v) < 1e-6f) ? 0.0f : v;
}

std::vector<Point> sphereTriangles(float radius, int slices, int stacks) {
    std::vector<Point> points;

    float stackStep = (float)M_PI / stacks;
    float sliceStep = 2.0f * (float)M_PI / slices;

    for (int i = 0; i < stacks; i++) {
        float phi1 = -(float)M_PI / 2.0f + i * stackStep;
        float phi2 = -(float)M_PI / 2.0f + (i + 1) * stackStep;

        for (int j = 0; j < slices; j++) {
            float theta1 = j * sliceStep;
            float theta2 = (j + 1) * sliceStep;

            Point p1(snapZero(radius * cos(phi1) * sin(theta1)),
                     snapZero(radius * sin(phi1)),
                     snapZero(radius * cos(phi1) * cos(theta1)));

            Point p2(snapZero(radius * cos(phi1) * sin(theta2)),
                     snapZero(radius * sin(phi1)),
                     snapZero(radius * cos(phi1) * cos(theta2)));

            Point p3(snapZero(radius * cos(phi2) * sin(theta1)),
                     snapZero(radius * sin(phi2)),
                     snapZero(radius * cos(phi2) * cos(theta1)));

            Point p4(snapZero(radius * cos(phi2) * sin(theta2)),
                     snapZero(radius * sin(phi2)),
                     snapZero(radius * cos(phi2) * cos(theta2)));

            points.push_back(p1);
            points.push_back(p2);
            points.push_back(p3);

            points.push_back(p2);
            points.push_back(p4);
            points.push_back(p3);
        }
    }

    return points;
}

bool generateSphere(float radius, int slices, int stacks, const char* filepath) {
    std::vector<Point> triangles = sphereTriangles(radius, slices, stacks);

    if (triangles.empty()) {
        std::cerr << "Error: Empty vector of triangles.\n";
        return false;
    }

    saveToFile(triangles, filepath);
    return true;
}