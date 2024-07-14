#include "world_generators.hpp"

namespace worldgens {
    bool WorldGenerator::isSameType (const WorldGenerator& other) const {
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