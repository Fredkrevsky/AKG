# AKG
Algorithms of Computer Graphics (6th semester BSUIR)

## Dependences

- [Conan2](https://conan.io/) — packet manager for CMake.
- [SFML 2.6.2](https://www.sfml-dev.org/) — multimedia library.

## Build and run

   ```bash
   pip install conan               # if not installed
   conan profile detect            # for the first time
   cd Lab1                         # or Lab2, ...
   conan build . --build=missing
   ./build/Release/Lab1            # or Lab2, ...
   ```

## Requirements

- Lab1: requires .obj files in folder ./models/
