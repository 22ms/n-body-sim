#pragma once

#include <memory>
#include <vector>
#include <string>

struct Position;
struct Velocity;

namespace worldgens {
    enum class WorldType {
        SPHERE,
        SPHERE_SHELL,
        TWO_SPHERES,
        BLACK_HOLE
    };

    class WorldGenerator {
        public:
            virtual void Generate(Position*& positions, Velocity*& velocities, int n) = 0;
            virtual std::string ToString() const = 0;
            virtual std::unique_ptr<WorldGenerator> clone() const = 0;
            bool isSameType(const WorldGenerator& other) const;
    };

    std::unique_ptr<WorldGenerator> CreateFromWorldType (WorldType worldType);

    class SphereGenerator : public WorldGenerator {
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

    class TwoSpheresGenerator : public WorldGenerator {
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
}