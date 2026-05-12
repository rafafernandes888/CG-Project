#include "parse.hpp"
#include "../../../libs/rapidxml-1.13/rapidxml.hpp"
#include "../../shared/include/utils.hpp"

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

// Create a VBO from a vector of points, return the OpenGL buffer ID
static GLuint createVBO(const std::vector<Point>& pts) {
    if (pts.empty()) return 0;

    // Build a flat float array
    std::vector<float> data;
    data.reserve(pts.size() * 3);
    for (const auto& p : pts) {
        data.push_back(p.x);
        data.push_back(p.y);
        data.push_back(p.z);
    }

    GLuint bufferId;
    glGenBuffers(1, &bufferId);
    glBindBuffer(GL_ARRAY_BUFFER, bufferId);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * data.size(), data.data(), GL_STATIC_DRAW);

    return bufferId;
}

Group parseGroup(rapidxml::xml_node<>* groupNode, const std::string& modelsDir) {
    Group group;
    if (!groupNode) return group;

    // ---- Parse transforms ----
    rapidxml::xml_node<>* transformNode = groupNode->first_node("transform");
    if (transformNode) {
        for (rapidxml::xml_node<>* node = transformNode->first_node();
             node;
             node = node->next_sibling()) {

            std::string nodeName = node->name();

            if (nodeName == "translate") {
                // Check if it's time-based (Catmull-Rom) or static
                auto* timeAttr = node->first_attribute("time");
                if (timeAttr) {
                    // Catmull-Rom translate
                    Transform t;
                    t.type = TRANSFORM_TRANSLATE_CATMULLROM;
                    t.catmullRom.time = std::stof(timeAttr->value());

                    auto* alignAttr = node->first_attribute("align");
                    if (alignAttr) {
                        std::string alignVal = alignAttr->value();
                        t.catmullRom.align = (alignVal == "True" || alignVal == "true" || alignVal == "1");
                    }

                    // Read control points
                    for (rapidxml::xml_node<>* pointNode = node->first_node("point");
                         pointNode;
                         pointNode = pointNode->next_sibling("point")) {
                        float px = (float)getAttrDouble(pointNode, "x", 0.0, true);
                        float py = (float)getAttrDouble(pointNode, "y", 0.0, true);
                        float pz = (float)getAttrDouble(pointNode, "z", 0.0, true);
                        t.catmullRom.points.push_back(Point(px, py, pz));
                    }

                    if (t.catmullRom.points.size() < 4) {
                        std::cerr << "Warning: Catmull-Rom translate needs at least 4 points, got "
                                  << t.catmullRom.points.size() << "\n";
                    }

                    group.transforms.push_back(t);
                } else {
                    // Static translate
                    Transform t;
                    t.type = TRANSFORM_TRANSLATE_STATIC;
                    t.x = (float)getAttrDouble(node, "x", 0.0, false);
                    t.y = (float)getAttrDouble(node, "y", 0.0, false);
                    t.z = (float)getAttrDouble(node, "z", 0.0, false);
                    group.transforms.push_back(t);
                }
            }
            else if (nodeName == "rotate") {
                auto* timeAttr = node->first_attribute("time");
                if (timeAttr) {
                    // Time-based rotation (360 degrees in 'time' seconds)
                    Transform t;
                    t.type = TRANSFORM_ROTATE_TIME;
                    t.rotTime = std::stof(timeAttr->value());
                    t.x = (float)getAttrDouble(node, "x", 0.0, true);
                    t.y = (float)getAttrDouble(node, "y", 0.0, true);
                    t.z = (float)getAttrDouble(node, "z", 0.0, true);
                    group.transforms.push_back(t);
                } else {
                    // Static rotation
                    Transform t;
                    t.type = TRANSFORM_ROTATE_STATIC;
                    t.angle = (float)getAttrDouble(node, "angle", 0.0, true);
                    t.x = (float)getAttrDouble(node, "x", 0.0, true);
                    t.y = (float)getAttrDouble(node, "y", 0.0, true);
                    t.z = (float)getAttrDouble(node, "z", 0.0, true);
                    group.transforms.push_back(t);
                }
            }
            else if (nodeName == "scale") {
                Transform t;
                t.type = TRANSFORM_SCALE;
                t.x = (float)getAttrDouble(node, "x", 1.0, false);
                t.y = (float)getAttrDouble(node, "y", 1.0, false);
                t.z = (float)getAttrDouble(node, "z", 1.0, false);
                group.transforms.push_back(t);
            }
            else {
                std::cerr << "Warning: unknown transform node <" << nodeName << "> ignored\n";
            }
        }
    }

    // ---- Parse models (load into VBOs) ----
    rapidxml::xml_node<>* modelsNode = groupNode->first_node("models");
    if (modelsNode) {
        for (rapidxml::xml_node<>* modelNode = modelsNode->first_node("model");
             modelNode;
             modelNode = modelNode->next_sibling("model")) {

            std::string file = getAttrString(modelNode, "file", true);
            std::string fullPath = modelsDir + "/" + file;

            std::vector<Point> pts = parse3Dfile(fullPath);

            ModelData md;
            md.filename = file;
            md.vertexCount = (int)pts.size();
            md.vboId = createVBO(pts);

            // Read optional color
            rapidxml::xml_node<>* colorNode = modelNode->first_node("color");
            if (colorNode) {
                // Read either r/g/b (0-255)
                md.r = getAttrDouble(colorNode, "r", 255.0, false) / 255.0f;
                md.g = getAttrDouble(colorNode, "g", 255.0, false) / 255.0f;
                md.b = getAttrDouble(colorNode, "b", 255.0, false) / 255.0f;
                
                // Also check capital R/G/B just in case
                if (colorNode->first_attribute("R")) md.r = getAttrDouble(colorNode, "R", 255.0, false) / 255.0f;
                if (colorNode->first_attribute("G")) md.g = getAttrDouble(colorNode, "G", 255.0, false) / 255.0f;
                if (colorNode->first_attribute("B")) md.b = getAttrDouble(colorNode, "B", 255.0, false) / 255.0f;
            }

            group.models.push_back(md);
        }
    }

    // ---- Parse subgroups ----
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

    int fov = std::stoi(getAttrString(projection, "fov", true));
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