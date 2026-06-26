#include "../include/utils.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

void saveToFile(const std::vector<Point>& points, const std::string& filepath) {
    std::ofstream file(filepath);
    if (file.is_open()) {
        file << points.size() << "\n";
        for (const auto& p : points) {
            file << p.x << " " << p.y << " " << p.z << "\n";
        }
        file.close();
        std::cout << "Sucesso: " << points.size() << " vertices gravados em " << filepath << std::endl;
    } else {
        std::cerr << "Erro: Nao foi possivel criar o ficheiro " << filepath << std::endl;
    }
}

std::vector<Point> parse3Dfile(const std::string& filename) {
    std::vector<Point> points;
    std::ifstream file(filename);
    int numVertices;
    if (file.is_open()) {
        if (file >> numVertices) {
            for (int i = 0; i < numVertices; i++) {
                Point p;
                if (file >> p.x >> p.y >> p.z) {
                    points.push_back(p);
                }
            }
        }
        file.close();
    }
    return points;
}

void saveToFile(const std::vector<Vertex>& vertices, const std::string& filepath) {
    std::ofstream file(filepath);
    if (file.is_open()) {
        file << vertices.size() << "\n";
        for (const auto& v : vertices) {
            file << v.x  << " " << v.y  << " " << v.z  << " "
                 << v.nx << " " << v.ny << " " << v.nz << " "
                 << v.u  << " " << v.v  << "\n";
        }
        file.close();
        std::cout << "Sucesso: " << vertices.size() << " vertices gravados em " << filepath << std::endl;
    } else {
        std::cerr << "Erro: Nao foi possivel criar o ficheiro " << filepath << std::endl;
    }
}

std::vector<Vertex> parse3DfileV2(const std::string& filename) {
    std::vector<Vertex> vertices;
    std::ifstream file(filename);
    if (!file.is_open()) return vertices;

    std::string line;
    if (!std::getline(file, line)) return vertices;
    int numVertices = std::stoi(line);

    for (int i = 0; i < numVertices; i++) {
        if (!std::getline(file, line)) break;
        std::istringstream iss(line);
        Vertex v;

        if (iss >> v.x >> v.y >> v.z >> v.nx >> v.ny >> v.nz >> v.u >> v.v) {
            vertices.push_back(v);
        } else {
            std::istringstream iss2(line);
            if (iss2 >> v.x >> v.y >> v.z) {
                v.nx = 0; v.ny = 1; v.nz = 0;
                v.u = 0;  v.v = 0;
                vertices.push_back(v);
            }
        }
    }

    file.close();
    return vertices;
}