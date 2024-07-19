#include <cmath>
#include <stdio.h>
#include <memory>
#include <cstdlib>

#include "state.hpp"
#include "world_gens/world_generators.hpp"

// Initial parameters, can later be changed in UI.
// Note for the definition of the units:
// 1 OpenGL length is set to be 50,000 Lightyears.
// Thus, 1 OpenGL time unit = the time it takes light to travel 50,000 light-years = 50,000 years
// Unless otherwise specified, all units in the code are in OpenGL units.
namespace state {
    namespace simulation {
        // Numbers of particles for GPU to allocate.
        const unsigned int MAX_N = 524'288;
        // Numbers of particles, has to be < MAX_N and power of two.
        std::unique_ptr<unsigned int> NPtr = std::make_unique<unsigned int>(16'384);
        // Time step (in y) of the simulation.
        std::unique_ptr<float> TimeStepPtr = std::make_unique<float>(5.0f);
        // Minimum distance (in ly) to avoid extreme velocities.
        std::unique_ptr<float> EpsilonPtr = std::make_unique<float>(5.0f);
        // Total time (in My) elapsed since beginning of simulation.
        std::unique_ptr<float> ElapsedTimePtr = std::make_unique<float>(0.0f);
        // Particle conditions.
        std::unique_ptr<worldgens::WorldGenerator> WorldGeneratorPtr = std::make_unique<worldgens::TwoGalaxiesGenerator>(); // <-- Change here
    }
    namespace rendering {
        // Rendered size of the particles.
        std::unique_ptr<float> PointSizePtr = std::make_unique<float>(2.0f);
        // Movement speed of main camera.
        std::unique_ptr<float> MainCameraSpeedPtr = std::make_unique<float>(1.0f);
        // Movement speed of main camera.
        std::unique_ptr<float> MouseSensitivityPtr = std::make_unique<float>(0.1f);
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