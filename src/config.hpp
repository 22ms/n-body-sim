#pragma once

#include <string>

namespace worldgens {
    enum class WorldType;
}

// Declare config, modify somewhere else
namespace config {
    namespace simulation {
        extern const unsigned int N;
        extern const unsigned int MAX_N;
        extern const float TimeScale;
        extern const worldgens::WorldType WorldType;
    }
    namespace window {
        extern const unsigned int Width;
        extern const unsigned int Height;
        extern const std::string Title;
    }
}