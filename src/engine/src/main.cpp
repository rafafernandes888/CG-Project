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

#include <IL/il.h>

#define _USE_MATH_DEFINES
#include <math.h>
#include <iostream>
#include <string>

#include "draw.hpp"
#include "Parse.hpp"

float alpha = 0.0f;
float beta = 0.0f;
float camRadius = 5.0f;
int axis = 1;
int renderMode = 0;
int showCurves = 1;
int showNormals = 0;

int fpsFrame = 0;
int fpsTimeBase = 0;

Configuration c;

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
        glDisable(GL_LIGHTING);
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
        if (!c.lights.empty()) glEnable(GL_LIGHTING);
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
              c.camera.up.x, c.camera.up.y, c.camera.up.z);

    setupLights(c.lights);

    drawAxis();

    switch (renderMode) {
        case 0: glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);  break;
        case 1: glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);  break;
        case 2: glPolygonMode(GL_FRONT_AND_BACK, GL_POINT); break;
    }

    glColor3f(1.0f, 1.0f, 1.0f);
    drawGroup(c.root);

    glutSwapBuffers();

    // FPS counter
    fpsFrame++;
    int currentTime = glutGet(GLUT_ELAPSED_TIME);
    if (currentTime - fpsTimeBase > 1000) {
        float fps = fpsFrame * 1000.0f / (currentTime - fpsTimeBase);
        fpsTimeBase = currentTime;
        fpsFrame = 0;
        char title[64];
        snprintf(title, sizeof(title), "CG - Phase 4  |  FPS: %.1f", fps);
        glutSetWindowTitle(title);
    }
}

void processSpecialKeys(int key, int, int) {
    switch (key) {
        case GLUT_KEY_LEFT:  alpha -= 0.1f; break;
        case GLUT_KEY_RIGHT: alpha += 0.1f; break;
        case GLUT_KEY_UP:
            beta += 0.1f;
            if (beta > 1.5f) beta = 1.5f;
            break;
        case GLUT_KEY_DOWN:
            beta -= 0.1f;
            if (beta < -1.5f) beta = -1.5f;
            break;
    }
    glutPostRedisplay();
}

void processNormalKeys(unsigned char key, int, int) {
    switch (key) {
        case 'a':
            axis = !axis;
            break;
        case 'm':
            renderMode = (renderMode + 1) % 3;
            break;
        case 'c':
            showCurves = !showCurves;
            break;
        case 'n':
            showNormals = !showNormals;
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
        case 'o':
            camRadius += 2.0f;
            break;
        case 'i':
            camRadius -= 2.0f;
            if (camRadius < 0.1f) camRadius = 0.1f;
            break;
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
}

int main(int argc, char** argv) {
    if (argc == 1) {
        std::cout << "Invalid Arguments\n";
        std::cout << "Usage: ./engine <scene.xml>\n";
        return 1;
    }

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(1280, 800);
    glutCreateWindow("CG - Phase 4");

    GLenum err = glewInit();
    if (err != GLEW_OK) {
        std::cerr << "GLEW init error: " << glewGetErrorString(err) << std::endl;
        return 1;
    }

    ilInit();

    setupConfig(argv[1]);

    glutReshapeWindow(c.window.width, c.window.height);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_RESCALE_NORMAL);
    glEnable(GL_TEXTURE_2D);

    float globalAmb[4] = {1.0f, 1.0f, 1.0f, 1.0f};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmb);

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    glutIdleFunc(renderScene);
    glutDisplayFunc(renderScene);
    glutReshapeFunc(reshape);
    glutSpecialFunc(processSpecialKeys);
    glutKeyboardFunc(processNormalKeys);

    glutMainLoop();
    return 0;
}