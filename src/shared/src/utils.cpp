#include "../include/utils.hpp"
#include <fstream>
#include <iostream>

void saveToFile(const std::vector<Point>& points, std::string filepath) {
    std::ofstream file(filepath);

    if (file.is_open()) {
        // Cabeçalho: número total de vértices (ajuda muito na Fase 1) 
        file << points.size() << "\n"; 
        
        for (const auto& point : points) {
            // Escreve cada vértice: x y z [cite: 11]
            file << point.x << " " << point.y << " " << point.z << "\n";
        }
        file.close();
        std::cout << "Sucesso: " << points.size() << " vertices gravados em " << filepath << std::endl;
    } else {
        std::cerr << "Erro: Nao foi possivel criar o ficheiro " << filepath << std::endl;
    }
}

std::vector<Point> parse3Dfile(std::string filename) {
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