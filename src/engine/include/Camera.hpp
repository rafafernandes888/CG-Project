#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <string>

#include "../../shared/include/utils.hpp"

class Camera {
 public:
  Point position;
  Point lookAt;
  Point up;
  float fov;
  float nearP;
  float farP;

  Camera();
  Camera(Point position, Point lookAt, Point up, float fov, float nearP,
         float farP);

};

#endif  // CAMERA_HPP