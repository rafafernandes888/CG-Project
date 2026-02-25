#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <string>


class Window {
 public:
  float width;
  float height;

  Window();
  Window(float width, float height);

  // std::string toString();
};

#endif  // WINDOW_HPP