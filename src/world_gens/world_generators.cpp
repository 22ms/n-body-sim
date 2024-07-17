#include "world_generators.hpp"
#include "../utilities.hpp"
#include "../config.hpp"

namespace worldgens {
    void WorldGenerator::Generate(float*& particleArray, int n) {
        if (particleArray) {
            delete[] particleArray;
        }
        particleArray = new float[config::simulation::MAX_N * (4 + 3)];
    }

    bool WorldGenerator::IsSameType (const WorldGenerator& other) const {
        return this->ToString() == other.ToString();
    }

    std::unique_ptr<WorldGenerator> CreateFromWorldType (WorldType worldType) {
        switch (worldType) {
            case WorldType::SPHERE:
                return std::make_unique<SphereGenerator>();
            case WorldType::SPHERE_SHELL:
                return std::make_unique<SphereShellGenerator>();
            case WorldType::TWO_SPHERES:
                return std::make_unique<TwoSpheresGenerator>();
            case WorldType::BLACK_HOLE_SPHERE:
                return std::make_unique<BlackHoleSphereGenerator>();
            // Add new generators here
            default:
                return std::make_unique<SphereGenerator>();
        }
    }
}