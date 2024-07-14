#include <cmath>
#include <string>

#include "../world_generators.hpp"
#include "../utilities.hpp"
#include "../globals.hpp"

namespace worldgens {

    void BlackHoleSphereGenerator::Generate(Position*& positions, Velocity*& velocities, int n) {
        if (positions != nullptr) {
            delete[] positions;
        }

        if (velocities != nullptr) {
            delete[] velocities;
        }

        positions = new Position[MAX_N];
        velocities = new Velocity[MAX_N];

        float endRadius = 1.0f;
        float spacing = endRadius / n;

        positions[0].x = 0.0f;
        positions[0].y = 0.0f;
        positions[0].z = 0.0f;
        positions[0].m = 2.0f;

        // set positions
        for (int i = 1; i < n; i++) {
            double theta = acos(2 * rand() / double(RAND_MAX) - 1); // Polar angle
            double phi = rand() / double(RAND_MAX) * 2 * M_PI; // Azimuthal angle
            double radius = rand() / double(RAND_MAX)* endRadius; // Radius of the sphere

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

    std::string BlackHoleSphereGenerator::ToString() const {
        return "BLACK_HOLE_SPHERE";
    }

    std::unique_ptr<WorldGenerator> BlackHoleSphereGenerator::clone() const {
        return std::make_unique<BlackHoleSphereGenerator>(*this);
    }
}