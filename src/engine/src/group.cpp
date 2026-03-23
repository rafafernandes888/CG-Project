#define _USE_MATH_DEFINES
#include <cmath>

#include "group.hpp"

static std::array<std::array<double, 4>, 4> multiply(
    const std::array<std::array<double, 4>, 4>& A,
    const std::array<std::array<double, 4>, 4>& B)
{
    std::array<std::array<double, 4>, 4> R{};

    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j) {
            double sum = 0.0;
            for (int k = 0; k < 4; ++k) sum += A[i][k] * B[k][j];
            R[i][j] = sum;
        }

    return R;
}

Group::Group() = default;

Group::Group(std::vector<std::string> models,
             std::vector<Group> subgroups,
             std::vector<Point> points,
             std::array<std::array<double, 4>, 4> arr)
    : models(std::move(models)),
      subgroups(std::move(subgroups)),
      points(std::move(points)),
      arr(arr)
{}

void Group::translate(double x, double y, double z) {
    std::array<std::array<double, 4>, 4> T = {{
        {{1, 0, 0, x}},
        {{0, 1, 0, y}},
        {{0, 0, 1, z}},
        {{0, 0, 0, 1}}
    }};
    arr = multiply(arr, T);
}

void Group::scale(double x, double y, double z) {
    std::array<std::array<double, 4>, 4> S = {{
        {{x, 0, 0, 0}},
        {{0, y, 0, 0}},
        {{0, 0, z, 0}},
        {{0, 0, 0, 1}}
    }};
    arr = multiply(arr, S);
}

void Group::rotate(double angle, double x, double y, double z) {
    // normalizar eixo
    double len = std::sqrt(x*x + y*y + z*z);
    if (len == 0.0) return;
    x /= len; y /= len; z /= len;

    double rad = angle * M_PI / 180.0;
    double c = std::cos(rad);
    double s = std::sin(rad);
    double t = 1.0 - c;

    std::array<std::array<double, 4>, 4> R = {{
        {{t*x*x + c,     t*x*y - s*z,   t*x*z + s*y, 0}},
        {{t*x*y + s*z,   t*y*y + c,     t*y*z - s*x, 0}},
        {{t*x*z - s*y,   t*y*z + s*x,   t*z*z + c,   0}},
        {{0,             0,             0,           1}}
    }};

    arr = multiply(arr, R);
}