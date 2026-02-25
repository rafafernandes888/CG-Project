#ifndef POINT_HPP
#define POINT_HPP

struct Point {
    float x, y, z;

    // Construtor
    Point(float x_val = 0.0f, float y_val = 0.0f, float z_val = 0.0f)
        : x(x_val), y(y_val), z(z_val) {}
};

#endif  // POINT_HPP