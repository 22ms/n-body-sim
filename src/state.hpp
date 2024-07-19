#pragma once

#include <memory>
#include <vector>
#include <string>

namespace worldgens { class WorldGenerator; }

// Modify config in state.cpp.
namespace state {
    namespace simulation {
        extern const unsigned int MAX_N;
        extern std::unique_ptr<unsigned int> NPtr;
        extern std::unique_ptr<float> TimeStepPtr;
        extern std::unique_ptr<float> EpsilonPtr;
        extern std::unique_ptr<float> ElapsedTimePtr;
        extern std::unique_ptr<worldgens::WorldGenerator> WorldGeneratorPtr;
    }
    namespace rendering {
        extern std::unique_ptr<float> PointSizePtr;
        extern std::unique_ptr<float> MouseSensitivityPtr;
    }
    namespace window {
        extern const std::string TITLE;
        extern const unsigned int WIDTH;
        extern const unsigned int HEIGHT;
    }
}