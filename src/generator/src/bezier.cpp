#define _USE_MATH_DEFINES
#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <string>

#include "../../shared/include/utils.hpp"

// Bernstein basis polynomial of degree 3
static float bernstein(int i, float t) {
    switch (i) {
        case 0: return (1 - t) * (1 - t) * (1 - t);
        case 1: return 3 * t * (1 - t) * (1 - t);
        case 2: return 3 * t * t * (1 - t);
        case 3: return t * t * t;
        default: return 0;
    }
}

// Evaluate a point on a Bezier patch given 16 control points and (u,v) parameters
static Point bezierPatchPoint(const std::vector<Point>& cp, float u, float v) {
    Point p(0, 0, 0);
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            float b = bernstein(i, u) * bernstein(j, v);
            p.x += cp[i * 4 + j].x * b;
            p.y += cp[i * 4 + j].y * b;
            p.z += cp[i * 4 + j].z * b;
        }
    }
    return p;
}

// Parse a line of comma-separated indices
static std::vector<size_t> parseIndices(const std::string& line) {
    std::vector<size_t> indices;
    std::stringstream ss(line);
    std::string token;
    while (std::getline(ss, token, ',')) {
        // trim whitespace
        size_t start = token.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) continue;
        indices.push_back(std::stoul(token.substr(start)));
    }
    return indices;
}

bool generateBezier(const char* patchFile, int tessellation, const char* outputFile) {
    std::ifstream file(patchFile);
    if (!file.is_open()) {
        std::cerr << "Error: cannot open patch file: " << patchFile << "\n";
        return false;
    }

    // Read number of patches
    std::string line;
    std::getline(file, line);
    int numPatches = std::stoi(line);

    // Read patch indices (each patch has 16 indices)
    std::vector<std::vector<size_t>> patches(numPatches);
    for (int i = 0; i < numPatches; ++i) {
        std::getline(file, line);
        patches[i] = parseIndices(line);
        if (patches[i].size() != 16) {
            std::cerr << "Error: patch " << i << " has " << patches[i].size()
                      << " indices (expected 16)\n";
            return false;
        }
    }

    // Read number of control points
    std::getline(file, line);
    int numControlPoints = std::stoi(line);

    // Read control points
    std::vector<Point> controlPoints;
    for (int i = 0; i < numControlPoints; ++i) {
        std::getline(file, line);
        std::stringstream ss(line);
        float x, y, z;
        char comma;
        ss >> x >> comma >> y >> comma >> z;
        controlPoints.push_back(Point(x, y, z));
    }
    file.close();

    // Generate triangles for each patch
    std::vector<Point> triangles;

    for (int p = 0; p < numPatches; ++p) {
        // Get the 16 control points for this patch
        std::vector<Point> patchCP(16);
        for (int k = 0; k < 16; ++k) {
            patchCP[k] = controlPoints[patches[p][k]];
        }

        // Tessellate
        for (int ui = 0; ui < tessellation; ++ui) {
            for (int vi = 0; vi < tessellation; ++vi) {
                float u1 = (float)ui / tessellation;
                float v1 = (float)vi / tessellation;
                float u2 = (float)(ui + 1) / tessellation;
                float v2 = (float)(vi + 1) / tessellation;

                Point p1 = bezierPatchPoint(patchCP, u1, v1);
                Point p2 = bezierPatchPoint(patchCP, u2, v1);
                Point p3 = bezierPatchPoint(patchCP, u1, v2);
                Point p4 = bezierPatchPoint(patchCP, u2, v2);

                // Triangle 1: p1, p3, p2
                triangles.push_back(p1);
                triangles.push_back(p3);
                triangles.push_back(p2);

                // Triangle 2: p2, p3, p4
                triangles.push_back(p2);
                triangles.push_back(p3);
                triangles.push_back(p4);
            }
        }
    }

    if (triangles.empty()) {
        std::cerr << "Error: no triangles generated from patches\n";
        return false;
    }

    saveToFile(triangles, outputFile);
    return true;
}
