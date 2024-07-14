#pragma once

#include "world_gens/world_generators.hpp"

// Declare config, modify somewhere else
namespace config {
    namespace simulation {
        extern const unsigned int N;
        extern const unsigned int MAX_N;
        extern const float TimeScale;
        extern const worldgens::WorldType WorldType;
    }
    namespace window {
        const unsigned int Width;
        const unsigned int Height;
    }
}