#include "parse.hpp"
#include "../../../libs/rapidxml-1.13/rapidxml.hpp"
#include "../../src/shared/include/utils.hpp"

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

static double getAttrDouble(rapidxml::xml_node<>* n, const char* attrName, double def, bool required) {
    if (auto* a = n->first_attribute(attrName)) return std::stod(a->value());
    if (required) {
        std::cerr << "XML error: missing attribute '" << attrName
                  << "' in <" << n->name() << ">\n";
        exit(1);
    }
    return def;
}

static std::string getAttrString(rapidxml::xml_node<>* n, const char* attrName, bool required) {
    if (auto* a = n->first_attribute(attrName)) return std::string(a->value());
    if (required) {
        std::cerr << "XML error: missing attribute '" << attrName
                  << "' in <" << n->name() << ">\n";
        exit(1);
    }
    return "";
}

Group parseGroup(rapidxml::xml_node<>* groupNode, const std::string& modelsDir) {
    Group group;

    if (!groupNode) return group;

    rapidxml::xml_node<>* transformNode = groupNode->first_node("transform");
    if (transformNode) {
        bool seenTranslate = false;
        bool seenRotate = false;
        bool seenScale = false;

        for (rapidxml::xml_node<>* node = transformNode->first_node();
             node;
             node = node->next_sibling()) {

            std::string nodeName = node->name();

            if (nodeName == "translate") {
                if (seenTranslate) {
                    std::cerr << "XML error: duplicate <translate> in the same <transform>\n";
                    exit(1);
                }
                seenTranslate = true;

                double x = getAttrDouble(node, "x", 0.0, false);
                double y = getAttrDouble(node, "y", 0.0, false);
                double z = getAttrDouble(node, "z", 0.0, false);
                group.translate(x, y, z);
            }
            else if (nodeName == "rotate") {
                if (seenRotate) {
                    std::cerr << "XML error: duplicate <rotate> in the same <transform>\n";
                    exit(1);
                }
                seenRotate = true;

                double angle = getAttrDouble(node, "angle", 0.0, true);
                double x = getAttrDouble(node, "x", 0.0, true);
                double y = getAttrDouble(node, "y", 0.0, true);
                double z = getAttrDouble(node, "z", 0.0, true);

                if (x == 0.0 && y == 0.0 && z == 0.0) {
                    std::cerr << "XML error: rotate axis is (0,0,0)\n";
                    exit(1);
                }

                group.rotate(angle, x, y, z);
            }
            else if (nodeName == "scale") {
                if (seenScale) {
                    std::cerr << "XML error: duplicate <scale> in the same <transform>\n";
                    exit(1);
                }
                seenScale = true;

                double x = getAttrDouble(node, "x", 1.0, false);
                double y = getAttrDouble(node, "y", 1.0, false);
                double z = getAttrDouble(node, "z", 1.0, false);
                group.scale(x, y, z);
            }
            else {
                std::cerr << "Warning: unknown transform node <" << nodeName << "> ignored\n";
            }
        }
    }

    rapidxml::xml_node<>* modelsNode = groupNode->first_node("models");
    if (modelsNode) {
        for (rapidxml::xml_node<>* modelNode = modelsNode->first_node("model");
             modelNode;
             modelNode = modelNode->next_sibling("model")) {

            std::string file = getAttrString(modelNode, "file", true);
            group.models.push_back(file);

            std::string fullPath = modelsDir + "/" + file;
            std::vector<Point> pts = parse3Dfile(fullPath);
            group.points.insert(group.points.end(), pts.begin(), pts.end());
        }
    }

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

    if (xmlContent.empty()) {
        std::cerr << "XML error: empty file\n";
        exit(1);
    }

    rapidxml::xml_document<> doc;
    try {
        doc.parse<0>(&xmlContent[0]);
    } catch (const rapidxml::parse_error& e) {
        std::cerr << "XML parse error: " << e.what() << std::endl;
        exit(1);
    }

    rapidxml::xml_node<>* root = doc.first_node("world");
    if (!root) {
        std::cerr << "XML error: missing <world>\n";
        exit(1);
    }

    rapidxml::xml_node<>* windowNode = root->first_node("window");
    if (!windowNode) {
        std::cerr << "XML error: missing <window>\n";
        exit(1);
    }

    Window window_info(
        std::stoi(getAttrString(windowNode, "width", true)),
        std::stoi(getAttrString(windowNode, "height", true))
    );

    rapidxml::xml_node<>* camera = root->first_node("camera");
    if (!camera) {
        std::cerr << "XML error: missing <camera>\n";
        exit(1);
    }

    rapidxml::xml_node<>* position_n = camera->first_node("position");
    rapidxml::xml_node<>* lookAt_n = camera->first_node("lookAt");
    rapidxml::xml_node<>* up_n = camera->first_node("up");
    rapidxml::xml_node<>* projection = camera->first_node("projection");

    if (!position_n || !lookAt_n || !up_n || !projection) {
        std::cerr << "XML error: incomplete <camera>\n";
        exit(1);
    }

    Point position(
        std::stof(getAttrString(position_n, "x", true)),
        std::stof(getAttrString(position_n, "y", true)),
        std::stof(getAttrString(position_n, "z", true))
    );

    Point lookAt(
        std::stof(getAttrString(lookAt_n, "x", true)),
        std::stof(getAttrString(lookAt_n, "y", true)),
        std::stof(getAttrString(lookAt_n, "z", true))
    );

    Point up(
        std::stof(getAttrString(up_n, "x", true)),
        std::stof(getAttrString(up_n, "y", true)),
        std::stof(getAttrString(up_n, "z", true))
    );

    float fov = std::stof(getAttrString(projection, "fov", true));
    float nearP = std::stof(getAttrString(projection, "near", true));
    float farP = std::stof(getAttrString(projection, "far", true));

    Camera camera_info(position, lookAt, up, fov, nearP, farP);

    std::string scenesDir = filename.substr(0, filename.find_last_of("/\\"));
    std::string modelsDir = scenesDir + "/../models";

    Group rootGroup;
    for (rapidxml::xml_node<>* groupNode = root->first_node("group");
         groupNode;
         groupNode = groupNode->next_sibling("group")) {
        rootGroup.subgroups.push_back(parseGroup(groupNode, modelsDir));
    }

    return Configuration(window_info, camera_info, rootGroup);
}