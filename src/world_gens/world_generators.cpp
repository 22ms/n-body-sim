#include "world_generators.hpp"

namespace worldgens {
    bool WorldGenerator::isSameType (const WorldGenerator& other) const {
        return this->ToString() == other.ToString();
    }

    std::unique_ptr<WorldGenerator> CreateFromWorldType (WorldType worldType) {
        switch (worldType) {
            case WorldType::SPHERE:
                return std::make_unique<SphereGenerator>();
        }
    }
}