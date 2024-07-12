#ifndef UTILITIES_HPP
#define UTILITIES_HPP

struct Position
{
    float x, y, z, m;
};

struct Velocity
{
    float x, y, z;
    float EMPTY_FOR_ALIGNMENT = 0;
};

#endif