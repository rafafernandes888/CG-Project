#define _USE_MATH_DEFINES
#include <fstream>
#include <iostream>
#include <vector>
#include <cmath>

#include "../../shared/include/utils.hpp"

static float snapZero(float v) {
    return (std::fabs(v) < 1e-6f) ? 0.0f : v;
}

std::vector<Vertex> sphereTriangles(float radius, int slices, int stacks) {
    std::vector<Vertex> verts;

    float stackStep = (float)M_PI / stacks;
    float sliceStep = 2.0f * (float)M_PI / slices;

    for (int i = 0; i < stacks; i++) {
        float phi1 = -(float)M_PI / 2.0f + i * stackStep;
        float phi2 = -(float)M_PI / 2.0f + (i + 1) * stackStep;

        for (int j = 0; j < slices; j++) {
            float theta1 = j * sliceStep;
            float theta2 = (j + 1) * sliceStep;

            float x1 = snapZero(radius * cosf(phi1) * sinf(theta1));
            float y1 = snapZero(radius * sinf(phi1));
            float z1 = snapZero(radius * cosf(phi1) * cosf(theta1));

            float x2 = snapZero(radius * cosf(phi1) * sinf(theta2));
            float y2 = snapZero(radius * sinf(phi1));
            float z2 = snapZero(radius * cosf(phi1) * cosf(theta2));

            float x3 = snapZero(radius * cosf(phi2) * sinf(theta1));
            float y3 = snapZero(radius * sinf(phi2));
            float z3 = snapZero(radius * cosf(phi2) * cosf(theta1));

            float x4 = snapZero(radius * cosf(phi2) * sinf(theta2));
            float y4 = snapZero(radius * sinf(phi2));
            float z4 = snapZero(radius * cosf(phi2) * cosf(theta2));

            // Normals (normalized position)
            float nx1 = snapZero(cosf(phi1)*sinf(theta1)), ny1 = snapZero(sinf(phi1)), nz1 = snapZero(cosf(phi1)*cosf(theta1));
            float nx2 = snapZero(cosf(phi1)*sinf(theta2)), ny2 = snapZero(sinf(phi1)), nz2 = snapZero(cosf(phi1)*cosf(theta2));
            float nx3 = snapZero(cosf(phi2)*sinf(theta1)), ny3 = snapZero(sinf(phi2)), nz3 = snapZero(cosf(phi2)*cosf(theta1));
            float nx4 = snapZero(cosf(phi2)*sinf(theta2)), ny4 = snapZero(sinf(phi2)), nz4 = snapZero(cosf(phi2)*cosf(theta2));

            // Texture coordinates
            float u1t = (float)j / slices;
            float u2t = (float)(j + 1) / slices;
            float v1t = (float)i / stacks;
            float v2t = (float)(i + 1) / stacks;

            verts.push_back(Vertex(x1,y1,z1, nx1,ny1,nz1, u1t,v1t));
            verts.push_back(Vertex(x2,y2,z2, nx2,ny2,nz2, u2t,v1t));
            verts.push_back(Vertex(x3,y3,z3, nx3,ny3,nz3, u1t,v2t));

            verts.push_back(Vertex(x2,y2,z2, nx2,ny2,nz2, u2t,v1t));
            verts.push_back(Vertex(x4,y4,z4, nx4,ny4,nz4, u2t,v2t));
            verts.push_back(Vertex(x3,y3,z3, nx3,ny3,nz3, u1t,v2t));
        }
    }

    return verts;
}

bool generateSphere(float radius, int slices, int stacks, const char* filepath) {
    std::vector<Vertex> triangles = sphereTriangles(radius, slices, stacks);
    if (triangles.empty()) {
        std::cerr << "Error: Empty vector of triangles.\n";
        return false;
    }
    saveToFile(triangles, filepath);
    return true;
}