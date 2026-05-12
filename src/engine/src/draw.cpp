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

#define _USE_MATH_DEFINES
#include <cmath>
#include <vector>

#include "draw.hpp"

extern int showCurves;

// ===================== Catmull-Rom curve =====================

static const float catmullRomMatrix[4][4] = {
    {-0.5f,  1.5f, -1.5f,  0.5f},
    { 1.0f, -2.5f,  2.0f, -0.5f},
    {-0.5f,  0.0f,  0.5f,  0.0f},
    { 0.0f,  1.0f,  0.0f,  0.0f}
};

static void getCatmullRomPoint(float t, Point p0, Point p1, Point p2, Point p3,
                               Point& pos, Point& deriv) {
    float tt = t * t;
    float ttt = tt * t;

    float T[4]  = { ttt, tt, t, 1.0f };
    float dT[4] = { 3*tt, 2*t, 1.0f, 0.0f };

    // P matrix (each row is a coordinate axis)
    float px[4] = { p0.x, p1.x, p2.x, p3.x };
    float py[4] = { p0.y, p1.y, p2.y, p3.y };
    float pz[4] = { p0.z, p1.z, p2.z, p3.z };

    // A = M * P for each axis
    float ax[4], ay[4], az[4];
    for (int i = 0; i < 4; i++) {
        ax[i] = ay[i] = az[i] = 0;
        for (int j = 0; j < 4; j++) {
            ax[i] += catmullRomMatrix[i][j] * px[j];
            ay[i] += catmullRomMatrix[i][j] * py[j];
            az[i] += catmullRomMatrix[i][j] * pz[j];
        }
    }

    // pos = T * A, deriv = dT * A
    pos.x = pos.y = pos.z = 0;
    deriv.x = deriv.y = deriv.z = 0;
    for (int i = 0; i < 4; i++) {
        pos.x += T[i] * ax[i];
        pos.y += T[i] * ay[i];
        pos.z += T[i] * az[i];
        deriv.x += dT[i] * ax[i];
        deriv.y += dT[i] * ay[i];
        deriv.z += dT[i] * az[i];
    }
}

static void getGlobalCatmullRomPoint(float gt, const std::vector<Point>& pts,
                                     Point& pos, Point& deriv) {
    int pointCount = pts.size();
    float t = gt * pointCount;
    int index = (int)floor(t);
    t = t - index;

    int indices[4];
    indices[0] = (index + pointCount - 1) % pointCount;
    indices[1] = (index) % pointCount;
    indices[2] = (index + 1) % pointCount;
    indices[3] = (index + 2) % pointCount;

    getCatmullRomPoint(t, pts[indices[0]], pts[indices[1]],
                       pts[indices[2]], pts[indices[3]], pos, deriv);
}

// Draw the Catmull-Rom curve as a line loop (debug visualization)
static void renderCatmullRomCurve(const std::vector<Point>& pts) {
    glBegin(GL_LINE_LOOP);
    glColor3f(0.5f, 0.5f, 0.5f);
    float step = 1.0f / 100.0f;
    for (int i = 0; i < 100; i++) {
        Point pos, deriv;
        getGlobalCatmullRomPoint(i * step, pts, pos, deriv);
        glVertex3f(pos.x, pos.y, pos.z);
    }
    glEnd();
}

// Normalize a vector
static void normalize(float* v) {
    float len = sqrtf(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
    if (len > 0.0001f) {
        v[0] /= len; v[1] /= len; v[2] /= len;
    }
}

// Cross product: res = a x b
static void cross(float* a, float* b, float* res) {
    res[0] = a[1]*b[2] - a[2]*b[1];
    res[1] = a[2]*b[0] - a[0]*b[2];
    res[2] = a[0]*b[1] - a[1]*b[0];
}

// ===================== Apply transforms =====================

static void applyTransforms(Group& group) {
    float elapsed = glutGet(GLUT_ELAPSED_TIME) / 1000.0f; // seconds

    for (auto& t : group.transforms) {
        switch (t.type) {
            case TRANSFORM_TRANSLATE_STATIC:
                glTranslatef(t.x, t.y, t.z);
                break;

            case TRANSFORM_TRANSLATE_CATMULLROM: {
                auto& cr = t.catmullRom;
                if (showCurves) renderCatmullRomCurve(cr.points);

                float gt = fmod(elapsed, cr.time) / cr.time;
                Point pos, deriv;
                getGlobalCatmullRomPoint(gt, cr.points, pos, deriv);

                glTranslatef(pos.x, pos.y, pos.z);

                if (cr.align) {
                    float X[3] = { deriv.x, deriv.y, deriv.z };
                    normalize(X);

                    float Y[3] = { cr.prevY.x, cr.prevY.y, cr.prevY.z };

                    float Z[3];
                    cross(X, Y, Z);
                    normalize(Z);

                    cross(Z, X, Y);
                    normalize(Y);

                    cr.prevY = Point(Y[0], Y[1], Y[2]);

                    float m[16] = {
                        X[0], X[1], X[2], 0,
                        Y[0], Y[1], Y[2], 0,
                        Z[0], Z[1], Z[2], 0,
                        0,    0,    0,    1
                    };
                    glMultMatrixf(m);
                }
                break;
            }

            case TRANSFORM_ROTATE_STATIC:
                glRotatef(t.angle, t.x, t.y, t.z);
                break;

            case TRANSFORM_ROTATE_TIME: {
                float angle = fmod(elapsed, t.rotTime) / t.rotTime * 360.0f;
                glRotatef(angle, t.x, t.y, t.z);
                break;
            }

            case TRANSFORM_SCALE:
                glScalef(t.x, t.y, t.z);
                break;
        }
    }
}

// ===================== Draw models using VBOs =====================

static void drawModels(const std::vector<ModelData>& models) {
    for (const auto& m : models) {
        if (m.vboId == 0 || m.vertexCount == 0) continue;

        glColor3f(m.r, m.g, m.b); // Apply model specific color
        glBindBuffer(GL_ARRAY_BUFFER, m.vboId);
        glVertexPointer(3, GL_FLOAT, 0, 0);
        glDrawArrays(GL_TRIANGLES, 0, m.vertexCount);
    }
}

// ===================== Draw group recursively =====================

void drawGroup(Group& group) {
    glPushMatrix();

    applyTransforms(group);

    drawModels(group.models);

    for (auto& child : group.subgroups) {
        drawGroup(child);
    }

    glPopMatrix();
}