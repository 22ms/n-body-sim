#include <cmath>
#include <random>

#include "world_generators.hpp"
#include "../state.hpp"

namespace worldgens {

    void GalaxyGenerator::Generate(float*& particleArray, int n) {
        WorldGenerator::Generate(particleArray, n);

        float endRadius = 1.0f;
        float spacing = endRadius / n;

        // random generator
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> distZ(0.0, 0.1);
        std::uniform_real_distribution<> distRho(0.7, 1.0);
        std::uniform_real_distribution<> distPhi(0.0, 2 * M_PI);

        // set first particle to be heavier than the others
        particleArray[0] = 0.0f; // x
        particleArray[1] = 0.0f; // y
        particleArray[2] = 0.0f; // z
        particleArray[3] = 40000.0f; // m

        // set positions
        for (int i = 7; i < n * 7; i += 7) {
            double phi = distPhi(gen); // Azimuthal angle
            double rho = distRho(gen) * endRadius; // Radius of the sphere

            particleArray[i] = rho * cos(phi); // x
            particleArray[i+1] = rho * sin(phi); // y
            particleArray[i+2] = distZ(gen); // z
            particleArray[i+3] = 1.0f; // m
        }

        // set velocities to give a spinning motion
        for (int i = 11; i < n * 7; i += 7) {
            // Radial vector
            float rx = particleArray[i-4];
            float ry = particleArray[i-3];
            float rz = particleArray[i-2];

            // Tangential vector (cross product with an arbitrary vector, e.g., (0,0,1))
            // Ensure it's not parallel to the radial vector to avoid zero vector
            float vx, vy, vz;
            if (rx != 0 || ry != 0) {
                vx = -ry;
                vy = rx;
                vz = 0;
            } else {
                vx = 0;
                vy = rz;
                vz = -ry;
            }

            // Normalize the tangential vector, minimum distance so we dont explode
            float length = sqrt(vx * vx + vy * vy + vz * vz) + 0.0001;
            vx /= length;
            vy /= length;
            vz /= length;

            // Scale the velocity vector to the desired speed
            float speed = 200.0f; // Adjust this value as needed
            particleArray[i]   = vx * speed;
            particleArray[i+1] = vy * speed;
            particleArray[i+2] = vz * speed;
        }
    }

    std::string GalaxyGenerator::ToString() const {
        return "GALAXY";
    }

    std::unique_ptr<WorldGenerator> GalaxyGenerator::Clone() const {
        return std::make_unique<GalaxyGenerator>(*this);
    }
}