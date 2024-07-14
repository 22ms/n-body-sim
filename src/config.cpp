#include "config.hpp"

// Initial parameters, can later be changed in UI
namespace config {
    namespace simulation {
        const unsigned int N = 16'384; // Numbers of particles, has to be < MAX_N and power of two.
        const unsigned int MAX_N = 524'288; // Numbers of particles for GPU to allocate
        const float TimeScale = 1.0f; // Time scale of the simulation.
        const worldgens::WorldType WorldType = worldgens::WorldType::SPHERE; // Particle initial conditions
    }
    namespace window {
        const unsigned int Width = 1280; // Width of the glfw window
        const unsigned int Height = 720; // Height of the glfw window
        const std::string Title = "N-body simulation, O(n²))";
    }
}
