#ifndef WORLD_GENERATORS_HPP
#define WORLD_GENERATORS_HPP

struct Position;
struct Velocity;

typedef void (*GeneratorFunction)(Position*&, Velocity*&, const int);

namespace worldgenerators {
    enum class GeneratorType {
        SPHERE,
        SPHERE_SHELL
    };

    GeneratorFunction getGenerator(GeneratorType type);
}

#endif