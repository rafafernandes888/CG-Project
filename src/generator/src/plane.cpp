#include <fstream>
#include <iostream>
#include <vector>

#include "../../shared/include/utils.hpp"

std::vector<Vertex> planeTriangles(float length, int divisions) {
    float half = length / 2.0f;
    float step = length / divisions;

    std::vector<Vertex> verts;

    for (int i = 0; i < divisions; i++) {
        for (int j = 0; j < divisions; j++) {
            float x1 = -half + i * step;
            float z1 = -half + j * step;
            float x2 = x1 + step;
            float z2 = z1 + step;

            float u1 = (float)i / divisions;
            float v1 = (float)j / divisions;
            float u2 = (float)(i + 1) / divisions;
            float v2 = (float)(j + 1) / divisions;

            verts.push_back(Vertex(x1, 0, z1,  0, 1, 0,  u1, v1));
            verts.push_back(Vertex(x1, 0, z2,  0, 1, 0,  u1, v2));
            verts.push_back(Vertex(x2, 0, z2,  0, 1, 0,  u2, v2));

            verts.push_back(Vertex(x1, 0, z1,  0, 1, 0,  u1, v1));
            verts.push_back(Vertex(x2, 0, z2,  0, 1, 0,  u2, v2));
            verts.push_back(Vertex(x2, 0, z1,  0, 1, 0,  u2, v1));
        }
    }

    return verts;
}

bool generatePlane(float length, int divisions, const char* filepath) {
    std::vector<Vertex> triangles = planeTriangles(length, divisions);
    if (triangles.empty()) {
        std::cerr << "Error: Empty vector of triangles.\n";
        return false;
    }
    saveToFile(triangles, filepath);
    return true;
}