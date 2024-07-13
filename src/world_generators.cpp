#include <cmath>
#include <stdio.h>
#include <vector>
#include <cstdlib>

#include "globals.hpp"
#include "utilities.hpp"
#include "world_generators.hpp"

namespace worldgenerators {

    std::vector<WorldGenerator*> WorldGeneratorOptions;

    void Initialize(WorldGenerator* initialWorldGenerator) {
        WorldGeneratorOptions.push_back(new SphereGenerator());
        WorldGeneratorOptions.push_back(new SphereShellGenerator());
        WorldGeneratorOptions.push_back(new BlackHoleSphereGenerator());
        WorldGeneratorOptions.push_back(new TwoSpheresGenerator());

        // Initialize the options, placing initialWorldGenerator in front.
        bool found = false;
        for (size_t i = 0; i < WorldGeneratorOptions.size(); ++i) {
            if (WorldGeneratorOptions[i]->isSameType(initialWorldGenerator)) {
                std::swap(WorldGeneratorOptions[i], WorldGeneratorOptions[0]);
                found = true;
                break;
            }
        }

        if (!found) {
            WorldGeneratorOptions.insert(WorldGeneratorOptions.begin(), initialWorldGenerator);
        } 
    }
}