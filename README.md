![preview](preview.gif)
# About the project

This project showcases OpenCL/OpenGL interoperability through a simulation of gravitational forces acting on n particles. 
Employing a brute-force algorithm with O(n²) complexity, it leverages the power of parallel computing to manage and render the simulation efficiently. 
The project is developed entirely in C++, utilizing GLFW 3, GLAD, and GLM for high-performance rendering with OpenGL, and features a user-friendly interface built with ImGui. 
Adhering to the OpenCL 1.2 specification, it demonstrates the seamless integration of compute and graphics processing for sophisticated simulations.

# Building

## Linux

### Install dependencies

```console
sudo apt-get update
sudo apt-get install -y \
    build-essential cmake \
    xorg-dev \
    mesa-common-dev libglu1-mesa-dev freeglut3-dev \
    libglew-dev libglfw3-dev libglm-dev \
    libao-dev libmpg123-dev \
    wayland-protocols libwayland-dev
```
> [!NOTE]
> 
> This project is designed for use with a dedicated graphics card. While OpenCL and OpenGL can theoretically run without a GPU, this program assumes you have both a GPU and the necessary GPU drivers installed.

### Build using cmake
```console
mkdir build
cd build
build ..
```

### Install using cmake
```console
cmake --build . --target install --config Debug
```

### Run
```console
./n-body
```

# Program architechture

```mermaid
---
title: N-body simulation architechture
---
classDiagram
note for WorldState "These are not classes since they all manage one global state"
note for Camera "Actual classes used by the global state"
note for OpenCLWrapper "Acquires GL buffer and simulates timestep on it"
note for OpenGLWrapper "Manages host arrays and buffer, renders out the particles"
note for ImGuiWrapper "Frontend for parameters, updates WorldState"

WorldState..>WorldGenerator
OpenCLWrapper..>Kernel
OpenGLWrapper..>Shader
OpenGLWrapper..>Camera

OpenCLWrapper-->Config
OpenGLWrapper-->Config
ImGuiWrapper-->Config
WorldState-->Config

ImGuiWrapper-->OpenGLWrapper
OpenCLWrapper-->OpenGLWrapper
OpenGLWrapper-->OpenCLWrapper

ImGuiWrapper-->WorldState
OpenCLWrapper-->WorldState
OpenGLWrapper-->WorldState
namespace Global {
  class Config {
    +InitialSimulationParameters: const
    +InitialWindowParameters: const
  }
  class WorldState {
    +CurrentSimulationParameters
    +Initialize()
    +...()
  }
  class OpenGLWrapper {
    +Positions
    +Velocities
    +PosBuffer
    +...
    +Initialize()
    +Render()
    +...()
  }
  class OpenCLWrapper {
    -VelBuffer
    +CmdQueue
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
