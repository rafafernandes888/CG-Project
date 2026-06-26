#include <fstream>
#include <iostream>
#include <vector>

#include "../../shared/include/utils.hpp"

std::vector<Vertex> boxTriangles(float length, int divisions) {
    float h = length / 2.0f;
    float step = length / divisions;

    std::vector<Vertex> verts;

    for (int i = 0; i < divisions; ++i) {
        for (int j = 0; j < divisions; ++j) {
            float a1 = -h + i * step;
            float b1 = -h + j * step;
            float a2 = a1 + step;
            float b2 = b1 + step;

            float u1 = (float)i / divisions;
            float v1 = (float)j / divisions;
            float u2 = (float)(i+1) / divisions;
            float v2 = (float)(j+1) / divisions;

            // Front (z = +h)
            verts.push_back(Vertex(a1,b1,h, 0,0,1, u1,v1));
            verts.push_back(Vertex(a2,b2,h, 0,0,1, u2,v2));
            verts.push_back(Vertex(a1,b2,h, 0,0,1, u1,v2));
            verts.push_back(Vertex(a1,b1,h, 0,0,1, u1,v1));
            verts.push_back(Vertex(a2,b1,h, 0,0,1, u2,v1));
            verts.push_back(Vertex(a2,b2,h, 0,0,1, u2,v2));

            // Back (z = -h)
            verts.push_back(Vertex(a1,b1,-h, 0,0,-1, u2,v1));
            verts.push_back(Vertex(a1,b2,-h, 0,0,-1, u2,v2));
            verts.push_back(Vertex(a2,b2,-h, 0,0,-1, u1,v2));
            verts.push_back(Vertex(a1,b1,-h, 0,0,-1, u2,v1));
            verts.push_back(Vertex(a2,b2,-h, 0,0,-1, u1,v2));
            verts.push_back(Vertex(a2,b1,-h, 0,0,-1, u1,v1));

            // Left (x = -h)
            verts.push_back(Vertex(-h,b1,a1, -1,0,0, u1,v1));
            verts.push_back(Vertex(-h,b1,a2, -1,0,0, u2,v1));
            verts.push_back(Vertex(-h,b2,a2, -1,0,0, u2,v2));
            verts.push_back(Vertex(-h,b1,a1, -1,0,0, u1,v1));
            verts.push_back(Vertex(-h,b2,a2, -1,0,0, u2,v2));
            verts.push_back(Vertex(-h,b2,a1, -1,0,0, u1,v2));

            // Right (x = +h)
            verts.push_back(Vertex(h,b1,a1, 1,0,0, u2,v1));
            verts.push_back(Vertex(h,b2,a1, 1,0,0, u2,v2));
            verts.push_back(Vertex(h,b2,a2, 1,0,0, u1,v2));
            verts.push_back(Vertex(h,b1,a1, 1,0,0, u2,v1));
            verts.push_back(Vertex(h,b2,a2, 1,0,0, u1,v2));
            verts.push_back(Vertex(h,b1,a2, 1,0,0, u1,v1));

            // Top (y = +h)
            verts.push_back(Vertex(a1,h,b1, 0,1,0, u1,v1));
            verts.push_back(Vertex(a1,h,b2, 0,1,0, u1,v2));
            verts.push_back(Vertex(a2,h,b2, 0,1,0, u2,v2));
            verts.push_back(Vertex(a1,h,b1, 0,1,0, u1,v1));
            verts.push_back(Vertex(a2,h,b2, 0,1,0, u2,v2));
            verts.push_back(Vertex(a2,h,b1, 0,1,0, u2,v1));

            // Bottom (y = -h)
            verts.push_back(Vertex(a1,-h,b1, 0,-1,0, u1,v2));
            verts.push_back(Vertex(a2,-h,b1, 0,-1,0, u2,v2));
            verts.push_back(Vertex(a2,-h,b2, 0,-1,0, u2,v1));
            verts.push_back(Vertex(a1,-h,b1, 0,-1,0, u1,v2));
            verts.push_back(Vertex(a2,-h,b2, 0,-1,0, u2,v1));
            verts.push_back(Vertex(a1,-h,b2, 0,-1,0, u1,v1));
        }
    }

    return verts;
}

bool generateBox(float length, int divisions, const char* filepath) {
    std::vector<Vertex> triangles = boxTriangles(length, divisions);
    if (triangles.empty()) {
        std::cerr << "Error: Empty vector of triangles.\n";
        return false;
    }
    saveToFile(triangles, filepath);
    return true;
}