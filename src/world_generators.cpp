#include <cmath>

#include "world_generators.hpp"
#include "utilities.hpp"

void SphereGenerator(Position* positions, Velocity* velocites, const int n) {
    if (positions != nullptr) {
        delete[] positions;
    }

    if (velocites != nullptr) {
        delete[] velocites;
    }

    positions = new Position[n];
    velocites = new Velocity[n];

    float endRadius = 1.0f;
    float spacing = endRadius / n;

    // set positions
    for (int i = 0; i < n; i++) {
        double theta = acos(2 * rand() / double(RAND_MAX) - 1); // Polar angle
        double phi = rand() / double(RAND_MAX) * 2 * M_PI; // Azimuthal angle
        double radius = endRadius; // Radius of the sphere

        glPositions[i].x = radius * sin(theta) * cos(phi);
        glPositions[i].y = radius * sin(theta) * sin(phi);
        glPositions[i].z = radius * cos(theta);
        glPositions[i].m = 1;
    }

    // set velocities
    for (int i = 0; i < *nPtr; i++) {
        clVelocities[i].x = 0.0f;
        clVelocities[i].y = 0.0f;
        clVelocities[i].z = 0.0f;
    }
}
void WhateverGenerator(Position* positions, Velocity* velocites, const int n) {

}