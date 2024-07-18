#include "world_generators.hpp"
#include "../state.hpp"

namespace worldgens {
    std::unique_ptr<WorldGenerator> FromString (std::string str) {
        if (str == "GALAXY") {
            return std::make_unique<GalaxyGenerator>();
        }
        else if (str == "TWO_GALAXIES") {
            return std::make_unique<TwoGalaxiesGenerator>();
        }
        else if (str == "SPHERE") {
            return std::make_unique<SphereGenerator>();
        }
        else if (str == "SPHERE_SHELL") {
            return std::make_unique<SphereShellGenerator>();
        }
        else if (str == "TWO_SPHERES") {
            return std::make_unique<TwoSpheresGenerator>();
        }
        else {
            return std::make_unique<SphereGenerator>();
        }
    }

    void WorldGenerator::Generate(float*& particleArray, int n) {
        if (particleArray) {
            delete[] particleArray;
        }
        particleArray = new float[state::simulation::MAX_N * (4 + 3)];
        for (int i = 0; i < state::simulation::MAX_N * 7; i++) {
            particleArray[i] = 0;
        }
    }

    bool WorldGenerator::IsSameType (const WorldGenerator& other) const {
        return this->ToString() == other.ToString();
    }
}