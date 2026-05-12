#ifndef GROUP_HPP
#define GROUP_HPP

#include <string>
#include <vector>
#include "../../shared/include/utils.hpp"

#ifdef _WIN32
#define GLUT_BUILDING_LIB
#include <windows.h>
#endif

#include <GL/glew.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

// ---- Transform types ----

enum TransformType {
    TRANSFORM_TRANSLATE_STATIC,
    TRANSFORM_TRANSLATE_CATMULLROM,
    TRANSFORM_ROTATE_STATIC,
    TRANSFORM_ROTATE_TIME,
    TRANSFORM_SCALE
};

struct CatmullRomTranslate {
    float time;          // seconds for full loop
    bool align;          // align object to curve direction
    std::vector<Point> points;  // control points
    Point prevY;         // for align: previous Y axis (to keep continuity)

    CatmullRomTranslate() : time(0), align(false), prevY(0, 1, 0) {}
};

struct Transform {
    TransformType type;

    // Static translate / scale
    float x, y, z;

    // Static rotate
    float angle;

    // Time-based rotate
    float rotTime; // seconds for 360 degrees

    // Catmull-Rom translate
    CatmullRomTranslate catmullRom;

    Transform() : type(TRANSFORM_SCALE), x(0), y(0), z(0), angle(0), rotTime(0) {}
};

// ---- Model with VBO ----

struct ModelData {
    std::string filename;
    GLuint vboId;
    int vertexCount;
    float r, g, b;

    ModelData() : vboId(0), vertexCount(0), r(1.0f), g(1.0f), b(1.0f) {}
};

// ---- Group ----

class Group {
public:
    std::vector<Transform> transforms;
    std::vector<ModelData> models;
    std::vector<Group> subgroups;

    Group() = default;
};

#endif