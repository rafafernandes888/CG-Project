#ifndef CONFIGURATION_HPP
#define CONFIGURATION_HPP

#include <string>
#include <vector>
#include "Window.hpp"
#include "Camera.hpp"
#include "../../src/shared/include/point.hpp"

struct Model {
    std::string filename;
    std::vector<Point> vertices; 
};

struct Group {
    std::vector<Model> models;
};

class Configuration {
public:
    Window window;
    Camera camera;
    std::vector<Group> groups; 

    Configuration() = default;
    Configuration(Window window, Camera camera, std::vector<Group> groups);
};

#endif  // CONFIGURATION_HPP