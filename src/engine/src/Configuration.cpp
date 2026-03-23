#include "Configuration.hpp"

Configuration::Configuration(const Window& window, const Camera& camera, const Group& root) {
    this->window = window;
    this->camera = camera;
    this->root = root;
}