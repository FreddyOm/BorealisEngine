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
- [x] Simple Window
- [ ] [Job System](https://github.com/FreddyOm/BorealisJobs)
- [ ] Custom Memory Allocators
  - [x] Stack Allocator
  - [x] Pool Allocator
  - [ ] Frame Allocator
  - [ ] Custom RefCnt Pointer
- [ ] Input System
  - [ ] Mouse & Keyboard
  - [ ] Controller (XInput)
- [ ] Basic D3D11 Rendering Implementation

## Dependencies

## Sources
