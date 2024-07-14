#include <cmath>
#include <stdio.h>
#include <memory>
#include <cstdlib>

#include "globals.hpp"
#include "utilities.hpp"
#include "world_generators.hpp"

namespace worldgens {

    bool WorldGenerator::isSameType (const WorldGenerator& other) const {
        return this->ToString() == other.ToString();
    }

    std::vector<std::unique_ptr<WorldGenerator>> WorldGeneratorOptions;
    std::unique_ptr<WorldGenerator> CurrentWorldGenerator;

    void Initialize(WorldGenerator& initialWorldGenerator) {
        CurrentWorldGenerator = initialWorldGenerator.clone();

        WorldGeneratorOptions.push_back(std::make_unique<SphereGenerator>());
        WorldGeneratorOptions.push_back(std::make_unique<SphereShellGenerator>());
        WorldGeneratorOptions.push_back(std::make_unique<BlackHoleSphereGenerator>());
        WorldGeneratorOptions.push_back(std::make_unique<TwoSpheresGenerator>());
    }

    void Cleanup () {
        WorldGeneratorOptions.clear();
    }
}