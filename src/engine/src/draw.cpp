#ifdef _WIN32
#define GLUT_BUILDING_LIB
#include <windows.h>
#endif

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <vector>
#include "../../shared/include/utils.hpp"
#include "draw.hpp"

void drawTriangles(const std::vector<Point>& points) {
    glBegin(GL_TRIANGLES);
    for (size_t i = 0; i + 2 < points.size(); i += 3) {
        glColor3f(1.0f, 1.0f, 1.0f);
        glVertex3f(points[i].x,     points[i].y,     points[i].z);
        glVertex3f(points[i + 1].x, points[i + 1].y, points[i + 1].z);
        glVertex3f(points[i + 2].x, points[i + 2].y, points[i + 2].z);
    }
    glEnd();
}

void drawGroup(const Group& group) {
    glPushMatrix();

    glMultMatrixd(&group.arr[0][0]);

    drawTriangles(group.points);

    for (const auto& child : group.subgroups) {
        drawGroup(child);
    }

    glPopMatrix();
}