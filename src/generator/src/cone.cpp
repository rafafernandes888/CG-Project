#define _USE_MATH_DEFINES
#include <cmath>
#include <fstream>
#include <iostream>
#include <vector>

#include "../../shared/include/utils.hpp"

std::vector<Vertex> coneTriangles(float radius, float height, int slices, int stacks) {
    std::vector<Vertex> verts;

    const float alfa = 2.0f * (float)M_PI / slices;
    const float stackHeight = height / stacks;
    const float slopeLen = sqrtf(height * height + radius * radius);

    for (int slice = 0; slice < slices; ++slice) {
        float theta1 = slice * alfa;
        float theta2 = (slice + 1) * alfa;

        // Lateral normal: (h*sin(theta), r, h*cos(theta)) / slopeLen
        float nx1 = height * sinf(theta1) / slopeLen;
        float ny  = radius / slopeLen;
        float nz1 = height * cosf(theta1) / slopeLen;
        float nx2 = height * sinf(theta2) / slopeLen;
        float nz2 = height * cosf(theta2) / slopeLen;

        float u1 = (float)slice / slices;
        float u2 = (float)(slice + 1) / slices;

        for (int stack = 0; stack < stacks; ++stack) {
            float cR = radius - stack * radius / stacks;
            float nR = radius - (stack + 1) * radius / stacks;
            float y_lo = stack * stackHeight;
            float y_hi = (stack + 1) * stackHeight;

            float v_lo = (float)stack / stacks;
            float v_hi = (float)(stack + 1) / stacks;

            Vertex bl(cR*sinf(theta1), y_lo, cR*cosf(theta1), nx1,ny,nz1, u1,v_lo);
            Vertex br(cR*sinf(theta2), y_lo, cR*cosf(theta2), nx2,ny,nz2, u2,v_lo);
            Vertex tl(nR*sinf(theta1), y_hi, nR*cosf(theta1), nx1,ny,nz1, u1,v_hi);
            Vertex tr(nR*sinf(theta2), y_hi, nR*cosf(theta2), nx2,ny,nz2, u2,v_hi);

            verts.push_back(tl);
            verts.push_back(bl);
            verts.push_back(br);

            verts.push_back(tl);
            verts.push_back(br);
            verts.push_back(tr);
        }

        // Base
        float bx1 = radius * sinf(theta1);
        float bz1 = radius * cosf(theta1);
        float bx2 = radius * sinf(theta2);
        float bz2 = radius * cosf(theta2);

        float bu1 = 0.5f + 0.5f * sinf(theta1);
        float bv1 = 0.5f + 0.5f * cosf(theta1);
        float bu2 = 0.5f + 0.5f * sinf(theta2);
        float bv2 = 0.5f + 0.5f * cosf(theta2);

        verts.push_back(Vertex(0,0,0,       0,-1,0,  0.5f,0.5f));
        verts.push_back(Vertex(bx2,0,bz2,   0,-1,0,  bu2,bv2));
        verts.push_back(Vertex(bx1,0,bz1,   0,-1,0,  bu1,bv1));
    }

    return verts;
}

bool generateCone(float radius, float height, int slices, int stacks, const char* filepath) {
    std::vector<Vertex> triangles = coneTriangles(radius, height, slices, stacks);
    if (triangles.empty()) {
        std::cerr << "Error: Empty vector of triangles.\n";
        return false;
    }
    saveToFile(triangles, filepath);
    return true;
}