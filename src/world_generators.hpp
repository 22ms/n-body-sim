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
            virtual bool isSameType(const WorldGenerator* other) const = 0;
    };

    void Initialize(WorldGenerator* initialWorldGenerator);
    extern std::vector<WorldGenerator*> WorldGeneratorOptions;

    class SphereGenerator : public worldgenerators::WorldGenerator {
        public:
            void Generate(Position*& positions, Velocity*& velocities, int n) override;
            const char* ToString() override;
            bool isSameType(const WorldGenerator* other) const override;
    };

    class SphereShellGenerator : public WorldGenerator {
        public:
            void Generate(Position*& positions, Velocity*& velocities, int n) override;
            const char* ToString() override;
            bool isSameType(const WorldGenerator* other) const override;
    };

    class BlackHoleSphereGenerator : public WorldGenerator {
        public:
            void Generate(Position*& positions, Velocity*& velocities, int n) override;
            const char* ToString() override;
            bool isSameType(const WorldGenerator* other) const override;
    };

    class TwoSpheresGenerator : public WorldGenerator {
        public:
            void Generate(Position*& positions, Velocity*& velocities, int n) override;
            const char* ToString() override;
            bool isSameType(const WorldGenerator* other) const override;
    };
}

#endif