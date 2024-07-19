#pragma once

#include <memory>
#include <vector>
#include <string>

namespace utilities {
    struct Position;
    struct Velocity;
}

namespace worldgens {
    extern const double G;

    class WorldGenerator {
        public:
            virtual void Generate(float*& particleArray, int n) = 0;
            virtual std::string ToString() const = 0;
            virtual std::unique_ptr<WorldGenerator> Clone() const = 0;
            bool IsSameType(const WorldGenerator& other) const;
            virtual ~WorldGenerator() = default;
    };

    std::unique_ptr<WorldGenerator> FromString(std::string str);

    class GalaxyGenerator : public WorldGenerator {
        public:
            void Generate(float*& particleArray, int n) override;
            std::string ToString() const override;
            std::unique_ptr<WorldGenerator> Clone() const override;
    };

    class TwoGalaxiesGenerator : public WorldGenerator {
        public:
            void Generate(float*& particleArray, int n) override;
            std::string ToString() const override;
            std::unique_ptr<WorldGenerator> Clone() const override;
    };

    class SphereGenerator : public WorldGenerator {
        public:
            void Generate(float*& particleArray, int n) override;
            std::string ToString() const override;
            std::unique_ptr<WorldGenerator> Clone() const override;
    };

    class SphereShellGenerator : public WorldGenerator {
        public:
            void Generate(float*& particleArray, int n) override;
            std::string ToString() const override;
            std::unique_ptr<WorldGenerator> Clone() const override;
    };

    class TwoSpheresGenerator : public WorldGenerator {
        public:
            void Generate(float*& particleArray, int n) override;
            std::string ToString() const override;
            std::unique_ptr<WorldGenerator> Clone() const override;
    };
}