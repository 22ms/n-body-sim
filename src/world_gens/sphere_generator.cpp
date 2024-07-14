#include <cmath>

#include "../config.hpp"
#include "../utilities.hpp"
#include "world_generators.hpp"

namespace worldgens {

    void SphereGenerator::Generate(std::vector<utilities::Position>& positions, std::vector<utilities::Velocity>& velocities, int n) {
        WorldGenerator::Generate(positions, velocities, n);

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

        // set velocities to give a spinning motion
        for (int i = 0; i < n; i++) {
            // Radial vector
            float rx = positions[i].x;
            float ry = positions[i].y;
            float rz = positions[i].z;

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
            float speed = 10.0f; // Adjust this value as needed
            velocities[i].x = vx * speed;
            velocities[i].y = vy * speed;
            velocities[i].z = vz * speed;

        }
    }

    std::string SphereGenerator::ToString() const {
        return "SPHERE";
    }

    std::unique_ptr<WorldGenerator> SphereGenerator::clone() const {
        return std::make_unique<SphereGenerator>(*this); 
    }

}