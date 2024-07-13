#include <cmath>

#include "../globals.hpp"
#include "../utilities.hpp"
#include "../world_generators.hpp"

namespace worldgenerators {

    void SphereShellGenerator::Generate(Position*& positions, Velocity*& velocities, int n)
    {
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

        // set positions
        for (int i = 0; i < n; i++) {
            double theta = acos(2 * rand() / double(RAND_MAX) - 1); // Polar angle
            double phi = rand() / double(RAND_MAX) * 2 * M_PI; // Azimuthal angle
            double radius = endRadius; // Radius of the sphere

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

    const char* SphereShellGenerator::ToString()
    {
        return "SPHERE_SHELL";
    }

    bool SphereShellGenerator::isSameType(const WorldGenerator* other) const {
        return dynamic_cast<const SphereShellGenerator*>(other) != nullptr;
    }

}