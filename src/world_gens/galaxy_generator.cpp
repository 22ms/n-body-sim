#include <cmath>
#include <random>

#include "world_generators.hpp"
#include "../state.hpp"

namespace worldgens {

    void GalaxyGenerator::Generate(float*& particleArray, unsigned int n) {
        WorldGenerator::Generate(particleArray, n);

        float endRadius = 1.0f;
        float spacing = endRadius / n;

        // random generator
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> distZ(0.0f, 0.1f);
        std::uniform_real_distribution<float> distRho(0.7f, 1.0f);
        std::uniform_real_distribution<float> distPhi(0.0f, 2.0f * static_cast<float>(M_PI));

        // set first particle to be heavier than the others
        particleArray[0] = 0.0f; // x
        particleArray[1] = 0.0f; // y
        particleArray[2] = 0.0f; // z
        particleArray[3] = 6e14f * G; // m in kg * G

        // set positions
        for (unsigned int i = 7; i < n * 7; i += 7) {
            float phi = distPhi(gen); // Azimuthal angle
            float rho = distRho(gen) * endRadius; // Radius of the sphere

            particleArray[i] = rho * cos(phi); // x
            particleArray[i+1] = rho * sin(phi); // y
            particleArray[i+2] = distZ(gen); // z
            particleArray[i+3] = 1.5e10f * G; // m in kg * G
        }

        // set velocities to give a spinning motion
        for (unsigned int i = 11; i < n * 7; i += 7) {
            // Radial vector
            float rx = particleArray[i-4];
            float ry = particleArray[i-3];
            float rz = particleArray[i-2];

            float r = sqrt(rx * rx + ry * ry + rz * rz);

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
            float length = sqrt(vx * vx + vy * vy + vz * vz) + 0.0001f;
            vx /= length;
            vy /= length;
            vz /= length;

            // Scale the velocity vector to the desired speed
            float speed = 200.0f / r; // Adjust this value as needed
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