#include <cmath>
#include <random>

#include "../config.hpp"
#include "../utilities.hpp"
#include "world_generators.hpp"

namespace worldgens {

    void SphereShellGenerator::Generate(std::vector<utilities::Position>& positions, std::vector<utilities::Velocity>& velocities, int n) {
        WorldGenerator::Generate(positions, velocities, n);

        float endRadius = 1.0f;
        float spacing = endRadius / n;

        // random generator
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> dist(0.0, 1.0);
        std::uniform_real_distribution<> distPhi(0.0, 2 * M_PI);

        // set positions
        for (int i = 0; i < n; i++) {
            double theta = acos(2 * dist(gen) - 1); // Polar angle
            double phi = distPhi(gen); // Azimuthal angle
            double radius = endRadius; // Place all particles on the shell of the sphere

            positions[i].x = radius * sin(theta) * cos(phi);
            positions[i].y = radius * sin(theta) * sin(phi);
            positions[i].z = radius * cos(theta);
            positions[i].m = 1;
        }

        // set velocities
        for (int i = 0; i < n; i++) {
            velocities[i].x = 0.0f;
            velocities[i].y = 0.0f;
            velocities[i].z = 0.0f;
        }
    }

    std::string SphereShellGenerator::ToString() const {
        return "SPHERE_SHELL";
    }

    std::unique_ptr<WorldGenerator> SphereShellGenerator::Clone() const {
        return std::make_unique<SphereShellGenerator>(*this);
    }

}