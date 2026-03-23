#ifndef CONFIGURATION_HPP
#define CONFIGURATION_HPP

#include "Window.hpp"
#include "Camera.hpp"
#include "group.hpp"

class Configuration {
public:
    Window window;
    Camera camera;
    Group root;

    Configuration() = default;
    Configuration(const Window& window, const Camera& camera, const Group& root);
};

#endif