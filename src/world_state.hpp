#pragma once

#include <memory>
#include <vector>
#include <string>

namespace worldgens { class WorldGenerator; }

namespace worldstate {

    void Initialize();
    void Cleanup();

    extern std::unique_ptr<unsigned int> CurrentN;
    extern std::unique_ptr<worldgens::WorldGenerator> CurrentWorldGenerator;
    extern std::vector<std::unique_ptr<worldgens::WorldGenerator>> WorldGeneratorOptions;
}