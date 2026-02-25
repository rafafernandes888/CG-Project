#include "Camera.hpp"

#include <sstream>
#include <string>

Camera::Camera() {
  this->position = Point();
  this->lookAt = Point();
  this->up = Point();
  this->fov = 0;
  this->nearP = 0;
  this->farP = 0;
}

Camera::Camera(Point position, Point lookAt, Point up, int fov, float nearP,
               float farP) {
  this->position = position;
  this->lookAt = lookAt;
  this->up = up;
  this->fov = fov;
  this->nearP = nearP;
  this->farP = farP;
}
