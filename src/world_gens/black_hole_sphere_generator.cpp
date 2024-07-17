#include <cmath>
#include <random>

#include "../config.hpp"
#include "../utilities.hpp"
#include "world_generators.hpp"

namespace worldgens {

    void BlackHoleSphereGenerator::Generate(float*& particleArray, int n) {
        WorldGenerator::Generate(particleArray, n);

        float endRadius = 1.0f;
        float spacing = endRadius / n;

        // random generator
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> dist(0.0, 1.0);
        std::uniform_real_distribution<> distPhi(0.0, 2 * M_PI);

        // set first particle to be heavier than the others
        particleArray[0] = 0.0f;
        particleArray[1] = 0.0f;
        particleArray[2] = 0.0f;
        particleArray[3] = 2.0f;

        // set positions
        for (int i = 7; i < n; i += 7) {
            double theta = acos(2 * dist(gen) - 1); // Polar angle
            double phi = distPhi(gen); // Azimuthal angle
            double radius = dist(gen) * endRadius; // Radius of the sphere

            particleArray[i] = radius * sin(theta) * cos(phi); // x
            particleArray[i+1] = radius * sin(theta) * sin(phi); // y
            particleArray[i+2] = radius * cos(theta); // z
            particleArray[i+3] = 1; // m
        }

        // set velocities
        for (int i = 4; i < n; i += 7) {
            particleArray[i] = 0.0f; // vx
            particleArray[i+2] = 0.0f; // vy
            particleArray[i+3] = 0.0f; // vz
        }
    }

    std::string BlackHoleSphereGenerator::ToString() const {
        return "BLACK_HOLE_SPHERE";
    }

    std::unique_ptr<WorldGenerator> BlackHoleSphereGenerator::Clone() const {
        return std::make_unique<BlackHoleSphereGenerator>(*this);
    }
}