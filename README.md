![preview](preview.gif)
# About this project

This project showcases **OpenCL/OpenGL** interoperability through a simulation of gravitational forces acting on n particles. 
Employing a brute-force algorithm with O(n²) complexity, it leverages the power of parallel computing to manage and render the simulation efficiently. 
The project is developed entirely in C++, utilizing **GLFW 3**, **GLAD**, and **GLM** for high-performance rendering with OpenGL, and features a user-friendly interface built with ImGui. 
Adhering to the OpenCL 1.2 specification, it demonstrates the seamless integration of compute and graphics processing for sophisticated simulations.

> [!NOTE]
> 
> This project is designed for use with a dedicated graphics card. While OpenCL and OpenGL can theoretically run without a GPU, this program assumes you have both a GPU and the necessary GPU drivers installed.

# Usage

There are some pre-built executables for **Windows x86-64/MacOS ARM64/Linux x86-64**.
These were all tested on an NVIDIA GPU but should work with other vendors, granted the **OpenCL/OpenGL sharing extension** is supported by the device.
You can find the binaries [here](https://github.com/22ms/n-body-sim/releases/latest).

> [!NOTE]
> 
> The masses are **pre-multiplied with the gravitational constant G** to reduce the load on the GPU. If you want to change existing world generators or add new ones, you must edit the world generators source files in **src/world_gens**. The conversion from OpenGL units to real-life units is described in **state.cpp**. The color of a particle represents its speed.
>
> Some initial parameters can also be modified in **state.cpp** and adjusted at runtime using the ImGui user interface.

## Controls

- Use <kbd>w</kbd>/<kbd>a</kbd>/<kbd>s</kbd>/<kbd>d</kbd> for **up**/**left**/**down**/**right** respectively
- Hold <kbd>Right Click</kbd> and move the mouse to look around the scene
- Scroll <kbd>Mouse Wheel</kbd> to change the camera speed

# Building from source

## Linux

### Install dependencies

```console
sudo apt-get update
sudo apt-get install -y \
    build-essential cmake pkg-config python3 git \
    libxkbcommon-dev xorg-dev libwayland-dev
```

## MacOS

### Install C++ compiler

```console
xcode-select --install
```

### Install Homebrew package manager

1. Run the following command:
```console
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
```
2. Follow installer instructions to add brew to PATH

### Install dependencies

```console
brew update
brew install cmake python3
```

## Windows

### Install a C++ compiler (e.g. MinGW-w64)

If you choose to install MinGW-w64, download and install [MSYS2](https://www.msys2.org/). Add the `mingw-w64-x86_64-gcc` and `mingw-w64-x86_64-make` package from within the MSYS console:
```console
pacman -S mingw-w64-x86_64-gcc
pacman -S mingw-w64-x86_64-make
```
Make sure the binary directory is added to PATH before executing the CMake commands below.
For MSYS2, the PATH should be `C:\msys64\mingw64\bin` by default.

### Install CMake (3.10+)

Download and install [CMake](https://cmake.org/download/)
Make sure to set the correct PATH.

### Install Git for Windows

Download and install [Git](https://git-scm.com/download/win)
Make sure to set the correct PATH.

# Building

### Clone this repository

```console
git clone https://github.com/22ms/n-body-sim.git
```

### Set up your build directory

```console
mkdir build
cd build
```

### Generate the buildfiles

On **MacOS and Linux**:
```console
cmake ..
```
On **Windows** (given you installed MinGW-w64 with MSYS2):
```console
cmake -G "MinGW Makefiles" -DCMAKE_CXX_COMPILER="C:/msys64/mingw64/bin/g++.exe" ..
```

### Compile the project

```console
cmake --build . -t n-body
```
You should now have an executable called **n-body** inside of the build directory.

# Program architechture

```mermaid
---
title: N-body simulation architechture
---
classDiagram
note for State "These are not classes since they all manage one global state"
note for Camera "Actual classes used by the global state"
note for OpenCLWrapper "Acquires GL buffer and simulates timestep on it"
note for OpenGLWrapper "Manages host arrays and buffer, renders out the particles"
note for ImGuiWrapper "Frontend for parameters, updates WorldState"

OpenGLWrapper..>WorldGenerator
OpenCLWrapper..>Kernel
OpenGLWrapper..>Shader
OpenGLWrapper..>Camera

ImGuiWrapper-->OpenGLWrapper
OpenCLWrapper-->OpenGLWrapper
OpenGLWrapper-->OpenCLWrapper

ImGuiWrapper-->State
OpenCLWrapper-->State
OpenGLWrapper-->State
namespace Global {
  class State {
    +MAX_N
    +NPtr
    +TimeScalePtr
    +EpsilonPtr
    +WorldGeneratorPtr
    +...
  }
  class OpenGLWrapper {
    +ParticleArray
    +ParticleBuffer
    +...
    +Initialize()
    +Render()
    +...()
  }
  class OpenCLWrapper {
    -cmdQueue
    +Initialize()
    +Simulate()
    +...()
  }
  class ImGuiWrapper {
    +Initialize()
    +Display()
  }
}
namespace Classes {
  class Shader {
    +ID
    +Shader(filepath)
    +Use()
    +SetUniform()
    +...()
  }
  class Camera {
    +Position
    +...
    +ProcessInput()
    +...()
  }
  class Kernel {
    +Kernel(filepath)
    +GetKernel()
  }
  class WorldGenerator {
    +Generate(arrays) virtual void
    +ToString() virtual string
    +Clone()
    +...()
  }
}

```
