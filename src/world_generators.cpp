#include <cmath>
#include <stdio.h>

#include "globals.hpp"
#include "utilities.hpp"
#include "world_generators.hpp"

namespace worldgenerators {
    void SphereGenerator(Position*& positions, Velocity*& velocites, const int n);
    void SphereShellGenerator(Position*& positions, Velocity*& velocites, const int n);

    GeneratorFunction getGenerator(GeneratorType type) {
        switch(type) {
            case GeneratorType::SPHERE:
                return SphereGenerator;
            case GeneratorType::SPHERE_SHELL:
                return SphereShellGenerator;
            default:
                return nullptr; // or handle the error appropriately
        }
    }

    void SphereGenerator (Position*& positions, Velocity*& velocites, const int n)
    {
        // TODO: implement
    }

    void SphereShellGenerator (Position*& positions, Velocity*& velocites, const int n)
    {
        if (positions != nullptr) {
            delete[] positions;
        }

        if (velocites != nullptr) {
            delete[] velocites;
        }

        positions = new Position[MAX_N];
        velocites = new Velocity[MAX_N];

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
            velocites[i].x = 0.0f;
            velocites[i].y = 0.0f;
            velocites[i].z = 0.0f;
        }
    }
}