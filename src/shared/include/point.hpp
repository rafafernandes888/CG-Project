#ifndef POINT_HPP
#define POINT_HPP

struct Point {
    float x, y, z;

    Point(float x_val = 0.0f, float y_val = 0.0f, float z_val = 0.0f)
        : x(x_val), y(y_val), z(z_val) {}
};

struct Vertex {
    float x, y, z;
    float nx, ny, nz;
    float u, v;

    Vertex(float x_ = 0, float y_ = 0, float z_ = 0,
           float nx_ = 0, float ny_ = 0, float nz_ = 0,
           float u_ = 0, float v_ = 0)
        : x(x_), y(y_), z(z_), nx(nx_), ny(ny_), nz(nz_), u(u_), v(v_) {}
};

#endif  // POINT_HPP