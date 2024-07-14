#ifndef WORLD_GENERATORS_HPP
#define WORLD_GENERATORS_HPP

#include <memory>
#include <vector>
#include <string>

struct Position;
struct Velocity;

namespace worldgens {
    class WorldGenerator {
        public:
            virtual void Generate(Position*& positions, Velocity*& velocities, int n) = 0;
            virtual std::string ToString() const = 0;
            virtual std::unique_ptr<WorldGenerator> clone() const = 0;
            bool isSameType(const WorldGenerator& other) const;
    };

    void Initialize(WorldGenerator& initialWorldGenerator);
    void Cleanup();

    extern std::vector<std::unique_ptr<WorldGenerator>> WorldGeneratorOptions;
    extern std::unique_ptr<WorldGenerator> CurrentWorldGenerator;

    class SphereGenerator : public worldgens::WorldGenerator {
        public:
            void Generate(Position*& positions, Velocity*& velocities, int n) override;
            std::string ToString() const override;
            std::unique_ptr<WorldGenerator> clone() const override;
    };

    class SphereShellGenerator : public WorldGenerator {
        public:
            void Generate(Position*& positions, Velocity*& velocities, int n) override;
            std::string ToString() const override;
            std::unique_ptr<WorldGenerator> clone() const override;
    };

    class BlackHoleSphereGenerator : public WorldGenerator {
        public:
            void Generate(Position*& positions, Velocity*& velocities, int n) override;
            std::string ToString() const override;
            std::unique_ptr<WorldGenerator> clone() const override;
    };

    class TwoSpheresGenerator : public WorldGenerator {
        public:
            void Generate(Position*& positions, Velocity*& velocities, int n) override;
            std::string ToString() const override;
            std::unique_ptr<WorldGenerator> clone() const override;
    };
}

#endif