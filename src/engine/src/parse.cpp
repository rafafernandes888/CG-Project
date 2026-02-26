#include "parse.hpp"
#include "../../../libs/rapidxml-1.13/rapidxml.hpp"
#include "../../../src/shared/include/utils.hpp"
#include <fstream>
#include <iostream>
#include <string>

Configuration parseConfig(std::string filename) {
    std::cerr << "A abrir: " << filename << std::endl;

    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error opening the file: " << filename << std::endl;
        exit(1);
    }
    std::cerr << "Ficheiro aberto!" << std::endl;

    std::string xmlContent((std::istreambuf_iterator<char>(file)),
        std::istreambuf_iterator<char>());
    file.close();

    rapidxml::xml_document<> doc;
    doc.parse<0>(&xmlContent[0]);

    rapidxml::xml_node<>* root = doc.first_node("world");

    // window
    char* width = root->first_node("window")->first_attribute("width")->value();
    char* height = root->first_node("window")->first_attribute("height")->value();
    Window window_info = Window(static_cast<int>(std::stof(width)),
        static_cast<int>(std::stof(height)));

    // camera
    rapidxml::xml_node<>* camera = root->first_node("camera");

    rapidxml::xml_node<>* position_n = camera->first_node("position");
    Point position = Point(std::stof(position_n->first_attribute("x")->value()),
        std::stof(position_n->first_attribute("y")->value()),
        std::stof(position_n->first_attribute("z")->value()));

    rapidxml::xml_node<>* lookAt_n = camera->first_node("lookAt");
    Point lookAt = Point(std::stof(lookAt_n->first_attribute("x")->value()),
        std::stof(lookAt_n->first_attribute("y")->value()),
        std::stof(lookAt_n->first_attribute("z")->value()));

    rapidxml::xml_node<>* up_n = camera->first_node("up");
    Point up = Point(std::stof(up_n->first_attribute("x")->value()),
        std::stof(up_n->first_attribute("y")->value()),
        std::stof(up_n->first_attribute("z")->value()));

    rapidxml::xml_node<>* projection = camera->first_node("projection");
    float fov = std::stof(projection->first_attribute("fov")->value());
    float nearP = std::stof(projection->first_attribute("near")->value());
    float farP = std::stof(projection->first_attribute("far")->value());

    Camera camera_info = Camera(position, lookAt, up, fov, nearP, farP);

    // models
    std::string scenesDir = filename.substr(0, filename.find_last_of("/\\"));
    std::string modelsDir = scenesDir + "/../models";

    Group group;
    rapidxml::xml_node<>* models =
        root->first_node("group")->first_node("models");

    for (rapidxml::xml_node<>* model = models->first_node("model");
        model;
        model = model->next_sibling("model"))
    {
        Model m;
        m.filename = std::string(model->first_attribute("file")->value());

        std::string fullPath = modelsDir + "/" + m.filename;
        m.vertices = parse3Dfile(fullPath);

        if (m.vertices.empty())
            std::cerr << "Aviso: " << fullPath << " vazio ou nao encontrado\n";
        else
            std::cout << "Carregado: " << m.filename
            << " (" << m.vertices.size() << " vertices)\n";

        group.models.push_back(m);
    }

    std::vector<Group> groups;
    groups.push_back(group);

    return Configuration(window_info, camera_info, groups);
}