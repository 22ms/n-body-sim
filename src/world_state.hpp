#pragma once

#include <memory>
#include <vector>
#include <string>

namespace worldgens { class WorldGenerator; }

namespace worldstate {

    void Initialize();
    void Cleanup();

    extern std::unique_ptr<unsigned int> CurrentNPtr;
    extern std::unique_ptr<float> CurrentTimeScalePtr;
    extern std::unique_ptr<float> MainCameraSpeedPtr;
    extern std::unique_ptr<worldgens::WorldGenerator> CurrentWorldGeneratorPtr;
    extern std::vector<std::unique_ptr<worldgens::WorldGenerator>> WorldGeneratorOptions;
}