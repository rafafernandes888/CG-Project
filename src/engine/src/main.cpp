#ifdef _WIN32
#define GLUT_BUILDING_LIB
#include <windows.h>
#endif

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#define _USE_MATH_DEFINES
#include <math.h>
#include <iostream>
#include "draw.hpp"
#include "parse.hpp"

float alpha = 0.0f;
float beta = 0.0f;
float camRadius = 5.0f;
int axis = 1;

Configuration c;
std::vector<std::vector<Point>> vectors;

void reshape(int w, int h) {
    float aspect_ratio = (float)w / (float)h;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(c.camera.fov, aspect_ratio, c.camera.nearP, c.camera.farP);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void drawAxis(void) {
    if (axis) {
        glBegin(GL_LINES);
        glColor3f(1.0f, 0.0f, 0.0f);
        glVertex3f(-500.0f, 0.0f, 0.0f);
        glVertex3f(500.0f, 0.0f, 0.0f);
        glColor3f(0.0f, 1.0f, 0.0f);
        glVertex3f(0.0f, -500.0f, 0.0f);
        glVertex3f(0.0f, 500.0f, 0.0f);
        glColor3f(0.0f, 0.0f, 1.0f);
        glVertex3f(0.0f, 0.0f, -500.0f);
        glVertex3f(0.0f, 0.0f, 500.0f);
        glEnd();
    }
}

void renderScene(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    float lx = c.camera.lookAt.x;
    float ly = c.camera.lookAt.y;
    float lz = c.camera.lookAt.z;

    float camX = lx + camRadius * cosf(beta) * sinf(alpha);
    float camY = ly + camRadius * sinf(beta);
    float camZ = lz + camRadius * cosf(beta) * cosf(alpha);

    gluLookAt(camX, camY, camZ,
        lx, ly, lz,
        0.0f, 1.0f, 0.0f);

    drawAxis();

    glColor3f(1.0f, 1.0f, 1.0f);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    for (const auto& model : vectors) {
        drawTriangles(model);
    }

    glutSwapBuffers();
}

void processSpecialKeys(int key, int /* xx */, int /* yy */) {
    switch (key) {
    case GLUT_KEY_LEFT:  alpha -= 0.05f; break;
    case GLUT_KEY_RIGHT: alpha += 0.05f; break;
    case GLUT_KEY_UP:    beta += 0.05f; break;
    case GLUT_KEY_DOWN:  beta -= 0.05f; break;
    }
    glutPostRedisplay();
}

void processNormalKeys(unsigned char key, int /* x */, int /* y */) {
    switch (key) {
    case 'a':
        axis = !axis;
        break;
    case 'r': {
        float dx = c.camera.position.x - c.camera.lookAt.x;
        float dy = c.camera.position.y - c.camera.lookAt.y;
        float dz = c.camera.position.z - c.camera.lookAt.z;
        camRadius = sqrtf(dx * dx + dy * dy + dz * dz);
        beta = asinf(dy / camRadius);
        alpha = atan2f(dx, dz);
        break;
    }
    case 'o': camRadius += 0.2f; break;
    case 'i': camRadius -= 0.2f; if (camRadius < 0.1f) camRadius = 0.1f; break;
    }
    glutPostRedisplay();
}

void setupConfig(char* arg) {
    std::string filename(arg);
    c = parseConfig(filename);

    float dx = c.camera.position.x - c.camera.lookAt.x;
    float dy = c.camera.position.y - c.camera.lookAt.y;
    float dz = c.camera.position.z - c.camera.lookAt.z;
    camRadius = sqrtf(dx * dx + dy * dy + dz * dz);
    beta = asinf(dy / camRadius);
    alpha = atan2f(dx, dz);

    for (const auto& group : c.groups) {
        for (const auto& model : group.models) {
            vectors.push_back(model.vertices);
        }
    }
}

int main(int argc, char** argv) {
    if (argc == 1) {
        std::cout << "Invalid Arguments\n";
        std::cout << "Usage: ./engine <scene.xml>\n";
        return 1;
    }

    setupConfig(argv[1]);

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(c.window.width, c.window.height);
    glutCreateWindow("CG@DI");

    glutIdleFunc(renderScene);
    glutDisplayFunc(renderScene);
    glutReshapeFunc(reshape);
    glutSpecialFunc(processSpecialKeys);
    glutKeyboardFunc(processNormalKeys);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    glutMainLoop();
    return 0;
}