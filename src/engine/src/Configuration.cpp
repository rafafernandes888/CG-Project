#include "Configuration.hpp"

Configuration::Configuration(Window window, Camera camera,
                             std::vector<Group> groups) {
    this->window = window;
    this->camera = camera;
    this->groups = groups;
}