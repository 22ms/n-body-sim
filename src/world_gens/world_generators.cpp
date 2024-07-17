#include "world_generators.hpp"
#include "../state.hpp"

namespace worldgens {
    std::unique_ptr<WorldGenerator> FromString (std::string str) {
        if (str == "SPHERE") {
            return std::make_unique<SphereGenerator>();
        }
        else if (str == "SPHERE_SHELL") {
            return std::make_unique<SphereShellGenerator>();
        }
        else if (str == "TWO_SPHERES") {
            return std::make_unique<TwoSpheresGenerator>();
        }
        else if (str == "BLACK_HOLE_SPHERE") {
            return std::make_unique<BlackHoleSphereGenerator>();
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
    }

    bool WorldGenerator::IsSameType (const WorldGenerator& other) const {
        return this->ToString() == other.ToString();
    }
}