#ifndef WORLD_GENERATORS_HPP
#define WORLD_GENERATORS_HPP

#include <vector>

struct Position;
struct Velocity;

namespace worldgenerators {
    class WorldGenerator {
        public:
            virtual void Generate(Position*& positions, Velocity*& velocities, int n) = 0;
            virtual const char* ToString() = 0;
    };

    void Initialize(WorldGenerator* initialWorldGenerator);
    extern std::vector<WorldGenerator*> WorldGeneratorOptions;

    class SphereGenerator : public WorldGenerator {
        public:
            void Generate(Position*& positions, Velocity*& velocities, int n) override;
            const char* ToString() override;
    };

    class SphereShellGenerator : public WorldGenerator {
        public:
            void Generate(Position*& positions, Velocity*& velocities, int n) override;
            const char* ToString() override;
    };
}

#endif