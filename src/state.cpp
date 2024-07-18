#include <cmath>
#include <stdio.h>
#include <memory>
#include <cstdlib>

#include "state.hpp"
#include "world_gens/world_generators.hpp"

// Initial parameters, can later be changed in UI.
namespace state {
    namespace simulation {
        // Numbers of particles for GPU to allocate.
        const unsigned int MAX_N = 524'288;
        // Numbers of particles, has to be < MAX_N and power of two.
        std::unique_ptr<unsigned int> NPtr = std::make_unique<unsigned int>(16'384);
        // Time scale of the simulation.
        std::unique_ptr<float> TimeScalePtr = std::make_unique<float>(1.0f);
        // Particle conditions.
        std::unique_ptr<worldgens::WorldGenerator> WorldGeneratorPtr = std::make_unique<worldgens::GalaxyGenerator>(); // <-- Change here
    }
    namespace rendering {
        // Movement speed of main camera.
        std::unique_ptr<float> MainCameraSpeedPtr = std::make_unique<float>(1.0f);
        // Rendered size of the particles.
        std::unique_ptr<float> PointSizePtr = std::make_unique<float>(2.0f);
    }
    namespace window {
        // Title of the window.
        const std::string TITLE = "N-body simulation, O(n²) complexity";
        // Width of the window.
        const unsigned int WIDTH = 1280;
        // Height of the window.
        const unsigned int HEIGHT = 720;
    }
}