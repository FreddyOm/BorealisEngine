# Borealis Engine
![CMake Multi Platform Build](https://github.com/FreddyOm/BorealisEngine/actions/workflows/cmake-win-debug.yml/badge.svg)
![CMake Multi Platform Build](https://github.com/FreddyOm/BorealisEngine/actions/workflows/cmake-win-release.yml/badge.svg)
![CMake Multi Platform Build](https://github.com/FreddyOm/BorealisEngine/actions/workflows/cmake-linux-debug.yml/badge.svg)
![CMake Multi Platform Build](https://github.com/FreddyOm/BorealisEngine/actions/workflows/cmake-linux-release.yml/badge.svg)

**A C++ based 3D high-performance engine with aim for AAA graphics, fast iteration and great tooling.**

> Note: This project is currently WIP and systems are being transferred from my old project [Creative Chaos Engine](https://github.com/FreddyOm/CreativeChaosEngine).
> Visit this project for older, but more extensive code.

## Setup 

For the setup of the project you need a installation of **git** and **cmake**.
To setup the project, follow the steps below.

1. Navigate to your desired directory
2. Clone this repository.
```
git clone -b main https://github.com/FreddyOm/BorealisEngine.git
```
3. Navigate into the repository
```
cd BorealisEngine
```
4. Build the project with CMake
```
cmake -S . -B ./build
```


## Features
- [x] Logging
  - [x] Info Log
  - [x] Warning Log
  - [x] Error Log
  - [x] Assertions
- [x] Simple Window
- [ ] [Job System](https://github.com/FreddyOm/BorealisJobs)
- [x] Custom Memory Allocators
  - [x] Stack Allocator
  - [x] Pool Allocator
  - [x] Frame Allocator
  - [x] Custom RefCnt Pointer
  - [x] Heap Allocator
  - [x] Defragmentation
- [ ] Input System
  - [ ] Mouse & Keyboard
  - [ ] Controller (XInput)
- [ ] Basic D3D11 Rendering Implementation
- [ ] Basic D3D12 Rendering Implementation
- [ ] Advanced Rendering Features
  - [ ] Deferred Shading & Texturing
  - [ ] Depth of Field (see: Tiny Glades)
  - [ ] VFX
  - [ ] Volumetric Fog
  - [ ] Procedural Shading
- [ ] Physics Implementation
  - [ ] Nvidia PhysX
- [ ] Entity Component System
  - [ ] Jobified updating of ECS data buffers (see JobSystem above)
- [ ] String Class
- [ ] Runtime Debugging Toolkit
  - [ ] Dear ImGui Interface
  - [ ] Console Implementation
  - [ ] Rendering Options
  - [ ] Memory Usage
- [ ] Custom Profiler / Profiler integration
  - [ ] Correct representation of jobs

## Dependencies

## Sources
