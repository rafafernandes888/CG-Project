#define _USE_MATH_DEFINES
#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <string>

#include "../../shared/include/utils.hpp"

static float bernstein(int i, float t) {
    switch (i) {
        case 0: return (1-t)*(1-t)*(1-t);
        case 1: return 3*t*(1-t)*(1-t);
        case 2: return 3*t*t*(1-t);
        case 3: return t*t*t;
        default: return 0;
    }
}

static float bernsteinDeriv(int i, float t) {
    switch (i) {
        case 0: return -3*(1-t)*(1-t);
        case 1: return 3*(1-t)*(1-3*t);
        case 2: return 3*t*(2-3*t);
        case 3: return 3*t*t;
        default: return 0;
    }
}

static void bezierPatchEval(const std::vector<Point>& cp, float u, float v,
                            Point& pos, Point& dU, Point& dV) {
    pos = Point(0,0,0);
    dU  = Point(0,0,0);
    dV  = Point(0,0,0);

    for (int i = 0; i < 4; ++i) {
        float bi  = bernstein(i, u);
        float bdi = bernsteinDeriv(i, u);
        for (int j = 0; j < 4; ++j) {
            float bj  = bernstein(j, v);
            float bdj = bernsteinDeriv(j, v);
            const Point& p = cp[i * 4 + j];

            float bib = bi * bj;
            pos.x += p.x * bib;
            pos.y += p.y * bib;
            pos.z += p.z * bib;

            float dbidu = bdi * bj;
            dU.x += p.x * dbidu;
            dU.y += p.y * dbidu;
            dU.z += p.z * dbidu;

            float dbidv = bi * bdj;
            dV.x += p.x * dbidv;
            dV.y += p.y * dbidv;
            dV.z += p.z * dbidv;
        }
    }
}

static void normalizeVec(float& x, float& y, float& z) {
    float len = sqrtf(x*x + y*y + z*z);
    if (len > 1e-6f) { x /= len; y /= len; z /= len; }
}

static std::vector<size_t> parseIndices(const std::string& line) {
    std::vector<size_t> indices;
    std::stringstream ss(line);
    std::string token;
    while (std::getline(ss, token, ',')) {
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

    std::string line;
    std::getline(file, line);
    int numPatches = std::stoi(line);

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

    std::getline(file, line);
    int numControlPoints = std::stoi(line);

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

    std::vector<Vertex> vertices;

    for (int p = 0; p < numPatches; ++p) {
        std::vector<Point> patchCP(16);
        for (int k = 0; k < 16; ++k) {
            patchCP[k] = controlPoints[patches[p][k]];
        }

        for (int ui = 0; ui < tessellation; ++ui) {
            for (int vi = 0; vi < tessellation; ++vi) {
                float u1 = (float)ui / tessellation;
                float v1 = (float)vi / tessellation;
                float u2 = (float)(ui + 1) / tessellation;
                float v2 = (float)(vi + 1) / tessellation;

                Point pos1, dU1, dV1, pos2, dU2, dV2;
                Point pos3, dU3, dV3, pos4, dU4, dV4;

                bezierPatchEval(patchCP, u1, v1, pos1, dU1, dV1);
                bezierPatchEval(patchCP, u2, v1, pos2, dU2, dV2);
                bezierPatchEval(patchCP, u1, v2, pos3, dU3, dV3);
                bezierPatchEval(patchCP, u2, v2, pos4, dU4, dV4);

                auto computeNormal = [](const Point& du, const Point& dv,
                                        float& nx, float& ny, float& nz) {
                    nx = dv.y * du.z - dv.z * du.y;
                    ny = dv.z * du.x - dv.x * du.z;
                    nz = dv.x * du.y - dv.y * du.x;
                    normalizeVec(nx, ny, nz);
                };

                float nx1,ny1,nz1, nx2,ny2,nz2, nx3,ny3,nz3, nx4,ny4,nz4;
                computeNormal(dU1, dV1, nx1,ny1,nz1);
                computeNormal(dU2, dV2, nx2,ny2,nz2);
                computeNormal(dU3, dV3, nx3,ny3,nz3);
                computeNormal(dU4, dV4, nx4,ny4,nz4);

                Vertex vt1(pos1.x,pos1.y,pos1.z, nx1,ny1,nz1, u1,v1);
                Vertex vt2(pos2.x,pos2.y,pos2.z, nx2,ny2,nz2, u2,v1);
                Vertex vt3(pos3.x,pos3.y,pos3.z, nx3,ny3,nz3, u1,v2);
                Vertex vt4(pos4.x,pos4.y,pos4.z, nx4,ny4,nz4, u2,v2);

                vertices.push_back(vt1);
                vertices.push_back(vt3);
                vertices.push_back(vt2);

                vertices.push_back(vt2);
                vertices.push_back(vt3);
                vertices.push_back(vt4);
            }
        }
    }

    if (vertices.empty()) {
        std::cerr << "Error: no triangles generated from patches\n";
        return false;
    }

    saveToFile(vertices, outputFile);
    return true;
}