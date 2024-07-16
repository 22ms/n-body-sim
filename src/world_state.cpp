#include <cmath>
#include <stdio.h>
#include <memory>
#include <cstdlib>

#include "world_state.hpp"
#include "config.hpp"
#include "world_gens/world_generators.hpp"

namespace worldstate {

    std::unique_ptr<unsigned int> CurrentNPtr;
    std::unique_ptr<float> CurrentTimeScalePtr;
    std::unique_ptr<float> MainCameraSpeedPtr;
    std::unique_ptr<worldgens::WorldGenerator> CurrentWorldGeneratorPtr;
    std::vector<std::unique_ptr<worldgens::WorldGenerator>> WorldGeneratorOptions;

    void Initialize() {
        CurrentNPtr = std::make_unique<unsigned int>(config::simulation::N);
        CurrentTimeScalePtr = std::make_unique<float>(config::simulation::TimeScale);
        MainCameraSpeedPtr = std::make_unique<float>(1.0f);
        CurrentWorldGeneratorPtr = worldgens::CreateFromWorldType(config::simulation::WorldType);

        WorldGeneratorOptions.push_back(std::make_unique<worldgens::SphereGenerator>());
        WorldGeneratorOptions.push_back(std::make_unique<worldgens::SphereShellGenerator>());
        WorldGeneratorOptions.push_back(std::make_unique<worldgens::BlackHoleSphereGenerator>());
        WorldGeneratorOptions.push_back(std::make_unique<worldgens::TwoSpheresGenerator>());
    }

    void Cleanup () {
        WorldGeneratorOptions.clear();
    }
}