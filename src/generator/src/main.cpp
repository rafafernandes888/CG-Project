#include <iostream>
#include <string>

#include "../include/plane.hpp"
#include "../include/sphere.hpp"
#include "../include/box.hpp"
#include "../include/cone.hpp"
#include "../include/bezier.hpp"

void generateFigure(int argc, char* argv[]) {
    if (argc < 5) {
        std::cerr << "Insufficient arguments\n";
        return;
    }

    char* fileName = argv[argc - 1];
    std::string figureName = argv[1];

    if (figureName == "plane" && argc == 5) {
        std::cout << "A gerar Plano...\n";
        generatePlane(std::stof(argv[2]), std::stoi(argv[3]), fileName);

    } else if (figureName == "sphere" && argc == 6) {
        std::cout << "A gerar Esfera...\n";
        // sphere <radius> <slices> <stacks> <file>
        generateSphere(std::stof(argv[2]), std::stoi(argv[3]), std::stoi(argv[4]), fileName);

    } else if (figureName == "box" && argc == 5) {
        std::cout << "A gerar Caixa...\n";
        generateBox(std::stof(argv[2]), std::stoi(argv[3]), fileName);

    } else if (figureName == "cone" && argc == 7) {
        std::cout << "A gerar Cone...\n";
        // cone <radius> <height> <slices> <stacks> <file>
        generateCone(std::stof(argv[2]), std::stof(argv[3]), std::stoi(argv[4]), std::stoi(argv[5]), fileName);

    } else if (figureName == "bezier" && argc == 5) {
        std::cout << "A gerar Bezier Patch...\n";
        // bezier <patch_file> <tessellation> <output_file>
        generateBezier(argv[2], std::stoi(argv[3]), fileName);

    } else {
        std::cerr << "Invalid arguments\n";
        std::cerr << "  generator plane <length> <divisions> <file>\n";
        std::cerr << "  generator sphere <radius> <slices> <stacks> <file>\n";
        std::cerr << "  generator box <length> <divisions> <file>\n";
        std::cerr << "  generator cone <radius> <height> <slices> <stacks> <file>\n";
        std::cerr << "  generator bezier <patch_file> <tessellation> <output_file>\n";
    }
}

int main(int argc, char* argv[]) {
    generateFigure(argc, argv);
    return 0;
}