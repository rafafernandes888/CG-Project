#ifndef CONFIGURATION_HPP
#define CONFIGURATION_HPP

#include <string>
#include <vector>
#include "Window.hpp"
#include "Camera.hpp"
#include "../../src/shared/include/point.hpp"

// Um modelo = nome do ficheiro + vértices já carregados em memória
struct Model {
    std::string filename;
    std::vector<Point> vertices;  // carregados UMA só vez no arranque
};

// Um grupo de modelos (fase 1: sem transformações)
struct Group {
    std::vector<Model> models;
};

class Configuration {
public:
    Window window;
    Camera camera;
    std::vector<Group> groups;  // ← grupos com modelos e vértices

    Configuration() = default;
    Configuration(Window window, Camera camera, std::vector<Group> groups);
};

#endif  // CONFIGURATION_HPP