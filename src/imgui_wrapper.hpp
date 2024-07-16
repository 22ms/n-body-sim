#pragma once

namespace worldgens {
    class WorldGenerator;
}

class GLFWwindow;
class Position;
class Velocity;

namespace imguiwrapper {
    extern void Initialize ();
    extern void Display ();
    extern void Cleanup ();
}