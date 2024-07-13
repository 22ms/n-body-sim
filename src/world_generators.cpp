#include <cmath>
#include <stdio.h>
#include <vector>
#include <cstdlib>

#include "globals.hpp"
#include "utilities.hpp"
#include "world_generators.hpp"

namespace worldgenerators {

    std::vector<WorldGenerator*> WorldGeneratorOptions;

    void Initialize(WorldGenerator* initialWorldGenerator) {
        WorldGeneratorOptions.push_back(initialWorldGenerator);
        WorldGeneratorOptions.push_back(new SphereGenerator());
        // add more options if needed
    }

    void SphereGenerator::Generate(Position*& positions, Velocity*& velocities, int n) {
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

    const char* SphereGenerator::ToString() {
        return "SPHERE";
    }

    void SphereShellGenerator::Generate(Position*& positions, Velocity*& velocities, int n) {
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

    const char* SphereShellGenerator::ToString() {
        return "SPHERE_SHELL";
    }
}