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
#include "Configuration.hpp"

extern int showCurves;
extern int showNormals;
extern Configuration c;

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

    float px[4] = { p0.x, p1.x, p2.x, p3.x };
    float py[4] = { p0.y, p1.y, p2.y, p3.y };
    float pz[4] = { p0.z, p1.z, p2.z, p3.z };

    float ax[4], ay[4], az[4];
    for (int i = 0; i < 4; i++) {
        ax[i] = ay[i] = az[i] = 0;
        for (int j = 0; j < 4; j++) {
            ax[i] += catmullRomMatrix[i][j] * px[j];
            ay[i] += catmullRomMatrix[i][j] * py[j];
            az[i] += catmullRomMatrix[i][j] * pz[j];
        }
    }

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

static void renderCatmullRomCurve(const std::vector<Point>& pts) {
    glDisable(GL_LIGHTING);
    glBegin(GL_LINE_LOOP);
    glColor3f(0.5f, 0.5f, 0.5f);
    float step = 1.0f / 100.0f;
    for (int i = 0; i < 100; i++) {
        Point pos, deriv;
        getGlobalCatmullRomPoint(i * step, pts, pos, deriv);
        glVertex3f(pos.x, pos.y, pos.z);
    }
    glEnd();
    if (!c.lights.empty()) glEnable(GL_LIGHTING);
}

static void normalize(float* v) {
    float len = sqrtf(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
    if (len > 0.0001f) {
        v[0] /= len; v[1] /= len; v[2] /= len;
    }
}

static void cross(float* a, float* b, float* res) {
    res[0] = a[1]*b[2] - a[2]*b[1];
    res[1] = a[2]*b[0] - a[0]*b[2];
    res[2] = a[0]*b[1] - a[1]*b[0];
}

// ===================== Setup lights =====================

void setupLights(const std::vector<Light>& lights) {
    if (lights.empty()) {
        glDisable(GL_LIGHTING);
        return;
    }

    glEnable(GL_LIGHTING);

    for (size_t i = 0; i < lights.size() && i < 8; i++) {
        GLenum lightId = GL_LIGHT0 + (GLenum)i;
        glEnable(lightId);

        const Light& l = lights[i];

        float white[4] = {1, 1, 1, 1};
        glLightfv(lightId, GL_DIFFUSE,  white);
        glLightfv(lightId, GL_SPECULAR, white);

        switch (l.type) {
            case LIGHT_POINT: {
                float pos[4] = { l.posX, l.posY, l.posZ, 1.0f };
                glLightfv(lightId, GL_POSITION, pos);
                glLightf(lightId, GL_SPOT_CUTOFF, 180.0f);
                glLightf(lightId, GL_CONSTANT_ATTENUATION, 1.0f);
                glLightf(lightId, GL_LINEAR_ATTENUATION, 0.0f);
                glLightf(lightId, GL_QUADRATIC_ATTENUATION, 0.0f);
                break;
            }
            case LIGHT_DIRECTIONAL: {
                float dir[4] = { l.dirX, l.dirY, l.dirZ, 0.0f };
                glLightfv(lightId, GL_POSITION, dir);
                break;
            }
            case LIGHT_SPOTLIGHT: {
                float pos[4] = { l.posX, l.posY, l.posZ, 1.0f };
                float dir[3] = { l.dirX, l.dirY, l.dirZ };
                glLightfv(lightId, GL_POSITION, pos);
                glLightfv(lightId, GL_SPOT_DIRECTION, dir);
                glLightf(lightId, GL_SPOT_CUTOFF, l.cutoff);
                break;
            }
        }
    }
}

// ===================== Apply transforms =====================

static void applyTransforms(Group& group) {
    float elapsed = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;

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

// ===================== Draw models =====================

static void drawModels(const std::vector<ModelData>& models) {
    for (const auto& m : models) {
        if (m.vboPositions == 0 || m.vertexCount == 0) continue;

        // Apply material
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,  m.material.diffuse);
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,  m.material.ambient);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, m.material.specular);
        glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, m.material.emissive);
        glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, m.material.shininess);

        // Bind texture if available
        if (m.textureId != 0) {
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, m.textureId);
            glBindBuffer(GL_ARRAY_BUFFER, m.vboTexCoords);
            glTexCoordPointer(2, GL_FLOAT, 0, 0);
            glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        } else {
            glDisable(GL_TEXTURE_2D);
            glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        }

        // Bind normals
        glBindBuffer(GL_ARRAY_BUFFER, m.vboNormals);
        glNormalPointer(GL_FLOAT, 0, 0);

        // Bind positions
        glBindBuffer(GL_ARRAY_BUFFER, m.vboPositions);
        glVertexPointer(3, GL_FLOAT, 0, 0);

        // Draw
        glDrawArrays(GL_TRIANGLES, 0, m.vertexCount);

        // Cleanup texture state
        if (m.textureId != 0) {
            glDisable(GL_TEXTURE_2D);
            glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        }
    }
}

// ===================== Draw normals as red lines =====================

static void drawNormalsForModels(const std::vector<ModelData>& models) {
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glColor3f(1.0f, 0.0f, 0.0f);

    for (const auto& m : models) {
        if (m.vboPositions == 0 || m.vboNormals == 0 || m.vertexCount == 0) continue;

        glBindBuffer(GL_ARRAY_BUFFER, m.vboPositions);
        float* pos = (float*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_ONLY);
        if (!pos) continue;

        glBindBuffer(GL_ARRAY_BUFFER, m.vboNormals);
        float* nrm = (float*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_ONLY);
        if (!nrm) { glBindBuffer(GL_ARRAY_BUFFER, m.vboPositions); glUnmapBuffer(GL_ARRAY_BUFFER); continue; }

        float scale = 0.2f;
        glBegin(GL_LINES);
        for (int i = 0; i < m.vertexCount; i++) {
            float px = pos[i*3], py = pos[i*3+1], pz = pos[i*3+2];
            float nx = nrm[i*3], ny = nrm[i*3+1], nz = nrm[i*3+2];
            glVertex3f(px, py, pz);
            glVertex3f(px + nx*scale, py + ny*scale, pz + nz*scale);
        }
        glEnd();

        glBindBuffer(GL_ARRAY_BUFFER, m.vboNormals);
        glUnmapBuffer(GL_ARRAY_BUFFER);
        glBindBuffer(GL_ARRAY_BUFFER, m.vboPositions);
        glUnmapBuffer(GL_ARRAY_BUFFER);
    }

    if (!c.lights.empty()) glEnable(GL_LIGHTING);
}

// ===================== Draw group recursively =====================

void drawGroup(Group& group) {
    glPushMatrix();

    applyTransforms(group);
    drawModels(group.models);
    if (showNormals) drawNormalsForModels(group.models);

    for (auto& child : group.subgroups) {
        drawGroup(child);
    }

    glPopMatrix();
}