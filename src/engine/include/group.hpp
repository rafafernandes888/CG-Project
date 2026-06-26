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
    float time;
    bool align;
    std::vector<Point> points;
    Point prevY;

    CatmullRomTranslate() : time(0), align(false), prevY(0, 1, 0) {}
};

struct Transform {
    TransformType type;
    float x, y, z;
    float angle;
    float rotTime;
    CatmullRomTranslate catmullRom;

    Transform() : type(TRANSFORM_SCALE), x(0), y(0), z(0), angle(0), rotTime(0) {}
};

// ---- Light ----

enum LightType {
    LIGHT_POINT,
    LIGHT_DIRECTIONAL,
    LIGHT_SPOTLIGHT
};

struct Light {
    LightType type;
    float posX, posY, posZ;
    float dirX, dirY, dirZ;
    float cutoff;

    Light() : type(LIGHT_POINT),
              posX(0), posY(0), posZ(0),
              dirX(0), dirY(-1), dirZ(0),
              cutoff(45.0f) {}
};

// ---- Material ----

struct Material {
    float diffuse[4];
    float ambient[4];
    float specular[4];
    float emissive[4];
    float shininess;

    Material() : shininess(0.0f) {
        diffuse[0] = 200/255.0f; diffuse[1] = 200/255.0f; diffuse[2] = 200/255.0f; diffuse[3] = 1.0f;
        ambient[0] = 50/255.0f;  ambient[1] = 50/255.0f;  ambient[2] = 50/255.0f;  ambient[3] = 1.0f;
        specular[0] = 0; specular[1] = 0; specular[2] = 0; specular[3] = 1.0f;
        emissive[0] = 0; emissive[1] = 0; emissive[2] = 0; emissive[3] = 1.0f;
    }
};

// ---- Model with VBOs ----

struct ModelData {
    std::string filename;
    GLuint vboPositions;
    GLuint vboNormals;
    GLuint vboTexCoords;
    int vertexCount;
    Material material;
    std::string textureFile;
    GLuint textureId;

    ModelData() : vboPositions(0), vboNormals(0), vboTexCoords(0),
                  vertexCount(0), textureId(0) {}
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