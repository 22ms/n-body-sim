#pragma once

namespace utilities {
    struct Position
    {
        float x, y, z, m;
    };

    struct Velocity
    {
        float x, y, z;
        float EMPTY_FOR_ALIGNMENT = 0;
    };
}