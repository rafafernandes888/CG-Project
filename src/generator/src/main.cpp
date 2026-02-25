#include <iostream>
#include <string>

#include "../include/plane.hpp"
#include "../include/cone.hpp"


void generateFigure(int argc, char* argv[]) {
  if (argc < 5) {
    std::cerr << "Insufficient arguments\n";
    return;
  }

  char* fileName = argv[argc - 1];
  std::string figureName = argv[1];

  if (figureName == "plane" && argc == 5) {
    // Generate Plane
    std::cout << "Generating Plane\n";  // Added newline here
    float length = std::stof(argv[2]);
    int divisions = std::stoi(argv[3]);

    generatePlane(length, divisions,
                  fileName);  // Assuming saveToFile is available
  } else if (figureName == "cone" && argc == 7) {
    // Generate Cone
    std::cout << "Generating Cone\n";
    float radius = std::stof(argv[2]);
    float height = std::stof(argv[3]);
    int slices = std::stoi(argv[4]);
    int stacks = std::stoi(argv[5]);

    generateCone(radius, height, slices, stacks, fileName);
  
  } else {
    std::cerr << "Invalid arguments\n";
  }
}

int main(int argc, char* argv[]) {
  generateFigure(argc, argv);
  return 0;
}