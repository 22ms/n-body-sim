#ifndef WORLD_GENERATORS_HPP
#define WORLD_GENERATORS_HPP

struct Position;
struct Velocity;

namespace worldgenerators {
    void SphereGenerator(Position* positions, Velocity* velocites, const int n);
    void SphereShellGenerator(Position*& positions, Velocity*& velocites, const int n);
}

#endif