#include "Configuration.hpp"

Configuration::Configuration(const Window& window, const Camera& camera,
                             const Group& root, const std::vector<Light>& lights) {
    this->window = window;
    this->camera = camera;
    this->root = root;
    this->lights = lights;
}