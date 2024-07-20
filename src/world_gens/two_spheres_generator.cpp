#include <cmath>
#include <random>

#include "world_generators.hpp"
#include "../state.hpp"

namespace worldgens {

    void TwoSpheresGenerator::Generate(float*& particleArray, unsigned int n) {
        WorldGenerator::Generate(particleArray, n);

        float endRadius = 1.0f;
        float spacing = endRadius / n;

        // random generator
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> dist(0.0f, 1.0f);
        std::uniform_real_distribution<float> distPhi(0.0f, 2.0f * static_cast<float>(M_PI));

        // set positions
        for (unsigned int i = 0; i < n * 7; i += 7) {
            bool firstHalf = i < (n * 7) / 2;

            float theta = acos(2 * dist(gen) - 1); // Polar angle
            float phi = distPhi(gen); // Azimuthal angle
            float radius = dist(gen) * endRadius; // Radius of the sphere

            float xOffset = firstHalf ? -endRadius : endRadius;

            particleArray[i] = radius * sin(theta) * cos(phi) + xOffset; // x
            particleArray[i+1] = radius * sin(theta) * sin(phi); // y
            particleArray[i+2] = radius * cos(theta); // z
            particleArray[i+3] = 1.0e11f * G; // m in kg * G
        }

        // set velocities to give a spinning motion
        for (unsigned int i = 4; i < n * 7; i += 7) {
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
            float length = sqrt(vx * vx + vy * vy + vz * vz) + *state::simulation::EpsilonPtr;
            vx /= length;
            vy /= length;
            vz /= length;

            // Scale the velocity vector to the desired speed
            float speed = 10.0f; // Adjust this value as needed
            particleArray[i] = vx * speed;
            particleArray[i+1] = vy * speed;
            particleArray[i+2] = vz * speed;
        }
    }

    std::string TwoSpheresGenerator::ToString() const {
        return "TWO_SPHERES";
    }

    std::unique_ptr<WorldGenerator> TwoSpheresGenerator::Clone() const {
        return std::make_unique<TwoSpheresGenerator>(*this);
    }
}