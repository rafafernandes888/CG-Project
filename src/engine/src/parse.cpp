#include "parse.hpp"
#include "../../../libs/rapidxml-1.13/rapidxml.hpp"
#include "../../../src/shared/include/utils.hpp"

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

Group parseGroup(rapidxml::xml_node<>* groupNode, const std::string& modelsDir) {
    Group group;

    if (!groupNode) return group;

    // transform
    rapidxml::xml_node<>* transformNode = groupNode->first_node("transform");
    if (transformNode) {
        for (rapidxml::xml_node<>* node = transformNode->first_node();
             node;
             node = node->next_sibling()) {

            std::string nodeName = node->name();

            if (nodeName == "translate") {
                double x = std::stod(node->first_attribute("x")->value());
                double y = std::stod(node->first_attribute("y")->value());
                double z = std::stod(node->first_attribute("z")->value());
                group.translate(x, y, z);
            }
            else if (nodeName == "rotate") {
                double angle = std::stod(node->first_attribute("angle")->value());
                double x = std::stod(node->first_attribute("x")->value());
                double y = std::stod(node->first_attribute("y")->value());
                double z = std::stod(node->first_attribute("z")->value());
                group.rotate(angle, x, y, z);
            }
            else if (nodeName == "scale") {
                double x = std::stod(node->first_attribute("x")->value());
                double y = std::stod(node->first_attribute("y")->value());
                double z = std::stod(node->first_attribute("z")->value());
                group.scale(x, y, z);
            }
        }
    }

    // models
    rapidxml::xml_node<>* modelsNode = groupNode->first_node("models");
    if (modelsNode) {
        for (rapidxml::xml_node<>* modelNode = modelsNode->first_node("model");
             modelNode;
             modelNode = modelNode->next_sibling("model")) {

            std::string file = modelNode->first_attribute("file")->value();
            group.models.push_back(file);

            std::string fullPath = modelsDir + "/" + file;
            std::vector<Point> pts = parse3Dfile(fullPath);

            group.points.insert(group.points.end(), pts.begin(), pts.end());
        }
    }

    // child groups
    for (rapidxml::xml_node<>* childNode = groupNode->first_node("group");
         childNode;
         childNode = childNode->next_sibling("group")) {

        group.subgroups.push_back(parseGroup(childNode, modelsDir));
    }

    return group;
}

Configuration parseConfig(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error opening the file: " << filename << std::endl;
        exit(1);
    }

    std::string xmlContent((std::istreambuf_iterator<char>(file)),
                           std::istreambuf_iterator<char>());
    file.close();

    rapidxml::xml_document<> doc;
    doc.parse<0>(&xmlContent[0]);

    rapidxml::xml_node<>* root = doc.first_node("world");

    char* width = root->first_node("window")->first_attribute("width")->value();
    char* height = root->first_node("window")->first_attribute("height")->value();
    Window window_info(std::stoi(width), std::stoi(height));

    rapidxml::xml_node<>* camera = root->first_node("camera");

    rapidxml::xml_node<>* position_n = camera->first_node("position");
    Point position(
        std::stof(position_n->first_attribute("x")->value()),
        std::stof(position_n->first_attribute("y")->value()),
        std::stof(position_n->first_attribute("z")->value())
    );

    rapidxml::xml_node<>* lookAt_n = camera->first_node("lookAt");
    Point lookAt(
        std::stof(lookAt_n->first_attribute("x")->value()),
        std::stof(lookAt_n->first_attribute("y")->value()),
        std::stof(lookAt_n->first_attribute("z")->value())
    );

    rapidxml::xml_node<>* up_n = camera->first_node("up");
    Point up(
        std::stof(up_n->first_attribute("x")->value()),
        std::stof(up_n->first_attribute("y")->value()),
        std::stof(up_n->first_attribute("z")->value())
    );

    rapidxml::xml_node<>* projection = camera->first_node("projection");
    float fov = std::stof(projection->first_attribute("fov")->value());
    float nearP = std::stof(projection->first_attribute("near")->value());
    float farP = std::stof(projection->first_attribute("far")->value());

    Camera camera_info(position, lookAt, up, fov, nearP, farP);

    std::string scenesDir = filename.substr(0, filename.find_last_of("/\\"));
    std::string modelsDir = scenesDir + "/../models";

    rapidxml::xml_node<>* rootGroupNode = root->first_node("group");
    Group rootGroup = parseGroup(rootGroupNode, modelsDir);

    return Configuration(window_info, camera_info, rootGroup);
}