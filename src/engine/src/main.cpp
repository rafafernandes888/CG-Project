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

float cameraAngle  = 0.0f;
float cameraAngleY = 0.0f;
float zoom = 1.0f;
int axis = 1;

Configuration c;
std::vector<std::vector<Point>> vectors;

void reshape(int w, int h) {
    float aspect_ratio = (float)w / (float)h;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // ✅ nearP e farP em vez de near e far
    gluPerspective(c.camera.fov, aspect_ratio, c.camera.nearP, c.camera.farP);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void drawAxis(void) {
    if (axis) {
        glBegin(GL_LINES);
        glColor3f(50.0f, 0.0f, 0.0f);
        glVertex3f(-500.0f, 0.0f, 0.0f);
        glVertex3f(500.0f, 0.0f, 0.0f);
        glColor3f(0.0f, 50.0f, 0.0f);
        glVertex3f(0.0f, -500.0f, 0.0f);
        glVertex3f(0.0f, 500.0f, 0.0f);
        glColor3f(0.0f, 0.0f, 50.0f);
        glVertex3f(0.0f, 0.0f, -500.0f);
        glVertex3f(0.0f, 0.0f, 500.0f);
        glEnd();
    }
}

void renderScene(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(c.camera.position.x, c.camera.position.y, c.camera.position.z,
              c.camera.lookAt.x,   c.camera.lookAt.y,   c.camera.lookAt.z,
              c.camera.up.x,       c.camera.up.y,       c.camera.up.z);

    glRotatef(cameraAngleY, 0.0f, 1.0f, 0.0f);
    glRotatef(cameraAngle,  1.0f, 0.0f, 0.0f);
    glScalef(zoom, zoom, zoom);

    drawAxis();

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    for (std::vector<Point> model : vectors) {
        drawTriangles(model);
    }

    glutSwapBuffers();
}

void processSpecialKeys(int key, int xx, int yy) {
    switch (key) {
        case GLUT_KEY_LEFT:  cameraAngle  -= 1.0f; break;
        case GLUT_KEY_RIGHT: cameraAngle  += 1.0f; break;
        case GLUT_KEY_UP:    cameraAngleY += 1.0f; break;
        case GLUT_KEY_DOWN:  cameraAngleY -= 1.0f; break;
    }
    glutPostRedisplay();
}

void processNormalKeys(unsigned char key, int x, int y) {
    float value = zoom * 0.1f;
    switch (key) {
        case 'a': axis = !axis; break;
        case 'r': cameraAngle = 0; cameraAngleY = 0; zoom = 1.0f; break;
        case 'o': zoom -= value; break;
        case 'i': zoom += value; break;
    }
    glutPostRedisplay();
}

void setupConfig(char* arg) {
    std::cout << "setupConfig chamado com: " << arg << std::endl;  // ← NOVO
    std::cout.flush();  // ← força o print antes do crash
    
    std::string filename;
    filename.assign(arg);

    std::cout << "A chamar parseConfig..." << std::endl;  // ← NOVO
    std::cout.flush();

    c = parseConfig(filename);

    std::cout << "parseConfig OK!" << std::endl;  // ← NOVO
    std::cout.flush();
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

    std::cout << "A carregar: " << argv[1] << std::endl;  // ← NOVO
    
    setupConfig(argv[1]);
    
    std::cout << "Config carregada!" << std::endl;        // ← NOVO
    std::cout << "Janela: " << c.window.width << "x" << c.window.height << std::endl; // ← NOVO
    std::cout << "Modelos: " << vectors.size() << std::endl; // ← NOVO

    glutInit(&argc, argv);
    std::cout << "GLUT iniciado!" << std::endl;           // ← NOVO
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(c.window.width, c.window.height);
    glutCreateWindow("CG@DI");
    std::cout << "Janela criada!" << std::endl;           // ← NOVO

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