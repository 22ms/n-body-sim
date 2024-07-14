#include <cmath>
#include <stdio.h>
#include <memory>
#include <cstdlib>

#include "config.hpp"
#include "utilities.hpp"
#include "world_generators.hpp"

namespace worldstate {

    std::unique_ptr<unsigned int> CurrentN;
    std::unique_ptr<worldgens::WorldGenerator> CurrentWorldGenerator;
    std::vector<std::unique_ptr<worldgens::WorldGenerator>> WorldGeneratorOptions;

    void Initialize() {
        CurrentN = std::make_unique<unsigned int>(config::simulation::N);
        CurrentWorldGenerator = initialWorldGenerator.clone();

        WorldGeneratorOptions.push_back(std::make_unique<worldgens::SphereGenerator>());
        WorldGeneratorOptions.push_back(std::make_unique<worldgens::SphereShellGenerator>());
        WorldGeneratorOptions.push_back(std::make_unique<worldgens::BlackHoleSphereGenerator>());
        WorldGeneratorOptions.push_back(std::make_unique<worldgens::TwoSpheresGenerator>());
    }

    void Cleanup () {
        WorldGeneratorOptions.clear();
    }
}