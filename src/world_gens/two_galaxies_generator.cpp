#include <cmath>
#include <random>

#include "world_generators.hpp"
#include "../state.hpp"

namespace worldgens {

    void TwoGalaxiesGenerator::Generate(float*& particleArray, unsigned int n) {
        WorldGenerator::Generate(particleArray, n);

        float endRadius = 1.0f;
        float galaxyDistance = 5.0f;
        float spacing = endRadius / n;

        // random generator
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> distZ(0.0f, 0.1f);
        std::uniform_real_distribution<float> distRho(0.7f, 1.0f);
        std::uniform_real_distribution<float> distPhi(0.0f, 2.0f * static_cast<float>(M_PI));

        // first heavier particle
        particleArray[0] = -endRadius*galaxyDistance; // x
        particleArray[1] = 0.0f; // y
        particleArray[2] = 0.0f; // z
        particleArray[3] = 6e14f * G; // m in kg * G

        // second heavier particle
        particleArray[7] = endRadius*galaxyDistance; // x
        particleArray[8] = 0.0f; // y
        particleArray[9] = 0.0f; // z
        particleArray[10] = 6e14f * G; // m in kg * G

        // set positions
        for (unsigned int i = 14; i < n * 7; i += 7) {
            float phi = distPhi(gen); // Azimuthal angle
            float rho = distRho(gen) * endRadius; // Radius of the sphere

            float xOffset = i < n * 7 / 2 ? -endRadius*galaxyDistance : endRadius*galaxyDistance;

            particleArray[i] = rho * cos(phi) + xOffset; // x
            particleArray[i+1] = rho * sin(phi); // y
            particleArray[i+2] = distZ(gen); // z
            particleArray[i+3] = 1.5e10f * G; // m in kg * G
        }

        // set velocities to give a spinning motion
        for (unsigned int i = 18; i < n * 7; i += 7) {
            // Determine which sun the particle is orbiting
            float sunX = i < n * 7 / 2 ? -endRadius*galaxyDistance : endRadius*galaxyDistance;

            // Radial vector from the sun to the particle
            float rx = particleArray[i-4] - sunX;
            float ry = particleArray[i-3];
            float rz = particleArray[i-2];

            float r = sqrt(rx * rx + ry * ry + rz * rz);

            // Tangential vector (cross product with (0, 0, 1))
            float vx = -ry;
            float vy = rx;
            float vz = 0.0f;

            // Normalize the tangential vector, minimum distance so we don't explode
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

    std::string TwoGalaxiesGenerator::ToString() const {
        return "TWO_GALAXIES";
    }

    std::unique_ptr<WorldGenerator> TwoGalaxiesGenerator::Clone() const {
        return std::make_unique<TwoGalaxiesGenerator>(*this);
    }
}