#include "parse.hpp"
#include "../../../libs/rapidxml-1.13/rapidxml.hpp"
#include "../../shared/include/utils.hpp"

#include <IL/il.h>

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <map>

static std::map<std::string, GLuint> textureCache;

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

// Try multiple attribute names (posX vs posx)
static double getAttrFlex(rapidxml::xml_node<>* n, const char* name1, const char* name2, double def) {
    if (auto* a = n->first_attribute(name1)) return std::stod(a->value());
    if (auto* a = n->first_attribute(name2)) return std::stod(a->value());
    return def;
}

// ---- Load texture using DevIL ----

static GLuint loadTexture(const std::string& filepath) {
    auto it = textureCache.find(filepath);
    if (it != textureCache.end()) return it->second;

    ILuint ilImg;
    ilGenImages(1, &ilImg);
    ilBindImage(ilImg);

    if (!ilLoadImage((ILstring)filepath.c_str())) {
        std::cerr << "DevIL: failed to load image: " << filepath << "\n";
        ilDeleteImages(1, &ilImg);
        return 0;
    }

    ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);

    int w = ilGetInteger(IL_IMAGE_WIDTH);
    int h = ilGetInteger(IL_IMAGE_HEIGHT);
    unsigned char* data = ilGetData();

    GLuint texId;
    glGenTextures(1, &texId);
    glBindTexture(GL_TEXTURE_2D, texId);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    ilDeleteImages(1, &ilImg);

    textureCache[filepath] = texId;
    std::cout << "Textura carregada: " << filepath << " (" << w << "x" << h << ")\n";
    return texId;
}

// ---- Create 3 VBOs from Vertex data ----

struct VBOSet {
    GLuint positions;
    GLuint normals;
    GLuint texCoords;
};

static VBOSet createVBOs(const std::vector<Vertex>& verts) {
    VBOSet set = {0, 0, 0};
    if (verts.empty()) return set;

    std::vector<float> pos, nrm, tex;
    pos.reserve(verts.size() * 3);
    nrm.reserve(verts.size() * 3);
    tex.reserve(verts.size() * 2);

    for (const auto& v : verts) {
        pos.push_back(v.x);  pos.push_back(v.y);  pos.push_back(v.z);
        nrm.push_back(v.nx); nrm.push_back(v.ny); nrm.push_back(v.nz);
        tex.push_back(v.u);  tex.push_back(v.v);
    }

    glGenBuffers(1, &set.positions);
    glBindBuffer(GL_ARRAY_BUFFER, set.positions);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * pos.size(), pos.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &set.normals);
    glBindBuffer(GL_ARRAY_BUFFER, set.normals);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * nrm.size(), nrm.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &set.texCoords);
    glBindBuffer(GL_ARRAY_BUFFER, set.texCoords);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * tex.size(), tex.data(), GL_STATIC_DRAW);

    return set;
}

// ---- Parse material ----

static Material parseMaterial(rapidxml::xml_node<>* colorNode) {
    Material mat;
    if (!colorNode) return mat;

    auto* diffNode = colorNode->first_node("diffuse");
    auto* ambNode  = colorNode->first_node("ambient");
    auto* specNode = colorNode->first_node("specular");
    auto* emisNode = colorNode->first_node("emissive");
    auto* shinNode = colorNode->first_node("shininess");

    if (diffNode || ambNode || specNode || emisNode || shinNode) {
        if (diffNode) {
            mat.diffuse[0] = (float)getAttrDouble(diffNode, "R", 200, false) / 255.0f;
            mat.diffuse[1] = (float)getAttrDouble(diffNode, "G", 200, false) / 255.0f;
            mat.diffuse[2] = (float)getAttrDouble(diffNode, "B", 200, false) / 255.0f;
            mat.diffuse[3] = 1.0f;
        }
        if (ambNode) {
            mat.ambient[0] = (float)getAttrDouble(ambNode, "R", 50, false) / 255.0f;
            mat.ambient[1] = (float)getAttrDouble(ambNode, "G", 50, false) / 255.0f;
            mat.ambient[2] = (float)getAttrDouble(ambNode, "B", 50, false) / 255.0f;
            mat.ambient[3] = 1.0f;
        }
        if (specNode) {
            mat.specular[0] = (float)getAttrDouble(specNode, "R", 0, false) / 255.0f;
            mat.specular[1] = (float)getAttrDouble(specNode, "G", 0, false) / 255.0f;
            mat.specular[2] = (float)getAttrDouble(specNode, "B", 0, false) / 255.0f;
            mat.specular[3] = 1.0f;
        }
        if (emisNode) {
            mat.emissive[0] = (float)getAttrDouble(emisNode, "R", 0, false) / 255.0f;
            mat.emissive[1] = (float)getAttrDouble(emisNode, "G", 0, false) / 255.0f;
            mat.emissive[2] = (float)getAttrDouble(emisNode, "B", 0, false) / 255.0f;
            mat.emissive[3] = 1.0f;
        }
        if (shinNode) {
            mat.shininess = (float)getAttrDouble(shinNode, "value", 0, false);
        }
    } else {
        // Phase 3 backwards compat: <color r="..." g="..." b="..." />
        float r = (float)getAttrDouble(colorNode, "r", 255, false) / 255.0f;
        float g = (float)getAttrDouble(colorNode, "g", 255, false) / 255.0f;
        float b = (float)getAttrDouble(colorNode, "b", 255, false) / 255.0f;
        if (colorNode->first_attribute("R")) r = (float)getAttrDouble(colorNode, "R", 255, false) / 255.0f;
        if (colorNode->first_attribute("G")) g = (float)getAttrDouble(colorNode, "G", 255, false) / 255.0f;
        if (colorNode->first_attribute("B")) b = (float)getAttrDouble(colorNode, "B", 255, false) / 255.0f;

        mat.diffuse[0] = r; mat.diffuse[1] = g; mat.diffuse[2] = b; mat.diffuse[3] = 1.0f;
        mat.ambient[0] = r*0.2f; mat.ambient[1] = g*0.2f; mat.ambient[2] = b*0.2f; mat.ambient[3] = 1.0f;
    }

    return mat;
}

// ---- Parse lights ----

static std::vector<Light> parseLights(rapidxml::xml_node<>* lightsNode) {
    std::vector<Light> lights;
    if (!lightsNode) return lights;

    for (rapidxml::xml_node<>* ln = lightsNode->first_node("light"); ln; ln = ln->next_sibling("light")) {
        Light light;
        std::string type = getAttrString(ln, "type", true);

        if (type == "point") {
            light.type = LIGHT_POINT;
            light.posX = (float)getAttrFlex(ln, "posX", "posx", 0);
            light.posY = (float)getAttrFlex(ln, "posY", "posy", 0);
            light.posZ = (float)getAttrFlex(ln, "posZ", "posz", 0);
        }
        else if (type == "directional") {
            light.type = LIGHT_DIRECTIONAL;
            light.dirX = (float)getAttrFlex(ln, "dirX", "dirx", 0);
            light.dirY = (float)getAttrFlex(ln, "dirY", "diry", 0);
            light.dirZ = (float)getAttrFlex(ln, "dirZ", "dirz", 0);
        }
        else if (type == "spot" || type == "spotlight") {
            light.type = LIGHT_SPOTLIGHT;
            light.posX = (float)getAttrFlex(ln, "posX", "posx", 0);
            light.posY = (float)getAttrFlex(ln, "posY", "posy", 0);
            light.posZ = (float)getAttrFlex(ln, "posZ", "posz", 0);
            light.dirX = (float)getAttrFlex(ln, "dirX", "dirx", 0);
            light.dirY = (float)getAttrFlex(ln, "dirY", "diry", 0);
            light.dirZ = (float)getAttrFlex(ln, "dirZ", "dirz", 0);
            light.cutoff = (float)getAttrDouble(ln, "cutoff", 45, false);
        }

        lights.push_back(light);
    }

    return lights;
}

// ---- Parse group ----

Group parseGroup(rapidxml::xml_node<>* groupNode, const std::string& modelsDir, const std::string& texturesDir) {
    Group group;
    if (!groupNode) return group;

    // ---- Parse transforms ----
    rapidxml::xml_node<>* transformNode = groupNode->first_node("transform");
    if (transformNode) {
        for (rapidxml::xml_node<>* node = transformNode->first_node();
             node; node = node->next_sibling()) {

            std::string nodeName = node->name();

            if (nodeName == "translate") {
                auto* timeAttr = node->first_attribute("time");
                if (timeAttr) {
                    Transform t;
                    t.type = TRANSFORM_TRANSLATE_CATMULLROM;
                    t.catmullRom.time = std::stof(timeAttr->value());
                    auto* alignAttr = node->first_attribute("align");
                    if (alignAttr) {
                        std::string alignVal = alignAttr->value();
                        t.catmullRom.align = (alignVal == "True" || alignVal == "true" || alignVal == "1");
                    }
                    for (rapidxml::xml_node<>* pn = node->first_node("point"); pn; pn = pn->next_sibling("point")) {
                        float px = (float)getAttrDouble(pn, "x", 0, true);
                        float py = (float)getAttrDouble(pn, "y", 0, true);
                        float pz = (float)getAttrDouble(pn, "z", 0, true);
                        t.catmullRom.points.push_back(Point(px, py, pz));
                    }
                    if (t.catmullRom.points.size() < 4) {
                        std::cerr << "Warning: Catmull-Rom needs >= 4 points, got "
                                  << t.catmullRom.points.size() << "\n";
                    }
                    group.transforms.push_back(t);
                } else {
                    Transform t;
                    t.type = TRANSFORM_TRANSLATE_STATIC;
                    t.x = (float)getAttrDouble(node, "x", 0, false);
                    t.y = (float)getAttrDouble(node, "y", 0, false);
                    t.z = (float)getAttrDouble(node, "z", 0, false);
                    group.transforms.push_back(t);
                }
            }
            else if (nodeName == "rotate") {
                auto* timeAttr = node->first_attribute("time");
                if (timeAttr) {
                    Transform t;
                    t.type = TRANSFORM_ROTATE_TIME;
                    t.rotTime = std::stof(timeAttr->value());
                    t.x = (float)getAttrDouble(node, "x", 0, true);
                    t.y = (float)getAttrDouble(node, "y", 0, true);
                    t.z = (float)getAttrDouble(node, "z", 0, true);
                    group.transforms.push_back(t);
                } else {
                    Transform t;
                    t.type = TRANSFORM_ROTATE_STATIC;
                    t.angle = (float)getAttrDouble(node, "angle", 0, true);
                    t.x = (float)getAttrDouble(node, "x", 0, true);
                    t.y = (float)getAttrDouble(node, "y", 0, true);
                    t.z = (float)getAttrDouble(node, "z", 0, true);
                    group.transforms.push_back(t);
                }
            }
            else if (nodeName == "scale") {
                Transform t;
                t.type = TRANSFORM_SCALE;
                t.x = (float)getAttrDouble(node, "x", 1, false);
                t.y = (float)getAttrDouble(node, "y", 1, false);
                t.z = (float)getAttrDouble(node, "z", 1, false);
                group.transforms.push_back(t);
            }
        }
    }

    // ---- Parse models ----
    rapidxml::xml_node<>* modelsNode = groupNode->first_node("models");
    if (modelsNode) {
        for (rapidxml::xml_node<>* modelNode = modelsNode->first_node("model");
             modelNode; modelNode = modelNode->next_sibling("model")) {

            std::string file = getAttrString(modelNode, "file", true);
            std::string fullPath = modelsDir + "/" + file;

            std::vector<Vertex> verts = parse3DfileV2(fullPath);

            ModelData md;
            md.filename = file;
            md.vertexCount = (int)verts.size();

            VBOSet vbos = createVBOs(verts);
            md.vboPositions = vbos.positions;
            md.vboNormals   = vbos.normals;
            md.vboTexCoords = vbos.texCoords;

            // Parse texture
            rapidxml::xml_node<>* texNode = modelNode->first_node("texture");
            if (texNode) {
                md.textureFile = getAttrString(texNode, "file", true);
                std::string texPath = texturesDir + "/" + md.textureFile;
                md.textureId = loadTexture(texPath);
            }

            // Parse material
            rapidxml::xml_node<>* colorNode = modelNode->first_node("color");
            md.material = parseMaterial(colorNode);

            group.models.push_back(md);
        }
    }

    // ---- Parse subgroups ----
    for (rapidxml::xml_node<>* childNode = groupNode->first_node("group");
         childNode; childNode = childNode->next_sibling("group")) {
        group.subgroups.push_back(parseGroup(childNode, modelsDir, texturesDir));
    }

    return group;
}

// ---- Main parse entry point ----

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
    if (!root) { std::cerr << "XML error: missing <world>\n"; exit(1); }

    rapidxml::xml_node<>* windowNode = root->first_node("window");
    if (!windowNode) { std::cerr << "XML error: missing <window>\n"; exit(1); }
    Window window_info(
        std::stoi(getAttrString(windowNode, "width", true)),
        std::stoi(getAttrString(windowNode, "height", true))
    );

    rapidxml::xml_node<>* camera = root->first_node("camera");
    if (!camera) { std::cerr << "XML error: missing <camera>\n"; exit(1); }

    auto* position_n = camera->first_node("position");
    auto* lookAt_n   = camera->first_node("lookAt");
    auto* up_n       = camera->first_node("up");
    auto* projection = camera->first_node("projection");

    if (!position_n || !lookAt_n || !up_n || !projection) {
        std::cerr << "XML error: incomplete <camera>\n"; exit(1);
    }

    Point position(stof(getAttrString(position_n,"x",true)),
                   stof(getAttrString(position_n,"y",true)),
                   stof(getAttrString(position_n,"z",true)));
    Point lookAt(stof(getAttrString(lookAt_n,"x",true)),
                 stof(getAttrString(lookAt_n,"y",true)),
                 stof(getAttrString(lookAt_n,"z",true)));
    Point up(stof(getAttrString(up_n,"x",true)),
             stof(getAttrString(up_n,"y",true)),
             stof(getAttrString(up_n,"z",true)));

    int fov    = std::stoi(getAttrString(projection, "fov", true));
    float nearP = std::stof(getAttrString(projection, "near", true));
    float farP  = std::stof(getAttrString(projection, "far", true));

    Camera camera_info(position, lookAt, up, fov, nearP, farP);

    // Lights
    std::vector<Light> lights = parseLights(root->first_node("lights"));

    // Directories
    std::string scenesDir = filename.substr(0, filename.find_last_of("/\\"));
    std::string modelsDir = scenesDir + "/../models";
    std::string texturesDir = scenesDir + "/../textures";

    // Groups
    Group rootGroup;
    for (rapidxml::xml_node<>* groupNode = root->first_node("group");
         groupNode; groupNode = groupNode->next_sibling("group")) {
        rootGroup.subgroups.push_back(parseGroup(groupNode, modelsDir, texturesDir));
    }

    return Configuration(window_info, camera_info, rootGroup, lights);
}